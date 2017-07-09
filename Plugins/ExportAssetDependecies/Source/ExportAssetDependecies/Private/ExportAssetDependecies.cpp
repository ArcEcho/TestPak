// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

#include "ExportAssetDependecies.h"
#include "ExportAssetDependeciesStyle.h"
#include "ExportAssetDependeciesCommands.h"
#include "Misc/MessageDialog.h"
#include "Framework/MultiBox/MultiBoxBuilder.h"
#include "AssetRegistryModule.h"
#include "ARFilter.h"
#include "ModuleManager.h"
#include "ISettingsModule.h"
#include "PlatformFile.h"
#include "PlatformFilemanager.h"
#include "FileHelper.h"
#include "json.h"

#include "Framework/Notifications/NotificationManager.h"
#include "Widgets/Notifications/SNotificationList.h"

#include "ExportAssetDependeciesSettings.h"

#include "LevelEditor.h"

DEFINE_LOG_CATEGORY(LogExportAssetDependecies);


static const FName ExportAssetDependeciesTabName("ExportAssetDependecies");

#define LOCTEXT_NAMESPACE "FExportAssetDependeciesModule"

void FExportAssetDependeciesModule::StartupModule()
{
    // This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module

    FExportAssetDependeciesStyle::Initialize();
    FExportAssetDependeciesStyle::ReloadTextures();

    FExportAssetDependeciesCommands::Register();

    PluginCommands = MakeShareable(new FUICommandList);

    PluginCommands->MapAction(
        FExportAssetDependeciesCommands::Get().PluginAction,
        FExecuteAction::CreateRaw(this, &FExportAssetDependeciesModule::PluginButtonClicked),
        FCanExecuteAction());

    FLevelEditorModule& LevelEditorModule = FModuleManager::LoadModuleChecked<FLevelEditorModule>("LevelEditor");

    {
        TSharedPtr<FExtender> MenuExtender = MakeShareable(new FExtender());
        MenuExtender->AddMenuExtension("WindowLayout", EExtensionHook::After, PluginCommands, FMenuExtensionDelegate::CreateRaw(this, &FExportAssetDependeciesModule::AddMenuExtension));

        LevelEditorModule.GetMenuExtensibilityManager()->AddExtender(MenuExtender);
    }

    {
        TSharedPtr<FExtender> ToolbarExtender = MakeShareable(new FExtender);
        ToolbarExtender->AddToolBarExtension("Settings", EExtensionHook::After, PluginCommands, FToolBarExtensionDelegate::CreateRaw(this, &FExportAssetDependeciesModule::AddToolbarExtension));

        LevelEditorModule.GetToolBarExtensibilityManager()->AddExtender(ToolbarExtender);
    }

    //Initialize setting, take care of this registry syntax
    {
        ISettingsModule* SettingsModule = FModuleManager::GetModulePtr<ISettingsModule>("Settings");
        if (SettingsModule != nullptr)
        {
            // ClassViewer Editor Settings
            SettingsModule->RegisterSettings("Project", "Game", "Export Asset Dependencies",
                LOCTEXT("ExportAssetDependenciesSettingsName", "Export Asset Dependencies"),
                LOCTEXT("ExportAssetDependenciesSettingsDescription", "Export Asset Dependencies."),
                GetMutableDefault<UExportAssetDependeciesSettings>()
            );
        }
    }
}

void FExportAssetDependeciesModule::ShutdownModule()
{
    // This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
    // we call this function before unloading the module.
    FExportAssetDependeciesStyle::Shutdown();

    FExportAssetDependeciesCommands::Unregister();
}

void FExportAssetDependeciesModule::PluginButtonClicked()
{
    //If loading assets
    FAssetRegistryModule& AssetRegistryModule = FModuleManager::LoadModuleChecked<FAssetRegistryModule>(TEXT("AssetRegistry"));
    if (AssetRegistryModule.Get().IsLoadingAssets())
    {
        // We are still discovering assets, listen for the completion delegate before building the graph
        if (!AssetRegistryModule.Get().OnFilesLoaded().IsBoundToObject(this))
        {
            AssetRegistryModule.Get().OnFilesLoaded().AddRaw(this, &FExportAssetDependeciesModule::ExportAssetDependecies);
        }
    }
    else
    {
        ExportAssetDependecies();
    }
}


void FExportAssetDependeciesModule::AddMenuExtension(FMenuBuilder& Builder)
{
    Builder.AddMenuEntry(FExportAssetDependeciesCommands::Get().PluginAction);
}


void FExportAssetDependeciesModule::ExportAssetDependecies()
{
    //Validate settings
    auto CurrentSettings = GetMutableDefault<UExportAssetDependeciesSettings>();
    if (!CurrentSettings)
    {
        UE_LOG(LogExportAssetDependecies, Error, TEXT("Cannot read ExportAssetDependeciesSettings"));
        return;
    }

    auto ResultFileOutputPath = CurrentSettings->OutputPath.Path;
    if (!FPaths::ValidatePath(ResultFileOutputPath))
    {
        UE_LOG(LogExportAssetDependecies, Error, TEXT("Invalid output path£º %s"), *ResultFileOutputPath);
        return;
    }

    //Remove duplicated directories.
    //TODO

    //Get infos according to the settings.
    TMap<FString, TArray<FString>> Results;
    GatherDependenciesInfo(CurrentSettings, Results);

    //Write Results
    SaveDependicesInfo(ResultFileOutputPath, Results);
}


void FExportAssetDependeciesModule::GatherDependenciesInfo(const UExportAssetDependeciesSettings * CurrentSettings, TMap<FString, TArray<FString>> &Results)
{
    //If loading
    FAssetRegistryModule& AssetRegistryModule = FModuleManager::LoadModuleChecked<FAssetRegistryModule>(TEXT("AssetRegistry"));
    TArray<FString> ExpectedDirectories;
    for (auto &Diretory : CurrentSettings->ExportDirectories)
    {
        //Ignore empty directory
        if (!Diretory.Path.IsEmpty())
        {
            ExpectedDirectories.Push(Diretory.Path);
        }
    }

    if (ExpectedDirectories.Num() == 0)
    {
        //If the user doesn't set any valid directories, the game will be set as default.
        FText DialogText = FText(
            LOCTEXT("NoDirectoriesSetDialogText", "There is no directories set in config, the content dir will be set as default!")
        );
        FMessageDialog::Open(EAppMsgType::Ok, DialogText);

        ExpectedDirectories.Push("/Game");
    }

    //Scan assets
    TArray<FAssetData> AssetData;
    FARFilter Filter;
    for (auto &Directory : ExpectedDirectories)
    {
        Filter.PackagePaths.Add(*Directory);
    }
    Filter.bRecursivePaths = CurrentSettings->bShouldExportRecursively;
    AssetRegistryModule.Get().GetAssets(Filter, AssetData);

    //Collect final result.
    for (auto &ad : AssetData)
    {
        TArray<FString> &DependentList = Results.Add(ad.GetPackage()->GetName());
        TArray<FName> Dependencies;
        bool bGetDependenciesSuccess = AssetRegistryModule.Get().GetDependencies(*ad.GetPackage()->GetName(), Dependencies, EAssetRegistryDependencyType::Packages);
        if (bGetDependenciesSuccess)
        {
            UE_LOG(LogExportAssetDependecies, Log, TEXT("Dependencies of Package: %s"), *ad.GetPackage()->GetName());
            if (Dependencies.Num() != 0)
            {
                for (auto &d : Dependencies)
                {
                    if (CurrentSettings->bOnlyExportWhatInGameConent)
                    {
                        if (d.ToString().StartsWith("/Game"))
                        {
                            DependentList.Add(d.ToString());
                            UE_LOG(LogExportAssetDependecies, Log, TEXT("  %s"), *d.ToString());
                        }
                    }
                    else
                    {
                        DependentList.Add(d.ToString());
                        UE_LOG(LogExportAssetDependecies, Log, TEXT("   %s"), *d.ToString());
                    }
                }
            }
            else
            {
                UE_LOG(LogExportAssetDependecies, Log, TEXT("   No dependencies"));
            }
        }
    }
}


void FExportAssetDependeciesModule::SaveDependicesInfo(const FString &ResultFileOutputPath, const TMap<FString, TArray<FString>> &Results)
{
    //Construct JSON object.
    TSharedPtr<FJsonObject> RootJsonObject = MakeShareable(new FJsonObject);
    for (auto &ResultEntry : Results)
    {
        TArray< TSharedPtr<FJsonValue> > DependenciesEntry;
        for (auto &ValueElement : ResultEntry.Value)
        {
            DependenciesEntry.Add(MakeShareable(new FJsonValueString(ValueElement)));
        }

        RootJsonObject->SetArrayField(ResultEntry.Key, DependenciesEntry);
    }

    FString OutputString;
    auto JsonWirter = TJsonWriterFactory<>::Create(&OutputString);
    FJsonSerializer::Serialize(RootJsonObject.ToSharedRef(), JsonWirter);

    FString ResultFileFilename = FPaths::Combine(ResultFileOutputPath, TEXT("AssetDependencies.json"));
    bool bSaveSuccess = FFileHelper::SaveStringToFile(OutputString, *ResultFileFilename);
    if (bSaveSuccess)
    {
        auto Message = LOCTEXT("ExportAssetDependeciesSuccessNotification", "Succeed to export asset dependecies.");
        FNotificationInfo Info(Message);
        Info.bFireAndForget = true;
        Info.ExpireDuration = 5.0f;
        Info.bUseSuccessFailIcons = false;
        Info.bUseLargeFont = false;

        const FString HyperLinkText = ResultFileFilename;
        Info.Hyperlink = FSimpleDelegate::CreateStatic([](FString SourceFilePath)
        {
            FPlatformProcess::ExploreFolder(*(FPaths::GetPath(SourceFilePath)));
        }, HyperLinkText);
        Info.HyperlinkText = FText::FromString(HyperLinkText);

        FSlateNotificationManager::Get().AddNotification(Info);

        UE_LOG(LogExportAssetDependecies, Log, TEXT("%s. At %s"), *Message.ToString(), *ResultFileFilename);
    }
    else
    {
        UE_LOG(LogExportAssetDependecies, Error, TEXT("Failed to export %s"), *ResultFileFilename);
    }
}


void FExportAssetDependeciesModule::AddToolbarExtension(FToolBarBuilder& Builder)
{
    Builder.AddToolBarButton(FExportAssetDependeciesCommands::Get().PluginAction);
}

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FExportAssetDependeciesModule, ExportAssetDependecies)
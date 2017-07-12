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

    //TODO Validate setting input TargetLongPackageName.FilePath
    FString TargetLongPackageName = *CurrentSettings->TargetLongPackageName.FilePath;
    TArray<FString> DependicesInGameContentDir;
    TArray<FString> OtherDependices;
    FAssetRegistryModule& AssetRegistryModule = FModuleManager::LoadModuleChecked<FAssetRegistryModule>(TEXT("AssetRegistry"));
    GatherDependenciesInfoRecursively(AssetRegistryModule, TargetLongPackageName, DependicesInGameContentDir, OtherDependices);

    //Write Results
    SaveDependicesInfo(ResultFileOutputPath, TargetLongPackageName, DependicesInGameContentDir, OtherDependices);
}

void FExportAssetDependeciesModule::GatherDependenciesInfoRecursively(FAssetRegistryModule &AssetRegistryModule,
    const FString &TargetLongPackageName,
    TArray<FString> &DependicesInGameContentDir,
    TArray<FString> &OtherDependices)
{
    TArray<FName> Dependencies;
    bool bGetDependenciesSuccess = AssetRegistryModule.Get().GetDependencies(FName(*TargetLongPackageName), Dependencies, EAssetRegistryDependencyType::Packages);
    if (bGetDependenciesSuccess)
    {
        for (auto &d : Dependencies)
        {
            if (d.ToString().StartsWith("/Game"))
            {
                DependicesInGameContentDir.Add(d.ToString());
            }
            else
            {
                OtherDependices.Add(d.ToString());
            }

            GatherDependenciesInfoRecursively(AssetRegistryModule, d.ToString(), DependicesInGameContentDir, OtherDependices);
        }
    }
}

void FExportAssetDependeciesModule::SaveDependicesInfo(const FString &ResultFileOutputPath, const FString &TargetLongPackageName, const TArray<FString> &DependicesInGameContentDir, const TArray<FString> &OtherDependices)
{
    //Construct JSON object.
    TSharedPtr<FJsonObject> RootJsonObject = MakeShareable(new FJsonObject);

    //Write target TargetLongPackageName
    RootJsonObject->SetStringField("TargetLongPackageName", TargetLongPackageName);

    //Write dependencies in game content dir.
    {
        TArray< TSharedPtr<FJsonValue> > DependenciesEntry;
        for (auto &d : DependicesInGameContentDir)
        {
            DependenciesEntry.Add(MakeShareable(new FJsonValueString(d)));
        }
        RootJsonObject->SetArrayField("DependenciesInGameContentDir", DependenciesEntry);
    }

    //Write dependencies not in game content dir.
    {
        TArray< TSharedPtr<FJsonValue> > DependenciesEntry;
        for (auto &d : OtherDependices)
        {
            DependenciesEntry.Add(MakeShareable(new FJsonValueString(d)));
        }
        RootJsonObject->SetArrayField("OtherDependencies", DependenciesEntry);
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
// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

#include "ExportAssetDependecies.h"
#include "ExportAssetDependeciesStyle.h"
#include "ExportAssetDependeciesCommands.h"

#include "ExportAssetDependeciesSettings.h"
#include "SlateBasics.h"
#include "SlateExtras.h"

#include "LevelEditor.h"

#include "Misc/MessageDialog.h"
#include "Framework/MultiBox/MultiBoxBuilder.h"
#include "Framework/Notifications/NotificationManager.h"
#include "Widgets/Notifications/SNotificationList.h"

#include "AssetRegistryModule.h"
#include "ARFilter.h"
#include "ModuleManager.h"
#include "ISettingsModule.h"
#include "PlatformFile.h"
#include "PlatformFilemanager.h"
#include "FileHelper.h"
#include "json.h"

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
            SettingsModule->RegisterSettings("Project", "Game", "ExportAssetDependencies",
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
    //TODO ArcEcho
    //Should check whether the game content is dirty.

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

struct FDependicesInfo
{
    TArray<FString> DependicesInGameContentDir;
    TArray<FString> OtherDependices;
};

void FExportAssetDependeciesModule::ExportAssetDependecies()
{
    //Validate settings
    auto CurrentSettings = GetMutableDefault<UExportAssetDependeciesSettings>();
    if (!CurrentSettings)
    {
        UE_LOG(LogExportAssetDependecies, Error, TEXT("Cannot read ExportAssetDependeciesSettings"));
        return;
    }

    //TODO ArcEcho
    //1.Check input paths validation
    //2.Check it has valid package path.
    if (CurrentSettings->PackagesToExport.Num() == 0)
    {
        ISettingsModule* SettingsModule = FModuleManager::GetModulePtr<ISettingsModule>("Settings");
        if (SettingsModule != nullptr)
        {
            //If there is no PackagesToExport set, just to the setting viewer.
            SettingsModule->ShowViewer("Project", "Game", "ExportAssetDependencies");

            //UE4 API to show an editor notification.
            auto Message = LOCTEXT("ExportAssetDependeciesNoValidTargetPackages", "No valid target packages set.");
            FNotificationInfo Info(Message);
            Info.bUseSuccessFailIcons = true;
            FSlateNotificationManager::Get().AddNotification(Info)->SetCompletionState(SNotificationItem::CS_Fail);

            UE_LOG(LogExportAssetDependecies, Log, TEXT("No valid target packages set."));
            return;
        }
    }

    TMap<FString, FDependicesInfo> DependicesInfos;
    for (auto &PackageFilePath : CurrentSettings->PackagesToExport)
    {
        FAssetRegistryModule& AssetRegistryModule = FModuleManager::LoadModuleChecked<FAssetRegistryModule>(TEXT("AssetRegistry"));

        FStringAssetReference AssetRef = PackageFilePath.FilePath;
        FString TargetLongPackageName = AssetRef.GetLongPackageName();

        if (FPackageName::DoesPackageExist(TargetLongPackageName))
        {
            auto &DependicesInfoEntry = DependicesInfos.Add(TargetLongPackageName);

            GatherDependenciesInfoRecursively(AssetRegistryModule, TargetLongPackageName, DependicesInfoEntry.DependicesInGameContentDir, DependicesInfoEntry.OtherDependices);
        }

        TArray<FName>  ACs;
        AssetRegistryModule.Get().GetAncestorClassNames(*TargetLongPackageName, ACs);
    }

    //Write Results
    auto ResultFileOutputPath = FPaths::Combine(FPaths::GameSavedDir(), "ExportAssetDependecies");
    SaveDependicesInfo(ResultFileOutputPath, DependicesInfos);
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
            //Pick out packages in game content dir.
            FString LongDependentPackageName = d.ToString();
            if (LongDependentPackageName.StartsWith("/Game"))
            {
                //Try find firstly to avoid duplicated entry.
                if (DependicesInGameContentDir.Find(LongDependentPackageName) == INDEX_NONE)
                {
                    DependicesInGameContentDir.Add(LongDependentPackageName);
                    GatherDependenciesInfoRecursively(AssetRegistryModule, LongDependentPackageName, DependicesInGameContentDir, OtherDependices);
                }
            }
            else
            {
                if (OtherDependices.Find(LongDependentPackageName) == INDEX_NONE)
                {
                    OtherDependices.Add(LongDependentPackageName);
                    GatherDependenciesInfoRecursively(AssetRegistryModule, LongDependentPackageName, DependicesInGameContentDir, OtherDependices);
                }
            }
        }
    }
}

void FExportAssetDependeciesModule::SaveDependicesInfo(const FString &ResultFileOutputPath, const TMap<FString, FDependicesInfo> &DependicesInfos)
{
    TSharedPtr<FJsonObject> RootJsonObject = MakeShareable(new FJsonObject);
    for (auto &DependicesInfoEntry : DependicesInfos)
    {
        //Write dependencies in game content dir.
        TSharedPtr<FJsonObject> EntryJsonObject = MakeShareable(new FJsonObject);
        {
            TArray< TSharedPtr<FJsonValue> > DependenciesEntry;
            for (auto &d : DependicesInfoEntry.Value.DependicesInGameContentDir)
            {
                DependenciesEntry.Add(MakeShareable(new FJsonValueString(d)));
            }
            EntryJsonObject->SetArrayField("DependenciesInGameContentDir", DependenciesEntry);
        }

        //Write dependencies not in game content dir.
        {
            TArray< TSharedPtr<FJsonValue> > DependenciesEntry;
            for (auto &d : DependicesInfoEntry.Value.OtherDependices)
            {
                DependenciesEntry.Add(MakeShareable(new FJsonValueString(d)));
            }
            EntryJsonObject->SetArrayField("OtherDependencies", DependenciesEntry);
        }

        RootJsonObject->SetObjectField(DependicesInfoEntry.Key, EntryJsonObject);
    }

    FString OutputString;
    auto JsonWirter = TJsonWriterFactory<>::Create(&OutputString);
    FJsonSerializer::Serialize(RootJsonObject.ToSharedRef(), JsonWirter);

    FString ResultFileFilename = FPaths::Combine(ResultFileOutputPath, TEXT("AssetDependencies.json"));
    //Attention to FFileHelper::EEncodingOptions::ForceUTF8 here. In some case,
    bool bSaveSuccess = FFileHelper::SaveStringToFile(OutputString, *ResultFileFilename, FFileHelper::EEncodingOptions::ForceUTF8WithoutBOM);
    if (bSaveSuccess)
    {
        //UE4 API to show an editor notification.
        auto Message = LOCTEXT("ExportAssetDependeciesSuccessNotification", "Succeed to export asset dependecies.");
        FNotificationInfo Info(Message);
        Info.bFireAndForget = true;
        Info.ExpireDuration = 5.0f;
        Info.bUseSuccessFailIcons = false;
        Info.bUseLargeFont = false;

        const FString HyperLinkText = ResultFileFilename;
        Info.Hyperlink = FSimpleDelegate::CreateStatic([](FString SourceFilePath)
        {
            FPlatformProcess::ExploreFolder(*(FPaths::ConvertRelativePathToFull(FPaths::GetPath(SourceFilePath))));
        }, HyperLinkText);
        Info.HyperlinkText = FText::FromString(HyperLinkText);

        FSlateNotificationManager::Get().AddNotification(Info)->SetCompletionState(SNotificationItem::CS_Success);

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
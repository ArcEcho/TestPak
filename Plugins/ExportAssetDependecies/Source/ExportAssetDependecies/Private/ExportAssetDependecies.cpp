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
    auto CurrentSettings = GetMutableDefault<UExportAssetDependeciesSettings>();

    if (!CurrentSettings)
    {
        UE_LOG(LogExportAssetDependecies, Error, TEXT("Cannot read ExportAssetDependeciesSettings"));
        return;
    }

    for (auto &Diretory : CurrentSettings->ExportDirectories)
    {
        UE_LOG(LogExportAssetDependecies, Error, TEXT("Setting D --- %s"), *Diretory.Path);

    }

    // Put your "OnButtonClicked" stuff here
    //FText DialogText = FText::Format(
    //						LOCTEXT("PluginButtonDialogText", "Add code to {0} in {1} to override this button's actions"),
    //						FText::FromString(TEXT("FExportAssetDependeciesModule::PluginButtonClicked()")),
    //						FText::FromString(TEXT("ExportAssetDependecies.cpp"))
    //				   );
    //FMessageDialog::Open(EAppMsgType::Ok, DialogText);


    FAssetRegistryModule& AssetRegistryModule = FModuleManager::LoadModuleChecked<FAssetRegistryModule>(TEXT("AssetRegistry"));

    if (AssetRegistryModule.Get().IsLoadingAssets())
    {
        // We are still discovering assets, listen for the completion delegate before building the graph
        //if (!AssetRegistryModule.Get().OnFilesLoaded().IsBoundToObject(this))
        //{
        //    AssetRegistryModule.Get().OnFilesLoaded().AddSP(this, &SReferenceViewer::OnInitialAssetRegistrySearchComplete);
        //}
    }

    UE_LOG(LogExportAssetDependecies, Warning, TEXT("++++++++++++++++++++++++"));

    TArray<FName> D;
    bool bD = AssetRegistryModule.Get().GetDependencies("/Game/Maps/InitialMap", D, EAssetRegistryDependencyType::Hard);
    for (auto &d : D)
    {
        UE_LOG(LogExportAssetDependecies, Warning, TEXT("D ---- %s"), *d.ToString());
    }

    TArray<FName> R;
    bool bR = AssetRegistryModule.Get().GetReferencers("/Game/Maps/InitialMap", R, EAssetRegistryDependencyType::Hard);
    for (auto &r : R)
    {
        UE_LOG(LogExportAssetDependecies, Warning, TEXT("R ---- %s"), *r.ToString());
    }

    UE_LOG(LogExportAssetDependecies, Warning, TEXT("++++++++++++++++++++++++"));

    /*  {
          TArray<FAssetData> ADs;
          AssetRegistryModule.Get().GetAllAssets(ADs, true);
          for (auto &ad : ADs)
          {
              UE_LOG(LogExportAssetDependecies, Warning, TEXT("GetAllAssets  ---- %s  %s"), *ad.GetFullName(), *ad.GetPackage()->GetName());
          }
      }
      {
          TArray<FAssetData> ADs;
          AssetRegistryModule.Get().GetAssetsByPackageName("/Game", ADs, true);
          for (auto &ad : ADs)
          {
              UE_LOG(LogExportAssetDependecies, Warning, TEXT("GetAssetsByPackageName  ---- %s  %s"), *ad.GetFullName(), *ad.GetPackage()->GetName());
          }
      }*/

    {

        TArray<FAssetData> AssetData;
        FARFilter Filter;
        Filter.PackagePaths.Add("/Game/MyData");
        Filter.bRecursivePaths = true;
        AssetRegistryModule.Get().GetAssets(Filter, AssetData);
        for (auto &ad : AssetData)
        {
            UE_LOG(LogExportAssetDependecies, Warning, TEXT("GetAssets  --- %s"), *ad.GetPackage()->GetName());
        }
    }

    UE_LOG(LogExportAssetDependecies, Log, TEXT("FExportAssetDependeciesModule ssdsdsd %d"), GetDefault<UExportAssetDependeciesSettings>()->ExportDirectories.Num());
}

void FExportAssetDependeciesModule::AddMenuExtension(FMenuBuilder& Builder)
{
    Builder.AddMenuEntry(FExportAssetDependeciesCommands::Get().PluginAction);
}


void FExportAssetDependeciesModule::AddToolbarExtension(FToolBarBuilder& Builder)
{
    Builder.AddToolBarButton(FExportAssetDependeciesCommands::Get().PluginAction);
}

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FExportAssetDependeciesModule, ExportAssetDependecies)
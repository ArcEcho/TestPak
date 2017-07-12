// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "ModuleManager.h"

class FToolBarBuilder;
class FMenuBuilder;
class UExportAssetDependeciesSettings;
class FAssetRegistryModule;

DECLARE_LOG_CATEGORY_EXTERN(LogExportAssetDependecies, Log, All);

class FExportAssetDependeciesModule : public IModuleInterface
{
public:

    /** IModuleInterface implementation */
    virtual void StartupModule() override;
    virtual void ShutdownModule() override;

    /** This function will be bound to Command. */
    void PluginButtonClicked();

private:

    void AddToolbarExtension(FToolBarBuilder& Builder);
    void AddMenuExtension(FMenuBuilder& Builder);

    void ExportAssetDependecies();

    void GatherDependenciesInfoRecursively(FAssetRegistryModule &AssetRegistryModule, const FString &TargetLongPackageName,
        TArray<FString> &DependicesInGameContentDir, TArray<FString> &OtherDependices);

    /** This will save the dependencies information to the OutputPath/AssetDependencies.json */
    void SaveDependicesInfo(const FString &ResultFileOutputPath, const FString &TargetLongPackageName, const TArray<FString> &DependicesInGameContentDir, const TArray<FString> &OtherDependices);

private:
    TSharedPtr<class FUICommandList> PluginCommands;
};
// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

#include "ExportAssetDependeciesSettings.h"
#include "ExportAssetDependecies.h"
#include "UObject/ConstructorHelpers.h"

#define LOCTEXT_NAMESPACE "ExportAssetDependecies"

UExportAssetDependeciesSettings::UExportAssetDependeciesSettings(const FObjectInitializer& Initializer)
    : Super(Initializer)
{
    bShouldExportRecursively = false;


}

#undef LOCTEXT_NAMESPACE

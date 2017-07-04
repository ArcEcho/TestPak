// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

#include "ExportAssetDependeciesCommands.h"

#define LOCTEXT_NAMESPACE "FExportAssetDependeciesModule"

void FExportAssetDependeciesCommands::RegisterCommands()
{
	UI_COMMAND(PluginAction, "ExportAssetDependecies", "Execute ExportAssetDependecies action", EUserInterfaceActionType::Button, FInputGesture());
}

#undef LOCTEXT_NAMESPACE

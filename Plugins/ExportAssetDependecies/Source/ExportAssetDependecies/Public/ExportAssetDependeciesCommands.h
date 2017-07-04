// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Framework/Commands/Commands.h"
#include "ExportAssetDependeciesStyle.h"

class FExportAssetDependeciesCommands : public TCommands<FExportAssetDependeciesCommands>
{
public:

	FExportAssetDependeciesCommands()
		: TCommands<FExportAssetDependeciesCommands>(TEXT("ExportAssetDependecies"), NSLOCTEXT("Contexts", "ExportAssetDependecies", "ExportAssetDependecies Plugin"), NAME_None, FExportAssetDependeciesStyle::GetStyleSetName())
	{
	}

	// TCommands<> interface
	virtual void RegisterCommands() override;

public:
	TSharedPtr< FUICommandInfo > PluginAction;
};

#pragma once

#include "CoreMinimal.h"
#include "Engine/EngineTypes.h"
#include "ExportAssetDependeciesSettings.generated.h"

/**
* Settings for export asset dependencies.
*/
UCLASS(config = Game, defaultconfig)
class EXPORTASSETDEPENDECIES_API UExportAssetDependeciesSettings : public UObject
{
    GENERATED_UCLASS_BODY()

public:
    /** Choose directories in which you want export the asset's dependencies.*/
    UPROPERTY(EditAnywhere, config, Category = Default, meta = (DisplayName = "List of directories to search the exported assets.", ContentDir, LongPackageName, NoElementDuplicate))
        TArray<FDirectoryPath> ExportDirectories;
   
    /** Determine if scan assets recursively.*/
    UPROPERTY(config, EditAnywhere, BlueprintReadWrite, Category = Default)
        bool bShouldExportRecursively;
};

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
    UPROPERTY(config, EditAnywhere, BlueprintReadWrite, Category = Default)
        FDirectoryPath OutputPath;

    UPROPERTY(config, EditAnywhere, BlueprintReadWrite, Category = Default)
        FFilePath TargetLongPackageName;
};

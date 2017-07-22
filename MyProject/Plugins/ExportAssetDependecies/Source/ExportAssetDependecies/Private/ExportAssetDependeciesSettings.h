#pragma once

#include "ExportAssetDependecies.h"
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

    /** You can use copied asset string reference here, e.g. World'/Game/NewMap.NewMap'*/
    UPROPERTY(config, EditAnywhere, BlueprintReadWrite, Category = Default)
        TArray<FFilePath> PackagesToExport;
};

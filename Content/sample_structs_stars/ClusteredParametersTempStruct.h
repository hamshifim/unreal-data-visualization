#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "ClusteredParametersTempStruct.generated.h"

USTRUCT(BlueprintType)
struct FClusteredParametersTempStruct : public FTableRowBase
{
    GENERATED_BODY()

public:
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int32 IngestionId;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int32 Size;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int32 Cycle;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int32 molecule_count;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int32 cumulative_count;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int32 distance_rank_per_size;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float log_scaled;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    double scaled;

};

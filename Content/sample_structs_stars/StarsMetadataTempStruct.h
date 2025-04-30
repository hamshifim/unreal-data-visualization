#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "StarsMetadataTempStruct.generated.h"

USTRUCT(BlueprintType)
struct FStarsMetadataTempStruct : public FTableRowBase
{
    GENERATED_BODY()

public:
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FString star_name;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FString galaxy_name;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FString size;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FString color;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FString distance_from_earth;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float likelihood_of_life;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FString num_surr_planets;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FString type_of_star;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int32 p_life;

};

#pragma once
#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "MilkyWayRegularStruct.generated.h"

USTRUCT(BlueprintType)
struct FMilkyWayRegularStruct : public FTableRowBase
{
    GENERATED_BODY()

public:
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    double radius;
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FString color;
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FString star_name;
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FString galaxy_name;
};

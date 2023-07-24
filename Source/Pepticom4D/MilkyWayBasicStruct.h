#pragma once
#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "MilkyWayBasicStruct.generated.h"

USTRUCT(BlueprintType)
struct FMilkyWayBasicStruct : public FTableRowBase
{
    GENERATED_BODY()

public:
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    double radius;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FString color;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FString star_name;
};

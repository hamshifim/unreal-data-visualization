#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "SpatialDataStruct.generated.h"


/* This struct represents spatial data points */
USTRUCT(BlueprintType)
struct FSpatialDataStruct : public FTableRowBase
{
	GENERATED_BODY()

public:
	/** Please add a variable description */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (DisplayName = "x", MakeStructureDefaultValue = "0.000000"))
	double X;

	/** Please add a variable description */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (DisplayName = "y", MakeStructureDefaultValue = "0.000000"))
	double Y;

	/** Please add a variable description */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (DisplayName = "z", MakeStructureDefaultValue = "0.000000"))
	double Z;
};

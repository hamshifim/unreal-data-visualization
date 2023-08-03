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
	double x;

	/** Please add a variable description */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (DisplayName = "y", MakeStructureDefaultValue = "0.000000"))
	double y;

	/** Please add a variable description */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (DisplayName = "z", MakeStructureDefaultValue = "0.000000"))
	double z;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (DisplayName = "type", MakeStructureDefaultValue = "default"))
	FString type;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (DisplayName = "size", MakeStructureDefaultValue = "default"))
	double size;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (DisplayName = "color", MakeStructureDefaultValue = "default"))
	FString color;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (DisplayName = "opacity", MakeStructureDefaultValue = "0.000000"))
	double opacity;

};

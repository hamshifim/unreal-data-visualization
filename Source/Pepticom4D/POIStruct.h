// Fill out your copyright notice in the Description page of Project Settings.
#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "POIStruct.generated.h"

/* This struct represents points of interest, which can be used to, for example, generate view positions */
USTRUCT(BlueprintType)
struct FPOIStruct : public FTableRowBase
{
	GENERATED_BODY()

public:
	/** Please add a variable description */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (DisplayName = "x_min", MakeStructureDefaultValue = 0.000000))
	double X_MIN;

	/** Please add a variable description */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (DisplayName = "x_max", MakeStructureDefaultValue = 0.000000))
	double X_MAX;

	/** Please add a variable description */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (DisplayName = "y_min", MakeStructureDefaultValue = 0.000000))
	double Y_MIN;

	/** Please add a variable description */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (DisplayName = "y_max", MakeStructureDefaultValue = 0.000000))
	double Y_MAX;

	/** Please add a variable description */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (DisplayName = "z_min", MakeStructureDefaultValue = 0.000000))
	double Z_MIN;

	/** Please add a variable description */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (DisplayName = "z_max", MakeStructureDefaultValue = 0.000000))
	double Z_MAX;
};

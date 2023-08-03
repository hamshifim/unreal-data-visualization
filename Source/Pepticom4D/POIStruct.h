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
	// The below properties describe the bounding points along the X, Y, and Z axes
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (DisplayName = "min_x", MakeStructureDefaultValue = 0.000000))
	double min_x;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (DisplayName = "max_x", MakeStructureDefaultValue = 0.000000))
	double max_x;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (DisplayName = "min_y", MakeStructureDefaultValue = 0.000000))
	double min_y;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (DisplayName = "max_y", MakeStructureDefaultValue = 0.000000))
	double max_y;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (DisplayName = "min_z", MakeStructureDefaultValue = 0.000000))
	double min_z;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (DisplayName = "max_z", MakeStructureDefaultValue = 0.000000))
	double max_z;

	// The below properties describe the center and radius of a sphere that bounds all points
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (DisplayName = "center_x", MakeStructureDefaultValue = 0.000000))
	double center_x;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (DisplayName = "center_y", MakeStructureDefaultValue = 0.000000))
	double center_y;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (DisplayName = "center_z", MakeStructureDefaultValue = 0.000000))
	double center_z;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (DisplayName = "radius", MakeStructureDefaultValue = 0.000000))
	double radius;
};

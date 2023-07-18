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
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (DisplayName = "x_min", MakeStructureDefaultValue = 0.000000))
	double X_MIN;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (DisplayName = "x_max", MakeStructureDefaultValue = 0.000000))
	double X_MAX;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (DisplayName = "y_min", MakeStructureDefaultValue = 0.000000))
	double Y_MIN;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (DisplayName = "y_max", MakeStructureDefaultValue = 0.000000))
	double Y_MAX;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (DisplayName = "z_min", MakeStructureDefaultValue = 0.000000))
	double Z_MIN;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (DisplayName = "z_max", MakeStructureDefaultValue = 0.000000))
	double Z_MAX;

	// The below properties describe the center and radius of a sphere that bounds all points
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (DisplayName = "center_x", MakeStructureDefaultValue = 0.000000))
	double CENTER_X;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (DisplayName = "center_y", MakeStructureDefaultValue = 0.000000))
	double CENTER_Y;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (DisplayName = "center_z", MakeStructureDefaultValue = 0.000000))
	double CENTER_Z;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (DisplayName = "radius", MakeStructureDefaultValue = 0.000000))
	double RADIUS;
};

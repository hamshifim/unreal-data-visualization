// Fill out your copyright notice in the Description page of Project Settings.
#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "SpatialMetadataStruct.generated.h"

/* This struct represents points of interest, which can be used to, for example, generate view positions */
USTRUCT(BlueprintType)
struct FSpatialMetadataStruct : public FTableRowBase
{
	GENERATED_BODY()

public:
	// The below properties describe the bounding points along the X, Y, and Z axes
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (DisplayName = "name", MakeStructureDefaultValue = "Default Star Name"))
	FString NAME;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (DisplayName = "galaxy", MakeStructureDefaultValue = "Default Galaxy Name"))
	FString GALAXY;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (DisplayName = "radius", MakeStructureDefaultValue = 0.00000000))
	double RADIUS;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (DisplayName = "color", MakeStructureDefaultValue = "#FFFFFF"))
	FString COLOR;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (DisplayName = "distance_from_earth", MakeStructureDefaultValue = 0.00000000))
	double DISTANCE_FROM_EARTH;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (DisplayName = "likelihood_of_life", MakeStructureDefaultValue = 0.00000000))
	double LIKELIHOOD_OF_LIFE;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (DisplayName = "num_surr_planets", MakeStructureDefaultValue = 0))
	int NUM_SURR_PLANETS;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (DisplayName = "type_of_star", MakeStructureDefaultValue = "Default Star Type"))
	FString TYPE_OF_STAR;
};

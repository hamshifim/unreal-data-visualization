// Fill out your copyright notice in the Description page of Project Settings.
#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "ViewPositionStruct.generated.h"

/* This struct represents view positions which can be used to examine spatial data points from different perspectives */
USTRUCT(BlueprintType)
struct FViewPositionStruct : public FTableRowBase
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (DisplayName = "x", MakeStructureDefaultValue = 0.000000))
	double X;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (DisplayName = "y", MakeStructureDefaultValue = 0.000000))
	double Y;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (DisplayName = "z", MakeStructureDefaultValue = 0.000000))
	double Z;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (DisplayName = "view_number", MakeStructureDefaultValue = 1))
	int VIEW_NUMBER;

	// Stores the same information as the FName of each row; only populated with data during runtime and not in the DataTable
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (DisplayName = "view_number", MakeStructureDefaultValue = 1))
	FString VIEW_NAME;
};


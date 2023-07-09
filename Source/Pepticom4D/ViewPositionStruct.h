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
	/** Please add a variable description */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (DisplayName = "x", MakeStructureDefaultValue = 0.000000))
	double X;

	/** Please add a variable description */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (DisplayName = "y", MakeStructureDefaultValue = 0.000000))
	double Y;

	/** Please add a variable description */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (DisplayName = "z", MakeStructureDefaultValue = 0.000000))
	double Z;

	/** Please add a variable description */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (DisplayName = "view_number", MakeStructureDefaultValue = 1))
	int VIEW_NUMBER;

	/** Please add a variable description */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (DisplayName = "view_name", MakeStructureDefaultValue = "Home"))
	FString VIEW_NAME;
};


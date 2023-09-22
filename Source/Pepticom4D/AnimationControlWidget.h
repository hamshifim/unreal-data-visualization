// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "AnimationControlWidget.generated.h"

/**
 * 
 */
UCLASS()
class PEPTICOM4D_API UAnimationControlWidget : public UUserWidget
{
	GENERATED_BODY()



public:
	UAnimationControlWidget(const FObjectInitializer& ObjectInitializer);

	UFUNCTION()
	void OnAnimationButtonClick();
};

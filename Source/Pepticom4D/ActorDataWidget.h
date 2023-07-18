// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ActorDataWidget.generated.h"

/**
 * 
 */
UCLASS()
class PEPTICOM4D_API UActorDataWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UActorDataWidget(const FObjectInitializer& ObjectInitializer);
	
	UFUNCTION()
	void AnimateIn(float AnimationDuration);
};

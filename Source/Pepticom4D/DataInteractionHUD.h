// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "DataInteractionPlayerController.h"
#include "DataInteractionHUD.generated.h"

/**
 * 
 */
UCLASS()
class PEPTICOM4D_API ADataInteractionHUD : public AHUD
{
	GENERATED_BODY()

public:
	virtual void BeginPlay() override;

	virtual void DrawHUD() override;

	ADataInteractionPlayerController* DataInteractionPlayerController = nullptr;
	AActor* ActorToDrawLineTo = nullptr;
	FVector2D LineStart;

};

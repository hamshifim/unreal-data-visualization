// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "DataInteractionPlayerController.generated.h"

/**
 * 
 */
UCLASS()
class PEPTICOM4D_API ADataInteractionPlayerController : public APlayerController
{
    GENERATED_BODY()
	
public:
    virtual void SetupInputComponent() override;

    UFUNCTION()
    void OnLeftClick();

    UFUNCTION()
    void OnRightClick();

    virtual void Tick(float DeltaSeconds) override;

private:
    AActor* SelectedActor = nullptr;
};

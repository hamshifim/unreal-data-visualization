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

    ADataInteractionPlayerController();
	
public:
    virtual void SetupInputComponent() override;

    UFUNCTION()
    void OnLeftClickPressed();

    UFUNCTION()
    void OnRightClickPressed();

    UFUNCTION()
    void OnRightClickReleased();

    UFUNCTION()
    void EmptyAxisInput(float Value);

    UFUNCTION()
    void TogglePanning();

    virtual void Tick(float DeltaSeconds) override;

protected:
    virtual void BeginPlay() override;

private:
    AActor* SelectedActor = nullptr;
    bool bIsRightMouseDown = false;
    bool bIsPanning = false;
    double MouseSensitivity = 1.0;
    FVector2D PreviousMousePosition = FVector2D(0.0, 0.0);
};

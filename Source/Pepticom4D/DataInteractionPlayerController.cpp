// Fill out your copyright notice in the Description page of Project Settings.
/* In Unreal, go to Edit -> Project Settings -> Engine -> Input, and, in Bindings, 
add an Action Mapping called "LeftMouseClick" and bind it to the Left Mouse Button event,
and add an Action Mapping called "RightMouseClick" and bind it to the Right Mouse Button event.
Add an Action Mapping called "TogglePanning" and bind it to the Middle Mouse Button event.
Add an Axis Mapping called "MoveForward" and bind it to W (scale 1) and S (scale -1).
Add an Axis Mapping caled "MoveRight" and bind it to D (scale 1) and A (scale -1).
Add an Axis Mapping called "CameraYaw" and bind it to MouseX.
Add an Axis Mapping called "CameraPitch" and bind it to MouseY.
Add an Axis Mapping called "MoveUp" and bind it to C (scale 1) and Z (scale -1).
Add an Axis Mapping called "ZoomIn" and bind it to Mouse Wheel Axis. */

#include "DataInteractionPlayerController.h"
#include "GameFramework/Actor.h"

ADataInteractionPlayerController::ADataInteractionPlayerController() {}

void ADataInteractionPlayerController::BeginPlay()
{
	Super::BeginPlay();

    // Set this class to tick
    PrimaryActorTick.bCanEverTick = true;
    // Enable mouse input
    bEnableClickEvents = true;
    // Enable mouse cursor
    bShowMouseCursor = true;
    // Set input mode to game and UI
    auto InputMode = FInputModeGameAndUI();
    InputMode.SetHideCursorDuringCapture(false);
    SetInputMode(InputMode);
}

void ADataInteractionPlayerController::SetupInputComponent()
{
    Super::SetupInputComponent();

    // Bind the input
    InputComponent->BindAction("LeftMouseClick", IE_Pressed, this, &ADataInteractionPlayerController::OnLeftClickPressed);
    InputComponent->BindAction("RightMouseClick", IE_Pressed, this, &ADataInteractionPlayerController::OnRightClickPressed);
    InputComponent->BindAction("RightMouseClick", IE_Released, this, &ADataInteractionPlayerController::OnRightClickReleased);
    InputComponent->BindAction("TogglePanning", IE_Pressed, this, &ADataInteractionPlayerController::TogglePanning);
}

void ADataInteractionPlayerController::OnLeftClickPressed()
{
    bShowMouseCursor = true;
    FHitResult HitResult;
    GetHitResultUnderCursor(ECC_Pawn, true, HitResult);
    if (HitResult.bBlockingHit)
    {
        // Save a pointer to the selected actor
        SelectedActor = HitResult.GetActor();
    }
}

void ADataInteractionPlayerController::OnRightClickPressed()
{
    bIsRightMouseDown = true;
    // Disable mouse cursor
    bShowMouseCursor = false;
}

void ADataInteractionPlayerController::OnRightClickReleased()
{
    bIsRightMouseDown = false;
	// Enable mouse cursor
    bShowMouseCursor = true;
}

void ADataInteractionPlayerController::EmptyAxisInput(float Value)
{
	/* Empty function to bind to an axis input */
}

void ADataInteractionPlayerController::TogglePanning()
{
	bIsPanning = !bIsPanning;
    // Does not work yet! Needs to be developed.
}

void ADataInteractionPlayerController::Tick(float DeltaSeconds)
{
    Super::Tick(DeltaSeconds);
}
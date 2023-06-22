// Fill out your copyright notice in the Description page of Project Settings.
#include "DataInteractionPlayerController.h"
#include "GameFramework/Actor.h"

void ADataInteractionPlayerController::SetupInputComponent()
{
    Super::SetupInputComponent();

    // Bind the input
    InputComponent->BindAction("LeftMouseClick", IE_Pressed, this, &ADataInteractionPlayerController::OnLeftClick);
    InputComponent->BindAction("RightMouseClick", IE_Pressed, this, &ADataInteractionPlayerController::OnRightClick);
}

void ADataInteractionPlayerController::OnLeftClick()
{
    FHitResult HitResult;
    GetHitResultUnderCursor(ECC_Pawn, true, HitResult);
    if (HitResult.bBlockingHit)
    {
        // Save a pointer to the selected actor
        SelectedActor = HitResult.GetActor();
    }
}

void ADataInteractionPlayerController::OnRightClick()
{
    // Deselect the actor
    SelectedActor = nullptr;
}

void ADataInteractionPlayerController::Tick(float DeltaSeconds)
{
    Super::Tick(DeltaSeconds);

    // If an actor is selected and the mouse is moved, update the actor's position
    if (SelectedActor)
    {
        FHitResult HitResult;
        GetHitResultUnderCursor(ECC_Visibility, true, HitResult);
        if (HitResult.bBlockingHit)
        {
            SelectedActor->SetActorLocation(HitResult.Location);
        }
    }
}
// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerCameraPawn.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "DataInteractionPlayerController.h"
#include "ActorToSpawn.h"
#include "POIStruct.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
APlayerCameraPawn::APlayerCameraPawn()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

    //Create our components
    RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootSceneComponent"));
    StaticMeshComp = CreateDefaultSubobject <UStaticMeshComponent>(TEXT("StaticMeshComponent"));
    SpringArmComp = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArmComponent"));
    CameraComp = CreateDefaultSubobject<UCameraComponent>(TEXT("CameraComponent"));

    //Attach our components
    StaticMeshComp->SetupAttachment(RootComponent);
    SpringArmComp->SetupAttachment(StaticMeshComp);
    CameraComp->SetupAttachment(SpringArmComp, USpringArmComponent::SocketName);

    //Assign SpringArm class variables.
    SpringArmComp->SetRelativeLocationAndRotation(FVector(0.0f, 0.0f, 50.0f), FRotator(-60.0f, 0.0f, 0.0f));
    SpringArmComp->TargetArmLength = 400.f;
    SpringArmComp->bEnableCameraLag = true;
    SpringArmComp->CameraLagSpeed = 0.0f;
    // Allow the camera to clip through objects
    SpringArmComp->bDoCollisionTest = false;

    // Load the points of interest data table and store the center and radius of the view sphere
    StoreViewSphereCenterAndRadius();
    // Using the POI data table, generate several default views
    GenerateViewsFromPOIDataTable();
    // Load the views data table and store the views in a map for quick searching
    LoadViewsToMapFromDataTable();
    // Calculate and store the FOV
    FOV = CalculateFOV();

    // IMPORTANT NOTE: Any time that the view data table is updated, the views map must be updated as well

}

void APlayerCameraPawn::StoreViewSphereCenterAndRadius() {
    /* Using data from the appropriate data table, actors are spawned in the world */
    // Get the data table
    UDataTable* DataTable = LoadObject<UDataTable>(NULL, TEXT("/Game/POIDataTable.POIDataTable"), NULL, LOAD_None, NULL);
    // Make sure that we found the data table
    if (DataTable) {
        // Get all of the row names
        TArray<FName> RowNames = DataTable->GetRowNames();
        for (auto& Name : RowNames) {
            FPOIStruct* Row = DataTable->FindRow<FPOIStruct>(Name, TEXT(""));
            // Make sure that we found the row.
            if (Row) {
                // For now, we are assuming that there is only one row
                Center = FVector(Row->CENTER_X, Row->CENTER_Y, Row->CENTER_Z);
                Radius = Row->RADIUS;
            }
            else {
                UE_LOG(LogTemp, Warning, TEXT("Could not find row in data table for storing view sphere data"));
            }
        }
    }
    else {
        UE_LOG(LogTemp, Warning, TEXT("Could not find data table for storing view sphere data"));
    }
}

double APlayerCameraPawn::CalculateFOV() 
{
    double Tangent = (Radius * 2) / (Radius * 2 * ViewPaddingFactor);
    double HalfFOVRadians = atan(Tangent);
    double FovRadians = 2 * HalfFOVRadians;
    double FovDegrees = FMath::RadiansToDegrees(FovRadians);
    return FovDegrees * FOVPaddingFactor;
}

void APlayerCameraPawn::RestoreDefaults(bool AbandonTarget = true) {
    bIsResettingCamera = false;
    if (AbandonTarget) {
        // Abandon the target actor
        TargetActor = nullptr;
        // Hide the actor data widget
        UIManager->HideActorDataWidget();
        // Stop drawing a connecting line from the actor to the actor data widget
        UIManager->StopDrawingConnectingLineToActorDataWidget();
        // Reset the view name
        UpdateCurrentViewName("Free View");
    }
}

void APlayerCameraPawn::GenerateViewsFromPOIDataTable() 
{
    // Calculate camera offsets for each view
    FVector FrontViewOffset = FVector(-Radius * ViewPaddingFactor * 2, 0, 0);
    SaveView(FrontViewOffset, 0, "Front View");
    FVector SideViewOffset = FVector(0, -Radius * ViewPaddingFactor * 2, 0);
    SaveView(SideViewOffset, 1, "Side View");
    FVector TopViewOffset = FVector(0, 0, Radius * ViewPaddingFactor * 2);
    SaveView(TopViewOffset, 2, "Top View");

}

void APlayerCameraPawn::LoadViewsToMapFromDataTable() {
    // Load the views data table
    UDataTable* DataTable = LoadObject<UDataTable>(NULL, TEXT("/Game/ViewPositionDataTable.ViewPositionDataTable"), NULL, LOAD_None, NULL);
    // Make sure that we found the data table
    if (!DataTable) {
        UE_LOG(LogTemp, Error, TEXT("Could not find data table for obtaining view positions"));
        return;
    }

    // Clear the map
    ViewNumberToRowMap.Empty();

    // Get all of the row names
    TArray<FName> RowNames = DataTable->GetRowNames();
    // Iterate through all of the rows
    for (auto& Name : RowNames) {
		FViewPositionStruct* Row = DataTable->FindRow<FViewPositionStruct>(Name, TEXT(""));
		// Make sure that we found the row.
        if (Row) {
            // Store the name in the row so that we can access it later. This is not stored in the data table.
            Row->VIEW_NAME = Name.ToString();
            // Save the view in our map
            ViewNumberToRowMap.Add(Row->VIEW_NUMBER, Row);
		}
        else {
			UE_LOG(LogTemp, Warning, TEXT("Could not find row in data table for obtaining view positions"));
		}
	}
}

void APlayerCameraPawn::ResetCameraZoomAndRotation(double RadiusOfTarget, FVector CenterOfTarget, bool Animate = false) {
    // Reset the Camera's relative location to the spring arm
    TargetResetCameraLocation = FVector(-RadiusOfTarget * ViewPaddingFactor, 0, 0);
    // Reset the Spring Arm's rotation
    TargetResetSpringArmRotation = (CenterOfTarget - GetActorLocation()).Rotation();
    if (Animate) {
        // Trigger the camera to be reset in the Tick function
        bIsResettingCamera = true;
    }
    else {
        // Perform the reset immediately
        CameraComp->SetRelativeLocation(TargetResetCameraLocation);
        SpringArmComp->SetWorldRotation((CenterOfTarget - GetActorLocation()).Rotation());
    }
    // Reset the zoom factor
    ZoomFactor = 1.0f;
}

void APlayerCameraPawn::UpdateCurrentViewName(FString NewViewName) {
	CurrentViewName = NewViewName;
    if (UIManager) {
        UIManager->UpdateViewNameWidget(NewViewName);
    }
}

void APlayerCameraPawn::SwitchToNextView() {
    // Cancel other actions that may be trying to take control of the camera
    RestoreDefaults();

    int TotalNumberOfViews = ViewNumberToRowMap.Num();
    // Determine the next view number
    int NextViewNumber = (CurrentViewNumber + 1) % TotalNumberOfViews;

    // Search for the row with the next view number, stored in Row->VIEW_NUMBER
    FViewPositionStruct** RowPtr = ViewNumberToRowMap.Find(NextViewNumber);
    FViewPositionStruct* Row;
    if (RowPtr) 
    {
        // The view exists.
        Row = *RowPtr;
        FVector ViewOffset = FVector(Row->X, Row->Y, Row->Z);
        // Set the FOV
        CameraComp->SetFieldOfView(FOV);
        // Update the actor's location
        SetActorLocation(Center + ViewOffset);
        // Reset the camera's zoom and rotation
        ResetCameraZoomAndRotation(Radius, Center);
    }
    else
    {
		UE_LOG(LogTemp, Error, TEXT("Could not find row in views map for obtaining view positions"));
		return;
	}

    // Update the current view number
    CurrentViewNumber = NextViewNumber;
    // Update the current view name
    UpdateCurrentViewName(Row->VIEW_NAME);
}

void APlayerCameraPawn::SaveView(FVector& ViewCenterOffset, int ViewNumber, FString ViewName) {
    // Get the data table
    UDataTable* DataTable = LoadObject<UDataTable>(NULL, TEXT("/Game/ViewPositionDataTable.ViewPositionDataTable"), NULL, LOAD_None, NULL);
    // Make sure that we found the data table
    if (DataTable) {
        // Check if we have a view with the same name
        FViewPositionStruct* Row = DataTable->FindRow<FViewPositionStruct>(FName(*ViewName), TEXT(""));
        // Make sure that we found the row
        if (Row) {
            // The row exists. We need to update it.
            // Update the row
            Row->X = ViewCenterOffset.X;
            Row->Y = ViewCenterOffset.Y;
            Row->Z = ViewCenterOffset.Z;
            Row->VIEW_NUMBER = ViewNumber;
        }
        else {
            // The row does not exist. We need to add it.
            // Create a new row
            FViewPositionStruct NewRow;
            NewRow.X = ViewCenterOffset.X;
            NewRow.Y = ViewCenterOffset.Y;
            NewRow.Z = ViewCenterOffset.Z;
            NewRow.VIEW_NUMBER = ViewNumber;
            // Add the row to the data table
            DataTable->AddRow(FName(*ViewName), NewRow);
        }
    }
    else {
        UE_LOG(LogTemp, Warning, TEXT("Could not find data table for saving views"));
    }
}

void APlayerCameraPawn::SaveCurrentView() {
    // Placeholder - does nothing yet. When we save a view, we will need to update the view map.
}

FString APlayerCameraPawn::GetCurrentViewName() {
    return CurrentViewName;
}

void APlayerCameraPawn::MoveCameraToActor(AActor* Target) {
    TargetActor = Target;
    // Get the bounds of the target
    FVector Origin, BoxExtent;
    Target->GetActorBounds(false, Origin, BoxExtent);
    float RadiusOfTarget = BoxExtent.GetMax();
    // Reset the camera's zoom and rotation
    ResetCameraZoomAndRotation(RadiusOfTarget, Origin, true);
    // Zoom in on the target
    ZoomFactor = 100.0f;
    // The main functionality occurs in Tick; this function just sets the target actor
}

// Called when the game starts or when spawned
void APlayerCameraPawn::BeginPlay()
{
	Super::BeginPlay();

    // Store the UI Manager actor in the world
    UIManager = Cast<AUIManager>(UGameplayStatics::GetActorOfClass(GetWorld(), AUIManager::StaticClass()));

    // Update the view name to the default (this happens here because we need the UI Manager to be initialized)
    UpdateCurrentViewName("Free View");
}

// Called every frame
void APlayerCameraPawn::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    {
        // Control zoom by moving the camera along the forward vector (the direction the camera is facing)
        FVector CurrentCameraLocation = CameraComp->GetComponentLocation();
        // Calculate the new camera location
        // Multiplying by DeltaTime makes the zoom frame rate independent
        FVector TargetCameraLocation = CurrentCameraLocation + (CameraComp->GetForwardVector() * ZoomFactor * ZoomSpeed);
        FVector NewCameraLocation = FMath::VInterpTo(CurrentCameraLocation, TargetCameraLocation, DeltaTime, ZoomSpeed);
        if (NewCameraLocation != CurrentCameraLocation) {
            // Reset the view name
            RestoreDefaults();
        }
        // Set the new camera location
        CameraComp->SetWorldLocation(NewCameraLocation);
    }

    {
        // MoveCameraToActor() sets the target actor to the actor that we want to move the camera to
        // If we have a target actor, move the camera to it
        if (TargetActor != nullptr) {
            // Smoothly move the pawn towards the actor's location
            FVector TargetLocation = TargetActor->GetActorLocation();
            FVector NewLocation = FMath::VInterpTo(GetActorLocation(), TargetLocation, DeltaTime, MovementToTargetSpeed);
            SetActorLocation(NewLocation);

            // Check if we have reached within a certain distance of the target location
            if (FVector::Dist(GetActorLocation(), TargetLocation) < 100.0f) {
                // Populate data in the actor data widget
                UIManager->RefreshActorDataWidget(TargetActor);
                // Make the target actor's widget visible and position it   
                UIManager->DisplayActorDataWidget(TargetActor);
                // Draw a line from the ActorDataWidget to the target actor
                UIManager->StartDrawingConnectingLineToActorDataWidget(TargetActor);
                // Reset the target actor once we have reached it
                TargetActor = nullptr;
                // Update the view name
                UpdateCurrentViewName("Target View");
            }
        }
    }

    {
        // ResetCameraZoomAndRotation() sets the target camera location and rotation
        if (bIsResettingCamera) {
            // Interpolate camera's relative location
            FVector CurrentCameraLocation = CameraComp->GetRelativeLocation();
            FVector NewCameraLocation = FMath::VInterpTo(CurrentCameraLocation, TargetResetCameraLocation, DeltaTime, MovementToTargetSpeed);
            CameraComp->SetRelativeLocation(NewCameraLocation);

            // Interpolate spring arm's world rotation
            FRotator CurrentSpringArmRotation = SpringArmComp->GetComponentRotation();
            FRotator NewSpringArmRotation = FMath::RInterpTo(CurrentSpringArmRotation, TargetResetSpringArmRotation, DeltaTime, MovementToTargetSpeed);
            SpringArmComp->SetWorldRotation(NewSpringArmRotation);

            // Check if the movement and rotation are nearly done, then stop interpolating
            if (CurrentCameraLocation.Equals(TargetResetCameraLocation, KINDA_SMALL_NUMBER) && CurrentSpringArmRotation.Equals(TargetResetSpringArmRotation, KINDA_SMALL_NUMBER)) {
                UE_LOG(LogTemp, Warning, TEXT("Finished resetting camera"));
                bIsResettingCamera = false;
                TargetResetCameraLocation = FVector::ZeroVector;
                TargetResetSpringArmRotation = FRotator::ZeroRotator;
            }
        }
    }

    {
        ADataInteractionPlayerController* PlayerController = Cast<ADataInteractionPlayerController>(Controller);
        // Make sure that the controller is of the correct type
        if (PlayerController)
        {
            if (PlayerController->bIsRightMouseDown)
            {
                // Rotate our actor's yaw, which will turn our camera because we're attached to it
                {
                    FRotator NewRotation = GetActorRotation();
                    NewRotation.Yaw += CameraInput.X;
                    SetActorRotation(NewRotation);
                }

                // Rotate our camera's pitch, which will turn our camera because it's attached to the spring arm
                {
                    FRotator NewRotation = SpringArmComp->GetComponentRotation();
                    NewRotation.Pitch += CameraInput.Y;
                    SpringArmComp->SetWorldRotation(NewRotation);
                }

                // Reset the view name if we are changing camera angles
                if (CameraInput.Y != 0 || CameraInput.X != 0) {
                    RestoreDefaults(false);
                }
            }

            // Make sure that panning is enabled on the controller
            if (PlayerController->bIsPanning) {
                // Handle movement based on our X, Y, and Z inputs
                if (!MovementInput.IsZero())
                {
                    //Scale our movement input axis values by MovementSpeed units per second
                    MovementInput = MovementInput.GetSafeNormal() * MovementSpeed;
                    FVector NewLocation = GetActorLocation();
                    NewLocation += CameraComp->GetForwardVector() * MovementInput.X * DeltaTime;
                    NewLocation += CameraComp->GetRightVector() * MovementInput.Y * DeltaTime;
                    NewLocation += CameraComp->GetUpVector() * MovementInput.Z * DeltaTime;
                    SetActorLocation(NewLocation);
                    // Reset the view name if we are changing positions
                    RestoreDefaults();
                }
            }
        }
    }
}

// Called to bind functionality to input
void APlayerCameraPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

    // Zooming in and out
    InputComponent->BindAxis("ZoomIn", this, &APlayerCameraPawn::ZoomIn);

    // Movement across X, Y, Z axes
    InputComponent->BindAxis("MoveForward", this, &APlayerCameraPawn::MoveForward);
    InputComponent->BindAxis("MoveRight", this, &APlayerCameraPawn::MoveRight);
    InputComponent->BindAxis("MoveUp", this, &APlayerCameraPawn::MoveUp);

    // Camera rotation across X and Y axes
    InputComponent->BindAxis("CameraPitch", this, &APlayerCameraPawn::PitchCamera);
    InputComponent->BindAxis("CameraYaw", this, &APlayerCameraPawn::YawCamera);

    // Trigger views mode
    InputComponent->BindAction("TriggerViewsMode", IE_Pressed, this, &APlayerCameraPawn::SwitchToNextView);

}

void APlayerCameraPawn::MoveForward(float AxisValue)
{
    MovementInput.X = FMath::Clamp<float>(AxisValue, -1.0f, 1.0f);
}

void APlayerCameraPawn::MoveRight(float AxisValue)
{
    MovementInput.Y = FMath::Clamp<float>(AxisValue, -1.0f, 1.0f);
}

void APlayerCameraPawn::MoveUp(float AxisValue)
{
	MovementInput.Z = FMath::Clamp<float>(AxisValue, -1.0f, 1.0f);
}

void APlayerCameraPawn::PitchCamera(float AxisValue)
{
    CameraInput.Y = AxisValue;
}

void APlayerCameraPawn::YawCamera(float AxisValue)
{
    CameraInput.X = AxisValue;
}

void APlayerCameraPawn::ZoomIn(float AxisValue)
{
    ZoomFactor = AxisValue;
}


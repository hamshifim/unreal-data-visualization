// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerCameraPawn.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "DataInteractionPlayerController.h"

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

}

// Called when the game starts or when spawned
void APlayerCameraPawn::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void APlayerCameraPawn::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    // Control zoom by moving the camera along the forward vector (the direction the camera is facing)
    FVector CurrentCameraLocation = CameraComp->GetComponentLocation();
    // Calculate the new camera location
    // Multiplying by DeltaTime makes the zoom frame rate independent
    FVector TargetCameraLocation = CurrentCameraLocation + (CameraComp->GetForwardVector() * ZoomFactor * ZoomSpeed);
    FVector NewCameraLocation = FMath::VInterpTo(CurrentCameraLocation, TargetCameraLocation, DeltaTime, ZoomSpeed);
    // Set the new camera location
    CameraComp->SetWorldLocation(NewCameraLocation);

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
		}

        // Make sure that panning is enabled on the controller
        if (PlayerController->bIsPanning) {
            // Handle movement based on our X, Y, and Z inputs
            if (!MovementInput.IsZero())
            {
                //Scale our movement input axis values by MovementSpeed units per second
                MovementInput = MovementInput.GetSafeNormal() * MovementSpeed;
                FVector NewLocation = GetActorLocation();
                NewLocation += GetActorForwardVector() * MovementInput.X * DeltaTime;
                NewLocation += GetActorRightVector() * MovementInput.Y * DeltaTime;
                NewLocation += GetActorUpVector() * MovementInput.Z * DeltaTime;
                SetActorLocation(NewLocation);
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


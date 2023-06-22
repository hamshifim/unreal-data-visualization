// Fill out your copyright notice in the Description page of Project Settings.


#include "ActorToSpawn.h"
#include "Components/SphereComponent.h"
#include "Particles/ParticleSystemComponent.h"
#include "Components/PrimitiveComponent.h"

void AActorToSpawn::Create() {
    //Creating our Default Components
    SphereComp = CreateDefaultSubobject<USphereComponent>(TEXT("Sphere"));
    StaticMeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComp"));
    ParticleComp = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("ParticleComp"));

    //Attaching the Components
    SphereComp->SetupAttachment(RootComponent);
    StaticMeshComp->AttachToComponent(SphereComp, FAttachmentTransformRules::KeepRelativeTransform);
    ParticleComp->AttachToComponent(StaticMeshComp, FAttachmentTransformRules::KeepRelativeTransform);

    //Setting the Sphere radius to be of a smaller size in line with the Static Mesh.
    SphereComp->SetSphereRadius(16.0f);

    //Setting the Static Mesh Scale and Location to fit the radius of the Sphere.
    StaticMeshComp->SetRelativeLocation(FVector(0.0, 0.0, -12.0f));
    StaticMeshComp->SetRelativeScale3D(FVector(0.25, 0.25, 0.25));

    // Disable physics simulation for the actor
    SetActorEnableCollision(false);

    // Force the actor to stay fixed
    StaticMeshComp->Mobility = EComponentMobility::Movable;

    //Using Constructor Helpers to set our Static Mesh Comp with a Sphere Shape.
    static ConstructorHelpers::FObjectFinder<UStaticMesh>SphereMeshAsset(TEXT("StaticMesh'/Game/StarterContent/Shapes/Shape_Sphere.Shape_Sphere'"));
    StaticMeshComp->SetStaticMesh(SphereMeshAsset.Object);

    // Use a material to give the actor an inner glow
    static ConstructorHelpers::FObjectFinder<UMaterial> BaseMaterialAsset(TEXT("Material'/Game/Materials/BaseEmissiveMaterial.BaseEmissiveMaterial'"));
    if (BaseMaterialAsset.Succeeded()) {
        // Create a dynamic material instance for the mesh
        UMaterialInstanceDynamic* DynamicMaterial = UMaterialInstanceDynamic::Create(BaseMaterialAsset.Object, StaticMeshComp);
        if (DynamicMaterial) {
			// Set the emissive color of the material to be red
			DynamicMaterial->SetVectorParameterValue(FName("EmissiveColor"), FLinearColor::White);
            // Set the material on the mesh
			StaticMeshComp->SetMaterial(0, DynamicMaterial);
		}
	}

    /*
    //Using Constructor Helpers to set our Particle Comp with our Fire Particle Comp.
    static ConstructorHelpers::FObjectFinder<UParticleSystem>ParticleCompAsset(TEXT("ParticleSystem'/Game/StarterContent/Particles/P_Fire.P_Fire'"));
    ParticleComp->SetTemplate(ParticleCompAsset.Object);
    */
}

// Sets default values
AActorToSpawn::AActorToSpawn()
{
    // Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
    PrimaryActorTick.bCanEverTick = true;
    // Create the actor
    Create();
}

// Called when the game starts or when spawned
void AActorToSpawn::BeginPlay()
{
    Super::BeginPlay();

}

// Called every frame
void AActorToSpawn::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

}


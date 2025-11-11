// Fill out your copyright notice in the Description page of Project Settings.

#include "PickupParent.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "IntoTheFrontroomsCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundBase.h"
#include "Particles/ParticleSystem.h"

// Sets default values
APickupParent::APickupParent()
{
	// Set this actor to call Tick() every frame
	PrimaryActorTick.bCanEverTick = true;

	// Create sphere collision component
	SphereCollision = CreateDefaultSubobject<USphereComponent>(TEXT("SphereCollision"));
	RootComponent = SphereCollision;
	SphereCollision->SetGenerateOverlapEvents(true);
	SphereCollision->SetSphereRadius(100.f);
	SphereCollision->SetCollisionProfileName(TEXT("OverlapAllDynamic"));

	// Create mesh component
	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	Mesh->SetupAttachment(SphereCollision);
	Mesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	Mesh->SetRelativeScale3D(FVector(0.5f)); // Default scale

	// Enable replication for multiplayer
	bReplicates = true;
	SetReplicateMovement(true);
}

// Called when the game starts or when spawned
void APickupParent::BeginPlay()
{
	Super::BeginPlay();
	
	// Bind overlap event
	if (SphereCollision)
	{
		SphereCollision->OnComponentBeginOverlap.AddDynamic(this, &APickupParent::OnBeginOverlap);
	}

	// Warn if mesh is not set
	if (!Mesh || !Mesh->GetStaticMesh())
	{
		UE_LOG(LogTemp, Warning, TEXT("PickupParent '%s': Mesh component is missing or has no StaticMesh set. Set mesh in Blueprint defaults."), *GetName());
	}
}

// Called every frame
void APickupParent::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// Rotate the pickup for visual appeal
	if (Mesh)
	{
		FRotator NewRotation = Mesh->GetRelativeRotation();
		NewRotation.Yaw += DeltaTime * 90.0f; // Rotate 90 degrees per second
		Mesh->SetRelativeRotation(NewRotation);
	}
}

void APickupParent::OnBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	// Check if the overlapping actor is a player character
	AIntoTheFrontroomsCharacter* Character = Cast<AIntoTheFrontroomsCharacter>(OtherActor);
	if (Character)
	{
		Pickup(Character);
	}
}

void APickupParent::Pickup_Implementation(AIntoTheFrontroomsCharacter* OwningCharacter)
{
	if (!OwningCharacter)
	{
		return;
	}

	// Log pickup event
	UE_LOG(LogTemp, Log, TEXT("PickupParent '%s' picked up by '%s'"), *GetName(), *OwningCharacter->GetName());

	// Set the owner
	SetOwner(OwningCharacter);

	// Disable collision to prevent multiple pickups
	if (SphereCollision)
	{
		SphereCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}

	// Hide the mesh
	if (Mesh)
	{
		Mesh->SetVisibility(false);
	}

	// Play pickup sound if set
	if (PickupSound)
	{
		UGameplayStatics::PlaySoundAtLocation(this, PickupSound, GetActorLocation());
	}

	// Spawn pickup particle effect if set
	if (PickupEffect)
	{
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), PickupEffect, GetActorTransform());
	}

	// Destroy the actor after a short delay (allows sound/effects to play)
	SetLifeSpan(0.5f);
}



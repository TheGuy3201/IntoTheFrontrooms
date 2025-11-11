// Fill out your copyright notice in the Description page of Project Settings.

#include "RoamingAICharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystemComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraComponent.h"
#include "NavigationSystem.h"
#include "TimerManager.h"
#include "AIController.h"

ARoamingAICharacter::ARoamingAICharacter()
{
	PrimaryActorTick.bCanEverTick = false; // Controller handles all AI logic

	// Default AI settings
	SightRange = 1500.0f;
	RoamingSpeed = 200.0f;
	ChaseSpeed = 400.0f;
	MaxRoamDistance = 1000.0f;
	RoamWaitTime = 3.0f;
	LosePlayerTime = 5.0f;
	AcceptanceRadius = 100.0f;

	// Default Attack settings
	AttackRange = 150.0f;
	AttackDamage = 35.0f;
	AttackCooldown = 3.0f;
	bRespawnAtSpawnPoint = false; // Respawn at random location by default
	DespawnSmokeEffectNiagara = nullptr; // UE5 Niagara effect (preferred)
	DespawnSmokeEffectCascade = nullptr; // Legacy Cascade effect (fallback)
	DespawnSmokeScale = 2.0f; // 2x size by default for more visible effect
	DespawnSmokeLifetime = 2.0f; // Smoke lasts 2 seconds by default
	DespawnSound = nullptr;
	LastAttackTime = -999.0f; // Can attack immediately

	// Configure character movement
	GetCharacterMovement()->MaxWalkSpeed = RoamingSpeed;
	
	// Auto possess AI
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;
}

void ARoamingAICharacter::BeginPlay()
{
	Super::BeginPlay();
	
	// Store spawn location for roaming reference and respawning
	SpawnLocation = GetActorLocation();
}

bool ARoamingAICharacter::TryAttackPlayer(ACharacter* Player)
{
	if (!Player)
		return false;

	// Check if we can attack (cooldown)
	if (!CanAttack())
		return false;

	// Check distance to player
	float DistanceToPlayer = FVector::Dist(GetActorLocation(), Player->GetActorLocation());
	if (DistanceToPlayer > AttackRange)
		return false;

	// Deal damage to player
	UGameplayStatics::ApplyDamage(
		Player,
		AttackDamage,
		GetController(),
		this,
		UDamageType::StaticClass()
	);

	// Update last attack time
	if (UWorld* World = GetWorld())
	{
		LastAttackTime = World->GetTimeSeconds();
	}

	// Respawn after attacking
	RespawnWithEffects();

	return true;
}

void ARoamingAICharacter::RespawnWithEffects()
{
	UWorld* World = GetWorld();
	if (!World)
		return;

	// Store current location before teleporting
	FVector DespawnLocation = GetActorLocation();
	FRotator DespawnRotation = GetActorRotation();

	// Spawn smoke effect at current location
	// Prefer Niagara (UE5) over Cascade (legacy)
	if (DespawnSmokeEffectNiagara)
	{
		// Use modern Niagara system
		UNiagaraComponent* SmokeComponent = UNiagaraFunctionLibrary::SpawnSystemAtLocation(
			World,
			DespawnSmokeEffectNiagara,
			DespawnLocation,
			DespawnRotation,
			FVector(DespawnSmokeScale), // Scale
			true, // Auto destroy
			true, // Auto activate
			ENCPoolMethod::None,
			true // Pre-cull check
		);

		// Force destroy after specified lifetime if still exists (safety net)
		if (SmokeComponent)
		{
			// Set up timer to forcefully destroy if needed
			FTimerHandle DestroyTimerHandle;
			World->GetTimerManager().SetTimer(
				DestroyTimerHandle,
				[SmokeComponent]()
				{
					if (SmokeComponent && IsValid(SmokeComponent))
					{
						SmokeComponent->DestroyComponent();
					}
				},
				DespawnSmokeLifetime, // Use configurable lifetime
				false // Don't loop
			);
		}
	}
	else if (DespawnSmokeEffectCascade)
	{
		// Fallback to legacy Cascade system
		UParticleSystemComponent* SmokeComponent = UGameplayStatics::SpawnEmitterAtLocation(
			World,
			DespawnSmokeEffectCascade,
			DespawnLocation,
			DespawnRotation,
			FVector(DespawnSmokeScale), // Use configurable scale
			true, // Auto destroy - this handles auto-destruction
			EPSCPoolMethod::None,
			true // Auto activate
		);

		// Force destroy after specified lifetime if still exists (safety net)
		if (SmokeComponent)
		{
			// Set up timer to forcefully destroy if needed
			FTimerHandle DestroyTimerHandle;
			World->GetTimerManager().SetTimer(
				DestroyTimerHandle,
				[SmokeComponent]()
				{
					if (SmokeComponent && IsValid(SmokeComponent))
					{
						SmokeComponent->DestroyComponent();
					}
				},
				DespawnSmokeLifetime, // Use configurable lifetime
				false // Don't loop
			);
		}
	}

	// Play despawn sound
	if (DespawnSound)
	{
		UGameplayStatics::PlaySoundAtLocation(
			World,
			DespawnSound,
			DespawnLocation,
			1.0f, // Volume
			1.0f, // Pitch
			0.0f, // Start time
			nullptr, // Attenuation
			nullptr, // Concurrency
			nullptr  // Owner
		);
	}

	// Determine respawn location
	FVector RespawnLocation;
	bool bFoundValidLocation = false;
	
	if (bRespawnAtSpawnPoint)
	{
		// Respawn at original spawn point
		RespawnLocation = SpawnLocation;
		bFoundValidLocation = true;
	}
	else
	{
		// Try to find a valid random roaming location
		UNavigationSystemV1* NavSystem = UNavigationSystemV1::GetCurrent(World);
		if (NavSystem)
		{
			FNavLocation ResultLocation;
			
			// Try multiple times to find a valid location
			const int32 MaxAttempts = 5;
			for (int32 Attempt = 0; Attempt < MaxAttempts; ++Attempt)
			{
				bool bSuccess = NavSystem->GetRandomPointInNavigableRadius(
					SpawnLocation,
					MaxRoamDistance,
					ResultLocation
				);

				if (bSuccess)
				{
					// Verify the location is actually on navmesh
					FNavLocation ProjectedLocation;
					if (NavSystem->ProjectPointToNavigation(ResultLocation.Location, ProjectedLocation, FVector(500.0f, 500.0f, 500.0f)))
					{
						RespawnLocation = ProjectedLocation.Location;
						bFoundValidLocation = true;
						break;
					}
				}
			}
			
			// If random location failed, try from current location
			if (!bFoundValidLocation)
			{
				if (NavSystem->GetRandomPointInNavigableRadius(
					GetActorLocation(),
					MaxRoamDistance * 0.5f,
					ResultLocation))
				{
					FNavLocation ProjectedLocation;
					if (NavSystem->ProjectPointToNavigation(ResultLocation.Location, ProjectedLocation, FVector(500.0f, 500.0f, 500.0f)))
					{
						RespawnLocation = ProjectedLocation.Location;
						bFoundValidLocation = true;
					}
				}
			}
		}
		
		// Final fallback to spawn point if all else fails
		if (!bFoundValidLocation)
		{
			RespawnLocation = SpawnLocation;
		}
	}

	// Teleport to respawn location
	SetActorLocation(RespawnLocation, false, nullptr, ETeleportType::TeleportPhysics);

	// Reset AI controller state after respawn
	if (AAIController* AICtrl = Cast<AAIController>(GetController()))
	{
		// Stop any current movement
		AICtrl->StopMovement();
		
		// Give AI a moment to settle at new location before starting new movement
		// This prevents immediate path failures
		FTimerHandle SettleTimerHandle;
		World->GetTimerManager().SetTimer(
			SettleTimerHandle,
			[AICtrl]()
			{
				// AI can now start moving again
				// The controller will pick a new destination on next tick
			},
			0.1f, // Wait 0.1 seconds to settle
			false
		);
	}
}

bool ARoamingAICharacter::CanAttack() const
{
	if (UWorld* World = GetWorld())
	{
		float CurrentTime = World->GetTimeSeconds();
		float TimeSinceLastAttack = CurrentTime - LastAttackTime;
		return TimeSinceLastAttack >= AttackCooldown;
	}
	return false;
}

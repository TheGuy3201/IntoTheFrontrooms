// Fill out your copyright notice in the Description page of Project Settings.

#include "RoamingAIController.h"
#include "RoamingAICharacter.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "NavigationSystem.h"
#include "Navigation/PathFollowingComponent.h"
#include "DrawDebugHelpers.h"
#include "Components/CapsuleComponent.h"

ARoamingAIController::ARoamingAIController()
{
	PrimaryActorTick.bCanEverTick = true;
	
	// Initialize variables
	CurrentState = EAIState::Roaming;
	TimeSinceLastSawPlayer = 0.0f;
	WaitTimer = 0.0f;
	bReachedDestination = true; // Start by needing a new destination
	CurrentRoamDestination = FVector::ZeroVector;
	PlayerCharacter = nullptr;
}

void ARoamingAIController::BeginPlay()
{
	Super::BeginPlay();
	
	// Player will be found dynamically in UpdateAIBehavior (multiplayer support)
	// No need to cache player index 0 anymore
	
	// Start roaming behavior
	CurrentState = EAIState::Roaming;
}

void ARoamingAIController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	UpdateAIBehavior();
}

void ARoamingAIController::OnMoveCompleted(FAIRequestID RequestID, const FPathFollowingResult& Result)
{
	Super::OnMoveCompleted(RequestID, Result);
	
	// Only process if we're in roaming state
	if (CurrentState != EAIState::Roaming)
		return;
	
	ARoamingAICharacter* AIChar = Cast<ARoamingAICharacter>(GetPawn());
	if (!AIChar)
		return;
	
	if (Result.IsSuccess())
	{
		// Successfully reached destination
		bReachedDestination = true;
		CurrentState = EAIState::Waiting;
		WaitTimer = 0.0f;
	}
	else if (Result.IsFailure())
	{
		// Path failed or was blocked - get new destination
		bReachedDestination = true;
		CurrentRoamDestination = FVector::ZeroVector;
	}
}

void ARoamingAIController::UpdateAIBehavior()
{
	// Find closest player (multiplayer support)
	PlayerCharacter = FindClosestPlayer();
	
	if (!PlayerCharacter)
		return; // No player found, skip this frame

	// Verify we have a valid pawn
	if (!GetPawn())
		return;

	switch (CurrentState)
	{
		case EAIState::Roaming:
			RoamBehavior();
			break;
			
		case EAIState::Chasing:
			ChaseBehavior();
			break;
			
		case EAIState::Waiting:
			WaitBehavior(GetWorld()->GetDeltaSeconds());
			break;
	}
}

void ARoamingAIController::RoamBehavior()
{
	ARoamingAICharacter* AIChar = Cast<ARoamingAICharacter>(GetPawn());
	if (!AIChar)
		return;

	// Check if we can see the player
	if (CanSeePlayer())
	{
		// Switch to chase mode
		CurrentState = EAIState::Chasing;
		TimeSinceLastSawPlayer = 0.0f;
		
		// Increase speed for chasing
		AIChar->GetCharacterMovement()->MaxWalkSpeed = AIChar->ChaseSpeed;
		return;
	}

	// Set roaming speed
	AIChar->GetCharacterMovement()->MaxWalkSpeed = AIChar->RoamingSpeed;

	// Check if we need a new destination
	if (bReachedDestination || CurrentRoamDestination == FVector::ZeroVector)
	{
		// Get new random location to roam to
		FVector NewDestination = GetRandomRoamLocation();
		if (NewDestination != FVector::ZeroVector)
		{
			CurrentRoamDestination = NewDestination;
			
			// Use MoveToLocation with proper settings
			EPathFollowingRequestResult::Type Result = MoveToLocation(
				CurrentRoamDestination, 
				AIChar->AcceptanceRadius, 
				true,  // bStopOnOverlap
				true,  // bUsePathfinding
				false, // bProjectDestinationToNavigation
				true,  // bCanStrafe
				nullptr, // FilterClass
				true   // bAllowPartialPath - AI can get as close as possible even if full path fails
			);
			
			// Check if request was NOT successful
			if (Result != EPathFollowingRequestResult::RequestSuccessful && 
			  Result != EPathFollowingRequestResult::AlreadyAtGoal)
			{
				// Path completely failed, try again immediately
				CurrentRoamDestination = FVector::ZeroVector;
				bReachedDestination = true;
			}
			else
			{
				bReachedDestination = false;
			}
		}
		else
		{
			// Navigation system failed to find any valid location, wait and retry
			CurrentState = EAIState::Waiting;
			WaitTimer = 0.0f;
			StopMovement();
		}
	}
	else
	{
		// Currently moving to destination - monitor progress
		float DistanceToDestination = FVector::Dist(AIChar->GetActorLocation(), CurrentRoamDestination);
		FVector Velocity = AIChar->GetVelocity();
		float Speed = Velocity.Size2D();
		
		// Detect if AI is stuck (not moving but far from destination)
		static float StuckTimer = 0.0f;
		if (Speed < 10.0f && DistanceToDestination > AIChar->AcceptanceRadius * 1.5f)
		{
			if (UWorld* World = GetWorld())
			{
				StuckTimer += World->GetDeltaSeconds();
			}
			
			if (StuckTimer > 2.0f) // Stuck for 2 seconds
			{
				bReachedDestination = true;
				CurrentRoamDestination = FVector::ZeroVector;
				StopMovement();
				StuckTimer = 0.0f;
			}
		}
		else
		{
			StuckTimer = 0.0f; // Reset stuck timer if moving
		}
	}
}

void ARoamingAIController::ChaseBehavior()
{
	ARoamingAICharacter* AIChar = Cast<ARoamingAICharacter>(GetPawn());
	if (!AIChar || !PlayerCharacter)
		return;

	// Set chase speed
	AIChar->GetCharacterMovement()->MaxWalkSpeed = AIChar->ChaseSpeed;

	// Check if we're close enough to attack
	float DistanceToPlayer = FVector::Dist(AIChar->GetActorLocation(), PlayerCharacter->GetActorLocation());
	if (DistanceToPlayer <= AIChar->AttackRange && AIChar->CanAttack())
	{
		// Attack the player!
		if (AIChar->TryAttackPlayer(PlayerCharacter))
		{
			// Attack successful - AI has respawned, reset to roaming
			CurrentState = EAIState::Roaming;
			TimeSinceLastSawPlayer = 0.0f;
			bReachedDestination = true;
			CurrentRoamDestination = FVector::ZeroVector;
			return;
		}
	}

	// Check if we can still see the player
	if (CanSeePlayer())
	{
		// Reset timer since we can see player
		TimeSinceLastSawPlayer = 0.0f;
		
		// Move towards player (only update path every few frames for performance)
		static int32 FrameCounter = 0;
		if (FrameCounter % 5 == 0) // Update path every 5 frames
		{
			MoveToActor(PlayerCharacter, AIChar->AcceptanceRadius);
		}
		FrameCounter++;
	}
	else
	{
		// Increment time since last saw player
		if (UWorld* World = GetWorld())
		{
			TimeSinceLastSawPlayer += World->GetDeltaSeconds();
		}
		
		// If we haven't seen player for specified time, go back to roaming
		if (TimeSinceLastSawPlayer >= AIChar->LosePlayerTime)
		{
			CurrentState = EAIState::Roaming;
			TimeSinceLastSawPlayer = 0.0f;
			bReachedDestination = true; // This will trigger getting a new roam location
			CurrentRoamDestination = FVector::ZeroVector; // Force new destination
			StopMovement();
		}
		else
		{
			// Keep moving to last known position
			MoveToLocation(PlayerCharacter->GetActorLocation(), AIChar->AcceptanceRadius);
		}
	}
}

void ARoamingAIController::WaitBehavior(float DeltaTime)
{
	ARoamingAICharacter* AIChar = Cast<ARoamingAICharacter>(GetPawn());
	if (!AIChar)
		return;

	// Check if we can see the player while waiting
	if (CanSeePlayer())
	{
		CurrentState = EAIState::Chasing;
		TimeSinceLastSawPlayer = 0.0f;
		AIChar->GetCharacterMovement()->MaxWalkSpeed = AIChar->ChaseSpeed;
		return;
	}

	// Increment wait timer
	WaitTimer += DeltaTime;

	// Check if wait time is over
	if (WaitTimer >= AIChar->RoamWaitTime)
	{
		CurrentState = EAIState::Roaming;
		WaitTimer = 0.0f;
		bReachedDestination = true; // Signal that we need a new destination
		CurrentRoamDestination = FVector::ZeroVector; // Force new destination selection
	}
}

bool ARoamingAIController::CanSeePlayer()
{
	ARoamingAICharacter* AIChar = Cast<ARoamingAICharacter>(GetPawn());
	if (!AIChar || !PlayerCharacter)
		return false;

	// Check distance to player first (cheap operation)
	float DistanceToPlayer = FVector::Dist(AIChar->GetActorLocation(), PlayerCharacter->GetActorLocation());
	if (DistanceToPlayer > AIChar->SightRange)
		return false;

	// Get proper eye height using capsule component
	float EyeHeight = 0.0f;
	if (UCapsuleComponent* CapsuleComp = AIChar->GetCapsuleComponent())
	{
		EyeHeight = CapsuleComp->GetScaledCapsuleHalfHeight() * 0.9f; // 90% of capsule height
	}
	else
	{
		EyeHeight = 90.0f; // Fallback
	}

	// Perform line trace for line of sight
	FVector StartLocation = AIChar->GetActorLocation() + FVector(0.0f, 0.0f, EyeHeight);
	FVector EndLocation = PlayerCharacter->GetActorLocation() + FVector(0.0f, 0.0f, EyeHeight);

	FHitResult HitResult;
	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(AIChar);
	QueryParams.AddIgnoredActor(PlayerCharacter);
	QueryParams.bTraceComplex = false; // Use simple collision for performance

	// Perform line trace
	if (UWorld* World = GetWorld())
	{
		bool bHit = World->LineTraceSingleByChannel(
			HitResult,
			StartLocation,
			EndLocation,
			ECC_Visibility,
			QueryParams
		);

		// If nothing blocks the trace, we can see the player
		return !bHit;
	}

	return false;
}

FVector ARoamingAIController::GetRandomRoamLocation()
{
	ARoamingAICharacter* AIChar = Cast<ARoamingAICharacter>(GetPawn());
	if (!AIChar)
		return FVector::ZeroVector;

	UWorld* World = GetWorld();
	if (!World)
		return FVector::ZeroVector;

	UNavigationSystemV1* NavSystem = UNavigationSystemV1::GetCurrent(World);
	if (!NavSystem)
		return FVector::ZeroVector;

	// Try to get random point within roaming distance from spawn location
	FNavLocation ResultLocation;
	
	// Try multiple times if first attempt fails
	const int32 MaxAttempts = 3;
	for (int32 Attempt = 0; Attempt < MaxAttempts; ++Attempt)
	{
		bool bSuccess = NavSystem->GetRandomPointInNavigableRadius(
			AIChar->GetSpawnLocation(),
			AIChar->MaxRoamDistance,
			ResultLocation
		);

		if (bSuccess)
		{
			return ResultLocation.Location;
		}
	}

	// If all attempts failed, try getting a point from current location instead
	if (NavSystem->GetRandomPointInNavigableRadius(
		AIChar->GetActorLocation(),
		AIChar->MaxRoamDistance * 0.5f, // Use smaller radius from current position
		ResultLocation))
	{
		return ResultLocation.Location;
	}

	return FVector::ZeroVector;
}

ACharacter* ARoamingAIController::FindClosestPlayer()
{
	ARoamingAICharacter* AIChar = Cast<ARoamingAICharacter>(GetPawn());
	if (!AIChar)
		return nullptr;

	UWorld* World = GetWorld();
	if (!World)
		return nullptr;

	ACharacter* ClosestPlayer = nullptr;
	float ClosestDistance = MAX_FLT;
	FVector AILocation = AIChar->GetActorLocation();

	// Iterate through all player controllers
	for (FConstPlayerControllerIterator It = World->GetPlayerControllerIterator(); It; ++It)
	{
		APlayerController* PC = It->Get();
		if (!PC)
			continue;

		// Get the player's character
		ACharacter* PlayerChar = Cast<ACharacter>(PC->GetPawn());
		if (!PlayerChar || !IsValid(PlayerChar))
			continue;

		// Calculate distance to this player
		float Distance = FVector::Dist(AILocation, PlayerChar->GetActorLocation());

		// Check if this is the closest player so far
		if (Distance < ClosestDistance)
		{
			ClosestDistance = Distance;
			ClosestPlayer = PlayerChar;
		}
	}

	return ClosestPlayer;
}

// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "RoamingAIController.generated.h"

// AI Behavior States
UENUM(BlueprintType)
enum class EAIState : uint8
{
	Roaming		UMETA(DisplayName = "Roaming"),
	Chasing		UMETA(DisplayName = "Chasing"),
	Waiting		UMETA(DisplayName = "Waiting")
};

/**
 * AI Controller that handles roaming and chase behavior
 */
UCLASS()
class INTOTHEFRONTROOMS_API ARoamingAIController : public AAIController
{
	GENERATED_BODY()

public:
	ARoamingAIController();

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	
	// Called when AI movement completes or fails
	virtual void OnMoveCompleted(FAIRequestID RequestID, const FPathFollowingResult& Result) override;

	// AI Behavior Functions
	void UpdateAIBehavior();
	void RoamBehavior();
	void ChaseBehavior();
	void WaitBehavior(float DeltaTime);

	// Line of Sight Check
	bool CanSeePlayer();

	// Get random location for roaming
	FVector GetRandomRoamLocation();

	// Find the closest player character (multiplayer support)
	ACharacter* FindClosestPlayer();

protected:
	// Current AI state
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI|State")
	EAIState CurrentState;

	// Reference to player character
	UPROPERTY()
	class ACharacter* PlayerCharacter;

	// Time since last saw player
	float TimeSinceLastSawPlayer;

	// Timer for waiting at roam destination
	float WaitTimer;

	// Current roam destination
	FVector CurrentRoamDestination;

	// Flag to check if AI reached destination
	bool bReachedDestination;

public:
	// Get current AI state
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "AI")
	EAIState GetCurrentState() const { return CurrentState; }
};

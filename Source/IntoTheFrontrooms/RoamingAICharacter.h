// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "RoamingAICharacter.generated.h"

/**
 * AI Character that roams and chases the player
 */
UCLASS()
class INTOTHEFRONTROOMS_API ARoamingAICharacter : public ACharacter
{
	GENERATED_BODY()

public:
	ARoamingAICharacter();

	// AI Settings
	
	/** How far the AI can see the player */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI Settings")
	float SightRange;

	/** How fast the AI moves when roaming */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI Settings")
	float RoamingSpeed;

	/** How fast the AI moves when chasing */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI Settings")
	float ChaseSpeed;

	/** Maximum distance for roaming from spawn point */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI Settings")
	float MaxRoamDistance;

	/** Time to wait at roam destination before moving again */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI Settings")
	float RoamWaitTime;

	/** Time after losing sight before stopping chase (in seconds) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI Settings")
	float LosePlayerTime;

	/** How close the AI needs to be to destination to consider it reached */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI Settings", meta = (ClampMin = "10.0", ClampMax = "500.0"))
	float AcceptanceRadius;

	// Attack Settings
	
	/** How close the AI needs to be to attack the player */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI Attack")
	float AttackRange;

	/** Damage dealt to player on attack */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI Attack")
	float AttackDamage;

	/** Time between attacks (cooldown) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI Attack")
	float AttackCooldown;

	/** Niagara smoke effect to spawn when respawning (UE5 - Preferred) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI Attack|Effects")
	class UNiagaraSystem* DespawnSmokeEffectNiagara;

	/** Legacy Cascade smoke effect (deprecated - only use if Niagara is not set) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI Attack|Effects", meta = (DisplayName = "Despawn Smoke Effect (Legacy)"))
	class UParticleSystem* DespawnSmokeEffectCascade;

	/** Scale of the smoke effect (1.0 = normal, 2.0 = double size) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI Attack|Effects", meta = (ClampMin = "0.5", ClampMax = "5.0"))
	float DespawnSmokeScale;

	/** How long the smoke effect stays before being destroyed (in seconds) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI Attack|Effects", meta = (ClampMin = "0.5", ClampMax = "5.0"))
	float DespawnSmokeLifetime;

	/** Sound to play when respawning */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI Attack|Effects")
	class USoundBase* DespawnSound;

	/** Whether to respawn at spawn point (true) or random roam location (false) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI Attack")
	bool bRespawnAtSpawnPoint;

	// Functions

	/** Attempt to attack the player if in range */
	UFUNCTION(BlueprintCallable, Category = "AI")
	bool TryAttackPlayer(ACharacter* Player);

	/** Respawn AI with effects at spawn point or random location */
	UFUNCTION(BlueprintCallable, Category = "AI")
	void RespawnWithEffects();

	/** Get the spawn location */
	UFUNCTION(BlueprintCallable, Category = "AI")
	FVector GetSpawnLocation() const { return SpawnLocation; }

	/** Check if AI can attack (cooldown finished) */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "AI")
	bool CanAttack() const;

protected:
	virtual void BeginPlay() override;

	// Store spawn location for roaming and respawning
	FVector SpawnLocation;

	// Track last attack time for cooldown
	float LastAttackTime;
};

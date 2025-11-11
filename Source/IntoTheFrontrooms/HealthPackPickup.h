// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PickupParent.h"
#include "HealthPackPickup.generated.h"

/**
 * Health pack pickup that restores health when collected
 */
UCLASS()
class INTOTHEFRONTROOMS_API AHealthPackPickup : public APickupParent
{
	GENERATED_BODY()

public:
	AHealthPackPickup();

protected:
	// Amount of health to restore
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Health Pack")
	float HealAmount;

	// Override the pickup behavior
	virtual void Pickup_Implementation(AIntoTheFrontroomsCharacter* OwningCharacter) override;
};

// Fill out your copyright notice in the Description page of Project Settings.

#include "HealthPackPickup.h"
#include "IntoTheFrontroomsCharacter.h"

AHealthPackPickup::AHealthPackPickup()
{
	// Default heal amount
	HealAmount = 25.0f;
}

void AHealthPackPickup::Pickup_Implementation(AIntoTheFrontroomsCharacter* OwningCharacter)
{
	if (OwningCharacter)
	{
		// This will trigger your Blueprint's health system
		// The health logic is handled entirely in your Character Blueprint
		UE_LOG(LogTemp, Log, TEXT("Health Pack: Player collected health pack (+%.0f HP)"), HealAmount);
		
		// Your Blueprint will handle the actual health increase via the "Health Increase" custom event
		// which you'll need to create in your Character Blueprint
	}

	// Call parent implementation to handle destruction, effects, etc.
	Super::Pickup_Implementation(OwningCharacter);
}

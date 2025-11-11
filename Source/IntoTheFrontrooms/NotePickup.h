// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PickupParent.h"
#include "NotePickup.generated.h"

/**
 * Note pickup that collects a note and broadcasts an event for the pause menu to display
 */
UCLASS()
class INTOTHEFRONTROOMS_API ANotePickup : public APickupParent
{
	GENERATED_BODY()

public:
	ANotePickup();

protected:
	// Title of the note (displayed in pause menu)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Note")
	FText NoteTitle;

	// Content/description of the note
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Note", meta = (MultiLine = true))
	FText NoteContent;

	// Optional image/texture for the note
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Note")
	UTexture2D* NoteImage;

	// Unique ID for this note (useful for tracking collected notes)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Note")
	FName NoteID;

	// Override the pickup behavior
	virtual void Pickup_Implementation(AIntoTheFrontroomsCharacter* OwningCharacter) override;
};

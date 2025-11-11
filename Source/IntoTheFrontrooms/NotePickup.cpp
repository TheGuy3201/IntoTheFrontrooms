// Fill out your copyright notice in the Description page of Project Settings.

#include "NotePickup.h"
#include "IntoTheFrontroomsCharacter.h"

ANotePickup::ANotePickup()
{
	// Default note properties
	NoteTitle = FText::FromString("Untitled Note");
	NoteContent = FText::FromString("Set note content in Blueprint...");
	NoteID = FName("Note_Default");
	NoteImage = nullptr;
}

void ANotePickup::Pickup_Implementation(AIntoTheFrontroomsCharacter* OwningCharacter)
{
	if (OwningCharacter)
	{
		// Add the note to the character's collected notes
		OwningCharacter->AddNote(NoteID, NoteTitle, NoteContent, NoteImage);
		
		UE_LOG(LogTemp, Log, TEXT("Note Pickup: '%s' collected by %s"), *NoteTitle.ToString(), *OwningCharacter->GetName());
	}

	// Call parent implementation to handle destruction, sound, effects, etc.
	Super::Pickup_Implementation(OwningCharacter);
}

// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Logging/LogMacros.h"
#include "IntoTheFrontroomsCharacter.generated.h"

class UInputComponent;
class USkeletalMeshComponent;
class UCameraComponent;
class UInputAction;
class UInputMappingContext;
struct FInputActionValue;
class AIntoTheFrontroomsHUD;

DECLARE_LOG_CATEGORY_EXTERN(LogTemplateCharacter, Log, All);

// Structure to hold note data
USTRUCT(BlueprintType)
struct FCollectedNote
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, Category = "Note")
	FName NoteID;

	UPROPERTY(BlueprintReadOnly, Category = "Note")
	FText NoteTitle;

	UPROPERTY(BlueprintReadOnly, Category = "Note")
	FText NoteContent;

	UPROPERTY(BlueprintReadOnly, Category = "Note")
	UTexture2D* NoteImage;

	FCollectedNote()
		: NoteID(NAME_None)
		, NoteTitle(FText::GetEmpty())
		, NoteContent(FText::GetEmpty())
		, NoteImage(nullptr)
	{}
};

UCLASS(config=Game)
class AIntoTheFrontroomsCharacter : public ACharacter
{
	GENERATED_BODY()

	/** Pawn mesh: 1st person view (arms; seen only by self) */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Mesh, meta = (AllowPrivateAccess = "true"))
	USkeletalMeshComponent* Mesh1P;

	/** First person camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	UCameraComponent* FirstPersonCameraComponent;

	/** MappingContext */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputMappingContext* DefaultMappingContext;

	/** Jump Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Input, meta=(AllowPrivateAccess = "true"))
	UInputAction* JumpAction;

	/** Move Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Input, meta=(AllowPrivateAccess = "true"))
	UInputAction* MoveAction;

	/** Look Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* LookAction;
	
public:
	AIntoTheFrontroomsCharacter();

protected:
	virtual void BeginPlay();

public:	
	virtual void Tick(float DeltaTime) override;

	/** Returns Mesh1P subobject **/
	USkeletalMeshComponent* GetMesh1P() const { return Mesh1P; }
	/** Returns FirstPersonCameraComponent subobject **/
	UCameraComponent* GetFirstPersonCameraComponent() const { return FirstPersonCameraComponent; }

protected:
	/** Called for movement input */
	void Move(const FInputActionValue& Value);

	/** Called for looking input */
	void Look(const FInputActionValue& Value);

protected:
	// APawn interface
	virtual void NotifyControllerChanged() override;
	virtual void SetupPlayerInputComponent(UInputComponent* InputComponent) override;
	// End of APawn interface

public:
	// Note Collection System (ONLY NEW ADDITION)

	/** Array of collected notes */
	UPROPERTY(BlueprintReadOnly, Category = "Notes")
	TArray<FCollectedNote> CollectedNotes;

	/** Add a note to the collected notes array */
	UFUNCTION(BlueprintCallable, Category = "Notes")
	void AddNote(FName NoteID, FText NoteTitle, FText NoteContent, UTexture2D* NoteImage);

	/** Get all collected notes (for pause menu) */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Notes")
	TArray<FCollectedNote> GetCollectedNotes() const { return CollectedNotes; }

	/** Check if a specific note has been collected */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Notes")
	bool HasCollectedNote(FName NoteID) const;

	/** Blueprint event called when a note is collected - Use this in your pause menu to show notification */
	UFUNCTION(BlueprintImplementableEvent, Category = "Notes")
	void OnNoteCollected(const FCollectedNote& Note);
};


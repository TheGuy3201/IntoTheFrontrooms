// Copyright Epic Games, Inc. All Rights Reserved.

#include "IntoTheFrontroomsGameMode.h"
#include "IntoTheFrontroomsCharacter.h"
#include "IntoTheFrontroomsHUD.h"
#include "UObject/ConstructorHelpers.h"

AIntoTheFrontroomsGameMode::AIntoTheFrontroomsGameMode()
	: Super()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnClassFinder(TEXT("/Game/FirstPerson/Blueprints/BP_FirstPersonCharacter"));
	DefaultPawnClass = PlayerPawnClassFinder.Class;

	// Set our custom HUD class
	HUDClass = AIntoTheFrontroomsHUD::StaticClass();
}
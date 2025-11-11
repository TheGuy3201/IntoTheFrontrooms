// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "IntoTheFrontroomsHUD.generated.h"

class UUserWidget;

/**
 * HUD class that manages gameplay and end game UI
 * Handles timer display and final score screen
 */
UCLASS()
class AIntoTheFrontroomsHUD : public AHUD
{
	GENERATED_BODY()

public:
	AIntoTheFrontroomsHUD();

	/** Called when the game starts */
	virtual void BeginPlay() override;

	/** Update the timer display */
	UFUNCTION(BlueprintCallable, Category = "HUD")
	void UpdateTimer(float CurrentTime);

	/** Update health bar percentage */
	UFUNCTION(BlueprintCallable, Category = "HUD")
	void UpdateHealthBar(float HealthPercent);

	/** Show the end game screen with final score */
	UFUNCTION(BlueprintCallable, Category = "HUD")
	void ShowEndGameScreen(float FinalScore, float FinalTime);

	/** Hide the gameplay UI */
	UFUNCTION(BlueprintCallable, Category = "HUD")
	void HideGameplayUI();

protected:
	/** Widget class for gameplay UI (set in Blueprint) */
	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<UUserWidget> GameplayUIClass;

	/** Widget class for end game UI (set in Blueprint) */
	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<UUserWidget> EndGameUIClass;

	/** Reference to the active gameplay widget */
	UPROPERTY()
	UUserWidget* GameplayUIWidget;

	/** Reference to the active end game widget */
	UPROPERTY()
	UUserWidget* EndGameUIWidget;

private:
	/** Create and show the gameplay UI */
	void CreateGameplayUI();

	/** Create and show the end game UI */
	void CreateEndGameUI(float FinalScore, float FinalTime);
};

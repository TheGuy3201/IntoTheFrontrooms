// Copyright Epic Games, Inc. All Rights Reserved.

#include "IntoTheFrontroomsHUD.h"
#include "Blueprint/UserWidget.h"
#include "Components/TextBlock.h"
#include "Components/ProgressBar.h"
#include "GameFramework/PlayerController.h"

AIntoTheFrontroomsHUD::AIntoTheFrontroomsHUD()
{
	// Constructor
}

void AIntoTheFrontroomsHUD::BeginPlay()
{
	Super::BeginPlay();

	// Only create UI for local player controller
	APlayerController* PC = GetOwningPlayerController();
	if (PC && PC->IsLocalController())
	{
		CreateGameplayUI();
	}
}

void AIntoTheFrontroomsHUD::CreateGameplayUI()
{
	if (!GameplayUIClass)
	{
		UE_LOG(LogTemp, Warning, TEXT("GameplayUIClass is not set in HUD Blueprint!"));
		return;
	}

	APlayerController* PC = GetOwningPlayerController();
	if (!PC || !PC->IsLocalController())
	{
		return;
	}

	// Create the gameplay widget
	GameplayUIWidget = CreateWidget<UUserWidget>(PC, GameplayUIClass);
	if (GameplayUIWidget)
	{
		GameplayUIWidget->AddToViewport(0);
		UE_LOG(LogTemp, Log, TEXT("Gameplay UI created successfully"));
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to create Gameplay UI widget"));
	}
}

void AIntoTheFrontroomsHUD::UpdateTimer(float CurrentTime)
{
	if (!GameplayUIWidget || !IsValid(GameplayUIWidget))
	{
		return;
	}

	// Find the Timer text widget by name
	UTextBlock* TimerText = Cast<UTextBlock>(GameplayUIWidget->GetWidgetFromName(TEXT("TimerTXT")));
	if (TimerText)
	{
		// Format time as MM:SS
		int32 Minutes = FMath::FloorToInt(CurrentTime / 60.0f);
		int32 Seconds = FMath::FloorToInt(FMath::Fmod(CurrentTime, 60.0f));
		FString TimeString = FString::Printf(TEXT("%02d:%02d"), Minutes, Seconds);
		TimerText->SetText(FText::FromString(TimeString));
	}
}

void AIntoTheFrontroomsHUD::UpdateHealthBar(float HealthPercent)
{
	if (!GameplayUIWidget || !IsValid(GameplayUIWidget))
	{
		return;
	}

	// Find the health bar widget by name
	UProgressBar* HealthBar = Cast<UProgressBar>(GameplayUIWidget->GetWidgetFromName(TEXT("PG_Healthbar")));
	if (HealthBar)
	{
		HealthBar->SetPercent(FMath::Clamp(HealthPercent, 0.0f, 1.0f));
	}
}

void AIntoTheFrontroomsHUD::ShowEndGameScreen(float FinalScore, float FinalTime)
{
	// Hide gameplay UI first
	HideGameplayUI();

	APlayerController* PC = GetOwningPlayerController();
	if (!PC || !PC->IsLocalController())
	{
		return;
	}

	if (!EndGameUIClass)
	{
		UE_LOG(LogTemp, Warning, TEXT("EndGameUIClass is not set in HUD Blueprint!"));
		return;
	}

	// Create the end game widget
	EndGameUIWidget = CreateWidget<UUserWidget>(PC, EndGameUIClass);
	if (EndGameUIWidget)
	{
		EndGameUIWidget->AddToViewport(1); // Higher Z-order

		// Set final score and time
		UTextBlock* ScoreText = Cast<UTextBlock>(EndGameUIWidget->GetWidgetFromName(TEXT("FinalScoreText")));
		if (ScoreText)
		{
			FString ScoreString = FString::Printf(TEXT("Final Score: %.0f"), FinalScore);
			ScoreText->SetText(FText::FromString(ScoreString));
		}

		UTextBlock* TimeText = Cast<UTextBlock>(EndGameUIWidget->GetWidgetFromName(TEXT("FinalTimeText")));
		if (TimeText)
		{
			int32 Minutes = FMath::FloorToInt(FinalTime / 60.0f);
			int32 Seconds = FMath::FloorToInt(FMath::Fmod(FinalTime, 60.0f));
			FString TimeString = FString::Printf(TEXT("Time: %02d:%02d"), Minutes, Seconds);
			TimeText->SetText(FText::FromString(TimeString));
		}

		// Show mouse cursor for menu interaction
		PC->bShowMouseCursor = true;
		PC->SetInputMode(FInputModeUIOnly());

		UE_LOG(LogTemp, Log, TEXT("End Game UI created - Score: %.0f, Time: %.1f"), FinalScore, FinalTime);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to create End Game UI widget"));
	}
}

void AIntoTheFrontroomsHUD::HideGameplayUI()
{
	if (GameplayUIWidget && IsValid(GameplayUIWidget))
	{
		GameplayUIWidget->RemoveFromParent();
		GameplayUIWidget = nullptr;
	}
}

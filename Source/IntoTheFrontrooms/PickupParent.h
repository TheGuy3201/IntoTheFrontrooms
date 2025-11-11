// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PickupParent.generated.h"

class USphereComponent;
class UStaticMeshComponent;
class AIntoTheFrontroomsCharacter;
class USoundBase;
class UParticleSystem;

UCLASS()
class INTOTHEFRONTROOMS_API APickupParent : public AActor
{
	GENERATED_BODY()

public:
	APickupParent();

protected:
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;

protected:
	// Sphere collision for pickup detection
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Pickup")
	USphereComponent* SphereCollision;

	// Mesh for visual representation (set in Blueprint)
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Pickup")
	UStaticMeshComponent* Mesh;

	// Optional sound to play when picked up
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pickup|Effects")
	USoundBase* PickupSound;

	// Optional particle effect to spawn when picked up
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pickup|Effects")
	UParticleSystem* PickupEffect;

	// Called when overlap begins
	UFUNCTION()
	void OnBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

public:
	// BlueprintNativeEvent for pickup logic, can be overridden in BP
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Pickup")
	void Pickup(AIntoTheFrontroomsCharacter* OwningCharacter);
	virtual void Pickup_Implementation(AIntoTheFrontroomsCharacter* OwningCharacter);
};

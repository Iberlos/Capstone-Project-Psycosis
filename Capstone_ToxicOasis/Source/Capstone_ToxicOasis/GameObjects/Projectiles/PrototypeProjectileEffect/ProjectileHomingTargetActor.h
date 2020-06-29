// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ProjectileHomingTargetActor.generated.h"

UCLASS()
class CAPSTONE_TOXICOASIS_API AProjectileHomingTargetActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AProjectileHomingTargetActor();

	//Getters - Setters
#pragma region
	USceneComponent* GetTargetComponent() { return m_targetComp; }
#pragma endregion
	

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(EditDefaultsOnly, Category = Resources, Replicated)
		USceneComponent* m_targetComp;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};

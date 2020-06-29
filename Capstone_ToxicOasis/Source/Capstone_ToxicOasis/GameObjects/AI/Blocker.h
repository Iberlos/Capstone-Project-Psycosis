// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameObjects/MyActor.h"
#include "Blocker.generated.h"

UCLASS()
class CAPSTONE_TOXICOASIS_API ABlocker : public AMyActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ABlocker();

	/*Set the blocker to start updating*/
	void StartUpdate();
	/*Stop Update when the spawner becomes Inactive*/
	void StopUpdate();

	void TurnOn();
#pragma region
private:
	UFUNCTION(Server, Reliable, WithValidation)
		void SERVER_TurnOn();
	bool SERVER_TurnOn_Validate() { return true; }
public:
#pragma endregion
	void TurnOff();
#pragma region
private:
	UFUNCTION(Server, Reliable, WithValidation)
		void SERVER_TurnOff();
	bool SERVER_TurnOff_Validate() { return true; }
public:
#pragma endregion
	void HideMesh(bool a_hide);
#pragma region
private:
	UFUNCTION(NetMulticast, Reliable)
	void Net_HideMesh(bool a_hide);
public:
#pragma endregion

	void ToggleCollider(bool collisionEnabled);
#pragma region
private:
	UFUNCTION(NetMulticast, Reliable)
		void Net_ToggleCollider(bool collisionEnabled);
public:
#pragma endregion
	void UpdateBlocker();
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(EditInstanceOnly, Category = "Blocker")
		class AAISpawner* m_owningSpawner;

	UPROPERTY(EditAnywhere, Category = "Blocker")
		BlockerType m_blockerType;

	UPROPERTY(EditDefaultsOnly, Replicated ,Category = "Blocker")
		class UStaticMeshComponent* m_staticMeshComp_r;
public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
private:
	bool m_arenaHasBeenCleared;
};

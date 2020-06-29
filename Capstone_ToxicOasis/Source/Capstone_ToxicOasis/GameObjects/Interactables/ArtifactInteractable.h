// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameObjects/Interactables/InteractableActor.h"
#include "ArtifactInteractable.generated.h"

/**
 * This class is meant to send an event when interact with and then deactivate itself
 */
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnArtifactInteractedWith);

UCLASS()
class CAPSTONE_TOXICOASIS_API AArtifactInteractable : public AInteractableActor
{
	GENERATED_BODY()
public:
	//Essentials
	AArtifactInteractable();
	void Tick(float DeltaTime) override;
	//Other Methods
	virtual bool Interact(AActor* a_actor) override;
	void InitLifetime();
#pragma region
private:
	//...
public:
#pragma endregion
	//OnEvents

	//Components

	//Variables
	UPROPERTY(BlueprintAssignable, Category = "EventDispatchers")
		FOnArtifactInteractedWith ed_ArtifactInteractedWith;
	//Getters and Setters

protected:
	//Essentials
	void BeginPlay() override;
	//Other Methods
#pragma region
private:
	UFUNCTION(Server, Reliable, WithValidation)
		void SERVER_DisableArtifact();
	bool SERVER_DisableArtifact_Validate() { return true; };
	UFUNCTION(NetMulticast, Reliable, WithValidation)
		void NET_DisableArtifact();
	bool NET_DisableArtifact_Validate() { return true; };
protected:
#pragma endregion
	//OnEvents

	//Components

	//Variables
	AActor* m_pointedActor;
};

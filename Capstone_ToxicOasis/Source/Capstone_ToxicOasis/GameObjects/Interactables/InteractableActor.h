// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameObjects/MyActor.h"
#include "InteractableActor.generated.h"

/*************************************
Class: InteractableActor
Author: Luis Filipe Moraes
Created: october 2019
Last Modified: 30/jan/2020
Description: Base class for all Interactable classes in the game such as ChestInteractable, BedInteractable and PickupInteractable. this class allows the subclasses override an Interact function that will be called by the player when the interact key is pressed while looking at an interactable.
References:
Change Log
**********
Date: unknown
Author: Luis Filipe Moraes
Verified By: Lucas Felix & Abdul Ali
Changes: Added all the data and functionality to allow the function to play its particles and sounds as requested when interacted with and then return a boolean if it was successfully interacted with.
Date: between 20/jan and 30/jan
Author: Luis Filipe Moraes
Verified By: Lucas Felix & Abdul Ali
Changes: added the Interact key to the animation map and called the playAnimationMontage in the base Interact function.
*************************************/
UCLASS()
class CAPSTONE_TOXICOASIS_API AInteractableActor : public AMyActor
{
	GENERATED_BODY()
	
public:
	//Essentials
	AInteractableActor();
	//Other Methods
	void RegisterMesh(USkeletalMeshComponent* a_mesh); //May add a check so the same mesh can't be added twice.
	void UnregisterMesh(USkeletalMeshComponent* a_mesh) { m_registeredMeshes_r.Remove(a_mesh); } //May add a check to see if the mesh actually is registered
	virtual void Focus();
	virtual void Unfocus();
	virtual bool Interact(AActor* a_actor);
#pragma region
private:
	//...
public:
#pragma endregion
	//OnEvents
	//Components
	//Variables
	//Getters and Setters
	bool GetIsActiveInteractable() { return m_isActiveInteractable_r; }
	void SetIsActiveInteractable(bool a_isActiveInteractable) { m_isActiveInteractable_r = a_isActiveInteractable; }
protected:
	//Essentials
	void BeginPlay() override;
	//Other Methods
#pragma region
private:
	//...
protected:
#pragma endregion
	//OnEvents
	//Components
	//Variables
	UPROPERTY(Replicated)
	TArray<class USkeletalMeshComponent*> m_registeredMeshes_r;
	UPROPERTY(EditDefaultsOnly, Replicated, Category = "Dev. Data|Interactable Actor Data|Variables")
		bool m_isActiveInteractable_r;
};

// Fill out your copyright notice in the Description page of Project Settings.

//Generated include #must be included last because unreal complains if there are any includes between it and the generated body thingy.#
#pragma once

#include "CoreMinimal.h"
#include "GameObjects/Interactables/InteractableActor.h"
//Custom Unreal includes #There shouldn't be any includes in the .h file, but if one is needed it goes here.#

//Custom project includes #There shouldn't be any includes in the .h file, but if one is needed it goes here.#
#include "EnumMaster/EnumMaster.h"

//Generated include #must be included last because unreal complains if there are any includes between it and the generated body thingy.#
#include "PickupInteractable.generated.h"


/*************************************
Class: PickupInteractable
Author: Luis Filipe Moraes
Created: october 2019
Last Modified: 21/feb/2020
Description: This class is the base class for all pickups and defined the standard behaviour for interactions of the pickup type. Functions in this class must be overriden and defined in the base class to deal with their specific logic.
References:
Change Log
**********
Date: unknown
Author: Luis Filipe Moraes
Verified By: Lucas Felix & Abdul Ali
Changes: Added all the functionlaity related to the base fucntionality of the class.
----------------------------------------------------------------------------------
Date: unknown
Author: Luis Filipe Moraes
Verified By: Lucas Felix & Abdul Ali
Changes: Modified the default pickup type to be Invalid instead of artifacts, this will cause the inventory to actually throw out errors instead of adding unknow items into the poutdated invisible artifact tab.
----------------------------------------------------------------------------------
*************************************/
UCLASS()
class CAPSTONE_TOXICOASIS_API APickupInteractable : public AInteractableActor
{
	GENERATED_BODY()
	
public:
	//Essentials
	APickupInteractable();
	//Other Methods
	bool Interact(AActor* a_actor) override;
	virtual bool Use(AActor* a_actor);
	virtual void Drop(AActor* a_actor);
#pragma region
private:
	//...
public:
#pragma endregion
	//OnEvents
	//Components
	//Variables
	//Getters and Setters
	class UTexture* GetHUDImage() { return m_HUDImage; }
	void SetHUDImage(class UTexture* a_HUDImage) { m_HUDImage = a_HUDImage; }
	bool GetIsUseOnContact() { return m_isUseOnContact; }
	void SetIsUseOnContact(bool a_isUseOnContact) { m_isUseOnContact = a_isUseOnContact; } //probably will never be used outside of the class
	bool GetIsConsumeOnUse() { return m_isConsumeOnUse; }
	void SetIsConsumeOnUse(bool a_isConsumeOnUse) { m_isConsumeOnUse = a_isConsumeOnUse; } //probably will never be used outside of the class
	int GetUses() { return m_uses; }
	void SetUses(int a_uses) { m_uses = a_uses; } //probably will never be used outside of the class
	enum InventoryTabEnum GetPickupType() {return m_pickupType;}
    void SetPickupType(InventoryTabEnum a_pickupType) { m_pickupType = a_pickupType; }
protected:
	//Essentials
	void BeginPlay();
	//Other Methods
	bool AddSelfToInventory(AActor* a_actor);
#pragma region
private:
	UFUNCTION(NetMulticast, Reliable)
		void NET_Deactivate();
protected:
#pragma endregion
#pragma region
private:
	UFUNCTION(NetMulticast, Reliable)
		void NET_Activate();
protected:
#pragma endregion
	//OnEvents
	//Components
	//Variables
	UPROPERTY(EditDefaultsOnly, Category = "Dev. Data|Pickup Interactable Data|Variables")
		class UTexture* m_HUDImage;
	UPROPERTY(EditDefaultsOnly, Category = "Dev. Data|Pickup Interactable Data|Variables")
		bool m_isUseOnContact;
	UPROPERTY(EditDefaultsOnly, Category = "Dev. Data|Pickup Interactable Data|Variables")
		bool m_isConsumeOnUse;
	UPROPERTY(EditDefaultsOnly, Category = "Dev. Data|Pickup Interactable Data|Variables")
		int m_uses;
	UPROPERTY(EditDefaultsOnly, Category = "Dev. Data|Pickup Interactable Data|Variables")
		InventoryTabEnum m_pickupType;
};

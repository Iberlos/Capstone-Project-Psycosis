// Fill out your copyright notice in the Description page of Project Settings.

//Default Unreal Includes
#pragma once

#include "CoreMinimal.h"
#include "Custom_Components/MyActorComponent.h"
//Custom Unreal includes #There shouldn't be any includes in the .h file, but if one is needed it goes here.#

//Custom project includes #There shouldn't be any includes in the .h file, but if one is needed it goes here.#
#include <Capstone_ToxicOasis\EnumMaster\EnumMaster.h>

//Generated include #must be included last because unreal complains if there are any includes between it and the generated body thingy.#
#include "InventoryComponent.generated.h"

/*************************************
Class: InventoryComponent
Author: Luis Filipe Moraes
Created: october 2019
Last Modified: 24/feb/2020
Description: This class is responsible for holding and managing pickupInteractables for players, chests and enemies.
References:
Change Log
**********
Date: unknown
Author: Luis Filipe Moraes
Verified By: Lucas Felix & Abdul Ali
Changes: Added all the functionlaity related to the base fucntionality of the class.
----------------------------------------------------------------------------------
Date: 20/feb/2020
Author: Luis Filipe Moraes
Verified By: Lucas Felix & Abdul Ali
Changes: modified the logic slightly to allow for PDWs to be stored in their own tab
----------------------------------------------------------------------------------
Date: 3/mar/2020
Author: Luis Filipe Moraes
Verified By: Lucas Felix & Abdul Ali
Changes: Modified the UseSelected function logic to fix a glitch that caused no weapons to be equipped.
----------------------------------------------------------------------------------
*************************************/
USTRUCT(BlueprintType)
struct FTabEnvelope
{
	GENERATED_BODY()

public: //REALLY?
	UPROPERTY()
		TArray<class APickupInteractable*> m_tab;
};

UCLASS()
class CAPSTONE_TOXICOASIS_API UInventoryComponent : public UMyActorComponent
{
	GENERATED_BODY()
	
public:
	//Essentials
	UInventoryComponent();
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	//Other Methods
	void MouseInputVertical(float a_val);
#pragma region
private:
	UFUNCTION(Server, Reliable, WithVAlidation)
		void SERVER_MouseInputVertical(float a_val);
		bool SERVER_MouseInputVertical_Validate(float a_val) { return true; }
public:
#pragma endregion
	void MouseInputHorizontal(float a_val);
#pragma region
private:
	UFUNCTION(Server, Reliable, WithVAlidation)
		void SERVER_MouseInputHorizontal(float a_val);
		bool SERVER_MouseInputHorizontal_Validate(float a_val) { return true; }
public:
#pragma endregion
	bool AddPickup(class APickupInteractable* a_pickup);
	bool AddPickups(TArray<class APickupInteractable*> a_pickups);
    UFUNCTION(BlueprintCallable)
	    bool UseSelected(AActor* a_actor);
    UFUNCTION(BlueprintCallable)
        bool UseAt(AActor* a_actor, InventoryTabEnum a_tab, int a_tabIndex);
	void DropSelected(AActor* a_actor);
	void DropPickup(AActor* a_actor, APickupInteractable* a_pickup);
	void DisplayInventory(bool a_isDisplayingInventory_r);
	bool FlushInventory();
	void EquipAnyWeapon(InventoryTabEnum a_tab = InventoryTabEnum::ITE_Invalid);
	//OnEvents
	//Components
	//Variables
	//Getters and Setters
	TArray<TArray<class APickupInteractable*>> GetInventoryItems;
    UFUNCTION(BlueprintCallable)
        TArray< APickupInteractable*> GetPistolInventory() { return m_inventoryItems_r[0].m_tab; }
    UFUNCTION(BlueprintCallable)
        TArray< APickupInteractable*> GetShotgunInventory() { return m_inventoryItems_r[1].m_tab; }
    UFUNCTION(BlueprintCallable)
        TArray< APickupInteractable*> GetRifleInventory() { return m_inventoryItems_r[2].m_tab; }
	UFUNCTION(BlueprintCallable)
		TArray< APickupInteractable*> GetPDWInventory() { return m_inventoryItems_r[3].m_tab; }
	float GetDropDistance() { return m_dropDistance; }
	void SetDropDistance(float a_dropDistance) { m_dropDistance = a_dropDistance; }
	UFUNCTION(BlueprintCallable)
		bool GetIsDisplayingInventory() { return m_isDisplayingInventory_r; }
	UFUNCTION(BlueprintCallable)
		InventoryTabEnum GetDisplayedTab() {return m_displayedTab_r;}
	UFUNCTION(BlueprintCallable)
		int GetSelectedPickup() { return m_selectedPickup_r; }
	UFUNCTION(BlueprintCallable)
		FVector GetRelativeMouseDirection() { return m_relativeMouseDirection_r; }
	class UArrowComponent* GetArrowComponent() { return m_arrowComponent; }
	APickupInteractable* GetDefaultWeapon() { return m_defaultWeapon; }
	void SetDefaultWeapon(APickupInteractable* a_defaultWeapon) { m_defaultWeapon = a_defaultWeapon; }

protected:
	//Essentials
	void BeginPlay();
	//Other Methods
	void CalculateTab();
	void CalculateSelected();
	//OnEvents
	//Components
	UPROPERTY(EditDefaultsOnly, Category = "Dev. Data")
		class UArrowComponent* m_arrowComponent;
	//Variables
	UPROPERTY(Replicated)
		APickupInteractable* m_defaultWeapon;
	UPROPERTY(Replicated)
	 TArray<FTabEnvelope> m_inventoryItems_r; //nested containers are not supported for replication so I had to replicated the individual tabs
	UPROPERTY(Replicated)
		FVector m_relativeMouseDirection_r;
	UPROPERTY(Replicated)
		bool m_isDisplayingInventory_r;
	UPROPERTY(Replicated)
		InventoryTabEnum m_displayedTab_r;
	UPROPERTY(Replicated)
		int m_selectedPickup_r;
	UPROPERTY(EditDefaultsOnly, Category = "Dev. Data")
		float m_dropDistance;
	UPROPERTY(EditDefaultsOnly, Category = "Dev. Data")
		float m_exitButtonAngle;
	UPROPERTY(EditDefaultsOnly, Category = "Dev. Data")
		float m_relativeMouseDistanceForActivation;
    UPROPERTY(EditDefaultsOnly, Category = "Dev. Data")
        float m_UIMouseSpeed;
	UPROPERTY(EditDefaultsOnly, Category = "Dev. Data")
		float m_dropHalfAngle;
	UPROPERTY(EditDefaultsOnly, Category = "Dev. Data")
		float m_dropForce;
};

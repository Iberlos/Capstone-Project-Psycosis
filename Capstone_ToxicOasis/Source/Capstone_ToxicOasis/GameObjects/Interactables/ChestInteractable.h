// Fill out your copyright notice in the Description page of Project Settings.

//Default Unreal Includes
#pragma once

#include "CoreMinimal.h"
#include "GameObjects/Interactables/InteractableActor.h"
//Custom Unreal includes #There shouldn't be any includes in the .h file, but if one is needed it goes here.#

//Custom project includes #There shouldn't be any includes in the .h file, but if one is needed it goes here.#
#include "EnumMaster/EnumMaster.h"

//Generated include #must be included last because unreal complains if there are any includes between it and the generated body thingy.#
#include "ChestInteractable.generated.h"

/**
 * 
 */
UCLASS()
class CAPSTONE_TOXICOASIS_API AChestInteractable : public AInteractableActor
{
	GENERATED_BODY()
	
public:
	//Essentials
	AChestInteractable();
	//Other Methods
	bool Interact(AActor* a_actor) override;
	bool Open();
#pragma region
private:
	//...
public:
#pragma endregion
	//OnEvents
	//Components
	//Variables
	//Getters and Setters
	class UInventoryComponent* GetInventoryComponent() { return m_inventoryComponent; }

protected:
	//Essentials
	void BeginPlay();
	//Other Methods
#pragma region
private:
	//...
protected:
#pragma endregion
	//OnEvents
	//Components
	UPROPERTY(EditDefaultsOnly, Category = "Dev. Data|Chest Interactable Data|Components")
		class UInventoryComponent* m_inventoryComponent;
	UPROPERTY(EditDefaultsOnly, Category = "Dev. Data|Chest Interactable Data|Components")
		class UItemGeneratorComponent* m_itemGeneratorComponent;
	//Variables
	UPROPERTY(EditDefaultsOnly, Category = "Dev. Data|Chest Interactable Data|Variables")
		ChestModeEnum m_chestMode;
	UPROPERTY(EditDefaultsOnly, Category = "Dev. Data|Chest Interactable Data|Variables")
		int m_amountToSpawn;
};

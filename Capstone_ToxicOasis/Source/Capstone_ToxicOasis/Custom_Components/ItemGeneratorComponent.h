// Fill out your copyright notice in the Description page of Project Settings.

//Default Unreal Includes
#pragma once

#include "CoreMinimal.h"
#include "Custom_Components/MyActorComponent.h"
//Custom Unreal includes #There shouldn't be any includes in the .h file, but if one is needed it goes here.#

//Custom project includes #There shouldn't be any includes in the .h file, but if one is needed it goes here.#

//Generated include #must be included last because unreal complains if there are any includes between it and the generated body thingy.#
#include "ItemGeneratorComponent.generated.h"

/*************************************
Class: ItemGeneratorComponent
Author: Luis
Created: october 2019
Last Modified: 3/mar/2020
Description: This class is resposible for droptables and spawning of items to be dropped by chest and enemies
References:
Change Log
**********
Date: unknown
Author: Luis
Verified By: Lucas Felix & Abdul Ali
Changes: Added all the functionlaity related to the base fucntionality of the class.
--------------------------------------------------------
Date: 6/mar/2020
Author: Luis Filipe Moraes
Verified By: Lucas Felix & Abdul Ali
Changes: Modified the weighted random to work in the correct way
--------------------------------------------------------
*************************************/
USTRUCT(BlueprintType)
struct FDropDefinitionStruct
{
	GENERATED_BODY()

public: //REALLY?
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
		class TSubclassOf<class APickupInteractable> m_pickup;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
		int m_min;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
		int m_max;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
		float m_weight;
};

UCLASS()
class CAPSTONE_TOXICOASIS_API UItemGeneratorComponent : public UMyActorComponent
{
	GENERATED_BODY()

public:
	//Essentials
	UItemGeneratorComponent();
	//Other Methods
	TArray<class APickupInteractable*> Generate();
	TArray<class APickupInteractable*> Generate(int a_amount);
#pragma region
private:
	//...
public:
#pragma endregion
	//OnEvents
	//Components
	//Variables
	//Getters and Setters

protected:
	//Essentials
	void BeginPlay();
	//Other Methods
	void SpawnPickup(TArray<class APickupInteractable*>* a_outArray, TSubclassOf<APickupInteractable> a_pickup);
#pragma region
private:
	//...
protected:
#pragma endregion
	//OnEvents
	//Components
	//Variables
	UPROPERTY(EditDefaultsOnly, Category = "Dev. Data")
		TArray<FDropDefinitionStruct> m_dropTable;
};

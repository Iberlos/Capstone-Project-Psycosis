// Fill out your copyright notice in the Description page of Project Settings.

//Header include
#include "ChestInteractable.h"

//Custom Engine Includes
#include "Components/SkeletalMeshComponent.h"
#include "Components/ArrowComponent.h"

//Custom Project Includes
#include "GameObjects/Interactables/Pickups/PickupInteractable.h"
#include "Custom_Components/InventoryComponent.h"
#include "Custom_Components/ItemGeneratorComponent.h"

AChestInteractable::AChestInteractable()
{
	Cast<UPrimitiveComponent>(RootComponent)->SetSimulatePhysics(false);
	m_inventoryComponent = CreateDefaultSubobject<UInventoryComponent>(TEXT("Inventory Component"));
	m_itemGeneratorComponent = CreateDefaultSubobject<UItemGeneratorComponent>(TEXT("Item Generator Component"));

	m_inventoryComponent->GetArrowComponent()->SetRelativeRotation(FRotator(45,0,0).Quaternion());

}

void AChestInteractable::BeginPlay()
{
	Super::BeginPlay();
}

bool AChestInteractable::Interact(AActor* a_actor)
{
	if (Super::Interact(a_actor))//if you can't be highlited don't call your functions too
	{
		m_isActiveInteractable_r = false;
		return Open();
	}
	else
	{
		return false;
	}
}

bool AChestInteractable::Open()
{
	//Generate Items
	TArray<APickupInteractable*> generatedPickups;
	if (m_chestMode == ChestModeEnum::CME_GenerateFromTable || m_chestMode == ChestModeEnum::CME_GenerateBoth)
	{
		generatedPickups = m_itemGeneratorComponent->Generate();
	}
	if (m_chestMode == ChestModeEnum::CME_GenerateByWeight || m_chestMode == ChestModeEnum::CME_GenerateBoth)
	{
		generatedPickups = m_itemGeneratorComponent->Generate(m_amountToSpawn);
	}
	//Interact with all pickups
	if (generatedPickups.Num() != 0)
	{
		for (APickupInteractable* pickup: generatedPickups)
		{
			pickup->Interact(this);
		}
	}
	//Drop all the pickups in the world
	return m_inventoryComponent->FlushInventory();
}
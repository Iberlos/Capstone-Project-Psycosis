// Fill out your copyright notice in the Description page of Project Settings.

//Header include
#include "ItemGeneratorComponent.h"

//Custom Engine Includes
#include "Engine.h"
#include "Kismet/GameplayStatics.h"

//Custom Project Includes
#include "GameInstance/CapstoneGameInstance.h"
#include "GameObjects/Interactables/Pickups/PickupInteractable.h"
#include "GameObjects/Interactables/Pickups/Weapons/WeaponPickup.h"
#include "GameObjects/Interactables/ChestInteractable.h"
#include "GameObjects/Weapons/WeaponGeneratorActor.h"
#include "GameMode/MainGameMode.h"

UItemGeneratorComponent::UItemGeneratorComponent()
{

}

void UItemGeneratorComponent::BeginPlay()
{
	Super::BeginPlay();
}

TArray<class APickupInteractable*> UItemGeneratorComponent::Generate()
{
	TArray<class APickupInteractable*> returnValue;
	if (m_dropTable.Num() > 0)
	{
		for (FDropDefinitionStruct dropItem: m_dropTable)
		{
			int numToSpawn = FMath::RandRange(dropItem.m_min, dropItem.m_max); //generate a number of items to spawn between the min and max allowed
			for (int i = 0; i < numToSpawn; i++) //spawn as many as needed
			{
                if (dropItem.m_pickup)
                {
                    SpawnPickup(&returnValue, dropItem.m_pickup);
                }
			}
		}
	}
	return returnValue;
}

TArray<class APickupInteractable*> UItemGeneratorComponent::Generate(int a_amount)
{
	TArray<class APickupInteractable*> returnValue;
	float totalWeight = 0;
	for (FDropDefinitionStruct dropItem : m_dropTable) //sum all the weights in the drop table
	{
		totalWeight += dropItem.m_weight;
	}
	if (m_dropTable.Num() > 0) //if there are items in the drop table
	{
		for (int i =0; i<a_amount; i++) //for as many items as required to spawn
		{
			float randValue = FMath::RandRange(0.0f, totalWeight);; //generate a random value
			float countValue = 0;
			for (FDropDefinitionStruct dropItem : m_dropTable) //for each item in the drop table
			{
				if (randValue > countValue && randValue <= countValue + dropItem.m_weight) //spawn the item if the random value is in the threshold
				{
                    if (dropItem.m_pickup)
                    {
                        SpawnPickup(&returnValue, dropItem.m_pickup);
						break;
                    }
				}
				else
				{
					countValue += dropItem.m_weight; //add to the count for the next loop
				}
			}
		}
	}
	return returnValue;
}

void UItemGeneratorComponent::SpawnPickup(TArray<class APickupInteractable*>* a_outArray, TSubclassOf<APickupInteractable> a_pickup)
{
	AWeaponPickup* weapon = nullptr;
	//APickupInteractable* pickup_var = nullptr;

	FActorSpawnParameters pickupSpawnParams;
	APickupInteractable* pickup_var = GetWorld()->SpawnActor<APickupInteractable>(a_pickup, GetOwner()->GetActorLocation(), FRotator(0, 0, 0), pickupSpawnParams);
	//APickupInteractable* pickup = GetWorld()->SpawnActor<APickupInteractable>(a_pickup, GetOwner()->GetActorLocation(), FRotator(0, 0, 0), pickupSpawnParams);
	weapon = Cast<AMainGameMode>(GetWorld()->GetAuthGameMode())->GetWeaponFromPool(Cast<AWeaponPickup>(pickup_var)->GetWeaponRarity());

	if (weapon != nullptr)
	{
		if (GetOwnerRole() == ROLE_Authority)
		{
			//switch
				//check rarity of pickup
				//pull from specific rarity pool

			//destroy token
			pickup_var->Destroy();

			//AMainGameMode* CurrentGameMode = Cast<AMainGameMode>(GetWorld()->GetAuthGameMode());
			//weapon = CurrentGameMode->GetWeaponFromPool();
			weapon->TeleportTo(GetOwner()->GetActorLocation(), FRotator(0, 0, 0));
			Cast<UPrimitiveComponent>(weapon->GetRootComponent())->SetSimulatePhysics(true);

		}
	}
	else
	{
		//pickup_var = GetWorld()->SpawnActor<APickupInteractable>(a_pickup, GetOwner()->GetActorLocation(), FRotator(0, 0, 0), pickupSpawnParams);
		if (Cast<AWeaponPickup>(pickup_var))
		{
			weapon = Cast<AWeaponPickup>(pickup_var);
			if (GetOwnerRole() == ROLE_Authority)
			{
				if (Cast<AMainGameMode>(GetWorld()->GetAuthGameMode())->GetWorldWeaponGenerator())
				{
					Cast<AMainGameMode>(GetWorld()->GetAuthGameMode())->GetWorldWeaponGenerator()->GenerateWeaponFromTemplate(weapon);
				}
				else
				{
					weapon->DestroySelf();
					GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Red, TEXT("A WEAPON WAS DESTROYED DUE TO THE WEAPON GENERATOR BEING NULL"));
				}
			}

		}
	}
	if (weapon)
	{
		a_outArray->Add(weapon);
	}
	else if (pickup_var)
	{
		a_outArray->Add(pickup_var);
	}
	//a_outArray->Add(pickup);
}
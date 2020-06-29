// Fill out your copyright notice in the Description page of Project Settings.

//Header include
#include "PickupInteractable.h"

//Custom Engine Includes
#include "Net/UnrealNetwork.h"
#include "Engine.h"

//Custom Project Includes
#include "EnumMaster/EnumMaster.h"
#include "GameObjects/Players/MyCharacter.h"
#include "GameObjects/Players/PlayerCharacter.h"
#include "Custom_Components/InventoryComponent.h"
#include "GameObjects/Interactables/ChestInteractable.h"

APickupInteractable::APickupInteractable()
{
	//Init components
	m_colliderComponent->SetCollisionProfileName("PickupCollision");

	//Init variables
	m_isUseOnContact = false;
	m_isConsumeOnUse = true;
	m_uses = 1;
	m_pickupType = InventoryTabEnum::ITE_Invalid;

	//Add Cues Related to the MyActor base class
	m_soundCues.Add("Use");
	m_soundCues.Add("Drop");

	//Add partciles Related to the MyActor base class
	m_particleFXs.Add("Use");
	m_particleFXs.Add("Drop");

	//Setup Replication
	bAlwaysRelevant = false;
	NetPriority = 2;
}

void APickupInteractable::BeginPlay()
{
	Super::BeginPlay();
}

bool APickupInteractable::Interact(AActor* a_actor)
{
	if (Super::Interact(a_actor)) //if you can't be highlited don't call your functions too
	{
		return AddSelfToInventory(a_actor);
	}
	else
	{
		return false;
	}
}

bool APickupInteractable::Use(AActor* a_actor)
{
	PlaySound("Use", true); //could be played in the object or on the world, makes no difference
	PlayParticle("Use"); //could be played in the object or on the world, makes no difference
	return false;
}

void APickupInteractable::Drop(AActor* a_actor)
{
	PlaySound("Drop", true); //could be played in the object or on the world, makes no difference
	PlayParticle("Drop"); //could be played in the object or on the world, makes no difference

	APlayerCharacter* player = Cast<APlayerCharacter>(a_actor);
	if (player) //player characters should call the networked version of this function also so the item is removed on both (or all) sides
	{
		//Get the players net index and call NET_Drop or CLIENT_Drop passing in the net index
	}

	NET_Activate();
}

bool APickupInteractable::AddSelfToInventory(AActor* a_actor)
{
	//check if the interacting actor is a character (player or enemy) or a chest
	AMyCharacter* character = Cast<AMyCharacter>(a_actor);
	if (character)
	{
		if (character->GetInventoryComponent()->AddPickup(this)) //Try to add yourself to the inventory
		{
			NET_Deactivate(); //deactivate on all clients and server
			return true;
		}
		else
		{
			return false;
		}
	}
	else
	{
		AChestInteractable* chest = Cast<AChestInteractable>(a_actor);
		if (chest)
		{
			if (chest->GetInventoryComponent()->AddPickup(this)) //Try to add yourself to the inventory
			{
				NET_Deactivate(); //deactivate on all clients and server
				return true;
			}
			else
			{
				return false;
			}
		}
		else
		{
			return false;
		}
	}
}

void APickupInteractable::NET_Deactivate_Implementation()
{
	SetIsActiveInteractable(false);
	Cast<UPrimitiveComponent>(RootComponent)->SetSimulatePhysics(false);
	Cast<UPrimitiveComponent>(RootComponent)->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	//m_skeletalMeshComponent->SetSimulatePhysics(false);
	for (USkeletalMeshComponent* mesh : m_registeredMeshes_r)
	{
		if (mesh)
		mesh->SetVisibility(false);
		//mesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}
}

void APickupInteractable::NET_Activate_Implementation()
{
	SetIsActiveInteractable(true);
	Cast<UPrimitiveComponent>(RootComponent)->SetSimulatePhysics(true);
	Cast<UPrimitiveComponent>(RootComponent)->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	//m_skeletalMeshComponent->SetSimulatePhysics(true);
	for (USkeletalMeshComponent* mesh : m_registeredMeshes_r)
	{
		if (mesh)
		mesh->SetVisibility(true);
		//mesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	}
}
// Fill out your copyright notice in the Description page of Project Settings.

//Header include
#include "InventoryComponent.h"

//Custom Engine Includes
#include "Engine.h"
#include "Net/UnrealNetwork.h"
#include "Components/ArrowComponent.h"

//Custom Project Includes
#include "EnumMaster/EnumMaster.h"
#include "GameObjects/Interactables/Pickups/PickupInteractable.h"

UInventoryComponent::UInventoryComponent()
{
	//Initialize components
	m_arrowComponent = CreateDefaultSubobject<UArrowComponent>("Drop Direction");

	//Initialize variables
	for (int i = 0; i< (int)InventoryTabEnum::ITE_Invalid; i++)
	{
		m_inventoryItems_r.Add(FTabEnvelope());
	}

	m_exitButtonAngle = 120; //TEMPORARY, should be initialized based on the actual angle of the exit button 
	m_relativeMouseDistanceForActivation = 30; //TEMPORARY, should be initialized based on the size of the inventory wheel
}

void UInventoryComponent::BeginPlay()
{
	Super::BeginPlay();

	m_arrowComponent->AttachToComponent(GetOwner()->GetRootComponent(), FAttachmentTransformRules::SnapToTargetNotIncludingScale);
}

void UInventoryComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (m_isDisplayingInventory_r && GetOwner()->GetLocalRole() == ROLE_Authority) //should only be done in the server since the variables are replicated
	{
		////TEST
		//GEngine->AddOnScreenDebugMessage(1, 1.f, FColor::Red, FString::Printf(TEXT("RelativeVectorIs is: %f, %f, %f"), m_relativeMouseDirection_r.X, m_relativeMouseDirection_r.Y, m_relativeMouseDirection_r.Z));

		//float dotValue = FVector::DotProduct(m_relativeMouseDirection_r.GetSafeNormal(), FVector(0, -1, 0));
		//GEngine->AddOnScreenDebugMessage(2, 1.f, FColor::Red, FString::Printf(TEXT("Dot is: %f"), dotValue));
		//
		//float angle = -(dotValue * 0.5f - 0.5f) * 180.0f; //the angle between the two vectors
		//if (m_relativeMouseDirection_r.X < 0)
		//{
		//	angle = 360.0f - angle; //in order for quadrant 3 and 4 to be between 180 and 360
		//}
		//GEngine->AddOnScreenDebugMessage(3, 1.f, FColor::Red, FString::Printf(TEXT("Angle is: %f"), angle));

		//GEngine->AddOnScreenDebugMessage(4, 1.f, FColor::Red, FString::Printf(TEXT("Tab is: %d"), (int)m_displayedTab_r));

		//if (m_displayedTab_r != InventoryTabEnum::ITE_Invalid)
		//{
		//	if (m_selectedPickup_r >= 0)
		//	{
		//		GEngine->AddOnScreenDebugMessage(5, 1.f, FColor::Red, FString::Printf(TEXT("Pickup is: %d"), m_selectedPickup_r));
		//	}
		//	else
		//	{
		//		GEngine->AddOnScreenDebugMessage(5, 1.f, FColor::Red, TEXT("Pickup is: EXIT"));
		//	}
		//}

		////END TEST

		//Tab selction state
		if (m_displayedTab_r == InventoryTabEnum::ITE_Invalid)
		{
			if (m_relativeMouseDirection_r.SizeSquared() >= m_relativeMouseDistanceForActivation * m_relativeMouseDistanceForActivation) //if the mouse reached the threshold radius
			{
				CalculateTab(); //Calculate which tab is being pointed to
			}
		}
		//Pickup selection state
		else
		{
			CalculateSelected(); //Calculate which pickup is being pointed to, if pointing to the exit set m_selectedPickup_r as -1
			if (m_relativeMouseDirection_r.SizeSquared() <= (m_relativeMouseDistanceForActivation * 0.5) * (m_relativeMouseDistanceForActivation * 0.5)) //if the player returns the mouse to the center 
			{
				//m_displayedTab_r = InventoryTabEnum::ITE_Invalid; //return to the previous state
                m_selectedPickup_r = -1;
			}
		}
	}
}

void UInventoryComponent::MouseInputVertical(float a_val)
{
	SERVER_MouseInputVertical(-a_val); //Needs to be reversed
}

void UInventoryComponent::SERVER_MouseInputVertical_Implementation(float a_val)
{
	m_relativeMouseDirection_r.Y += a_val * m_UIMouseSpeed;
}

void UInventoryComponent::MouseInputHorizontal(float a_val)
{
	SERVER_MouseInputHorizontal(a_val);
}

void UInventoryComponent::SERVER_MouseInputHorizontal_Implementation(float a_val)
{
	m_relativeMouseDirection_r.X += a_val * m_UIMouseSpeed;
}

bool UInventoryComponent::AddPickup(APickupInteractable* a_pickup)
{
	if (a_pickup != m_defaultWeapon)
	{
		m_inventoryItems_r[(int)a_pickup->GetPickupType()].m_tab.Add(a_pickup);
	}
	return true;
}

bool UInventoryComponent::AddPickups(TArray<class APickupInteractable*> a_pickups)
{
	bool returnValue = true;
	for (APickupInteractable* pickup : a_pickups)//for all pickups in a_pickups
	{
		if (!AddPickup(pickup)) returnValue = false; //if you failed to add an item return false
	}
	return returnValue;
}

bool UInventoryComponent::UseSelected(AActor* a_actor)
{
	if (m_isDisplayingInventory_r)//if the inventory is currently displayed
	{
		if (m_displayedTab_r != InventoryTabEnum::ITE_Invalid)//if there is a valid tab selected
		{
			if (m_inventoryItems_r[(int)m_displayedTab_r].m_tab.Num() != 0)//if there are items in this tab
			{
				if (m_inventoryItems_r[(int)m_displayedTab_r].m_tab.IsValidIndex(m_selectedPickup_r))//if there is a valid item selected in that tab
				{
					return m_inventoryItems_r[(int)m_displayedTab_r].m_tab[m_selectedPickup_r]->Use(a_actor);//call use on the pickup and return the result
				}
				else
				{
					return m_inventoryItems_r[(int)m_displayedTab_r].m_tab[m_inventoryItems_r[(int)m_displayedTab_r].m_tab.Num()-1]->Use(a_actor);//return the result of using the last weapon in that tab
				}
			}
			else
			{
				return m_defaultWeapon->Use(GetOwner());// return the result of using the default weapon
			}
		}
	}
	return false;
}

bool UInventoryComponent::UseAt(AActor* a_actor, InventoryTabEnum a_tab, int a_tabIndex)
{
    if (a_tab != InventoryTabEnum::ITE_Invalid)
    {
        if (m_inventoryItems_r[(int)a_tab].m_tab.IsValidIndex(a_tabIndex))
        {
            return m_inventoryItems_r[(int)a_tab].m_tab[a_tabIndex]->Use(a_actor);
        }
    }

    return false;

}

void UInventoryComponent::DropSelected(AActor* a_actor)
{
	if (m_isDisplayingInventory_r)//if the inventory is currently displayed
	{
		if (m_displayedTab_r != InventoryTabEnum::ITE_Invalid)//if there is a valid tab selected
		{
			if (m_inventoryItems_r[(int)m_displayedTab_r].m_tab.Num() != 0)//if there are items in this tab
			{
				if (m_inventoryItems_r[(int)m_displayedTab_r].m_tab.IsValidIndex(m_selectedPickup_r))//if there is a valid item selected in that tab
				{
					APickupInteractable* droppedPickup = m_inventoryItems_r[(int)m_displayedTab_r].m_tab[m_selectedPickup_r];//save the pickup pointer before removing it
					DropPickup(a_actor, droppedPickup);
					CalculateSelected(); //should adjust the selected index to a valid range
				}
			}
		}
	}
}

void UInventoryComponent::DropPickup(AActor* a_actor, APickupInteractable* a_pickup)
{
	m_inventoryItems_r[(int)a_pickup->GetPickupType()].m_tab.Remove(a_pickup);//remove it
	FVector ownerLoc = GetOwner()->GetActorLocation();
	FVector spawnDirection = FMath::VRandCone(m_arrowComponent->GetForwardVector(), PI * m_dropHalfAngle/180.0f);
	FVector offsetFromOwner = spawnDirection * m_dropDistance;
	FVector spawnLocation = ownerLoc + offsetFromOwner;
	a_pickup->Drop(a_actor);//call the drop function so the pickup can do whatever it needs to
	a_pickup->SetActorLocation(spawnLocation);//position the pickup in the world
	Cast<UPrimitiveComponent>(a_pickup->GetRootComponent())->AddForceAtLocation(spawnDirection * m_dropForce, FVector(0.2f, 0.3f,-0.5f));
}

void UInventoryComponent::DisplayInventory(bool a_isDisplayingInventory_r)
{
	if (m_isDisplayingInventory_r && m_selectedPickup_r != -1) //if the inventory was being displayed and the selected pickup is not an invalid index
	{
		UseSelected(GetOwner());
	}
	if (!m_isDisplayingInventory_r) // if the inventory is going to be displayed set the related varibales to the defaults
	{
		m_displayedTab_r = InventoryTabEnum::ITE_Invalid;
		m_selectedPickup_r = -1;
		m_relativeMouseDirection_r = FVector::ZeroVector;
	}
	m_isDisplayingInventory_r = a_isDisplayingInventory_r;
}

bool UInventoryComponent::FlushInventory()
{
	//This probably will change into a delayed repeating delayed function so the items can be dropped in quick succession instead of instanteneously
	bool returnValue = false;
	for (FTabEnvelope tab: m_inventoryItems_r)
	{
		for (APickupInteractable* pickup: tab.m_tab)
		{
			DropPickup(GetOwner(), pickup);
			returnValue = true;
		}
	}
	return returnValue;
}

void UInventoryComponent::EquipAnyWeapon(InventoryTabEnum a_tab)
{
	if (a_tab != InventoryTabEnum::ITE_Invalid)
	{
		if (m_inventoryItems_r[(int)a_tab].m_tab.Num() != 0)
		{
			if (m_inventoryItems_r[(int)a_tab].m_tab[0])
			{
				m_inventoryItems_r[(int)a_tab].m_tab[0]->Use(GetOwner());
				return;
			}
		}
	}

	for (int i = 0; i< (int)InventoryTabEnum::ITE_Invalid; i++) //this should give me all the weapon tabs
	{
		if (m_inventoryItems_r[i].m_tab.Num() != 0)
		{
			if (m_inventoryItems_r[i].m_tab[0])
			{
				m_inventoryItems_r[i].m_tab[0]->Use(GetOwner());
				return;
			}
		}
	}

	m_defaultWeapon->Use(GetOwner());
}

void UInventoryComponent::CalculateTab()
{
	//use the m_relativeMouseDirection to calculate what tab is being pointed to.
	float dotValue = FVector::DotProduct(m_relativeMouseDirection_r.GetSafeNormal(), FVector(0, -1, 0)); //so the reference is the vector down
	float angle = -(dotValue * 0.5f - 0.5f) * 180.0f; //the angle between the two vectors
	if (m_relativeMouseDirection_r.X < 0) 
	{
		angle = 360.0f - angle; //in order for quadrant 3 and 4 to be between 180 and 360
	}
	//convert angle to TabEnum
	m_displayedTab_r = (InventoryTabEnum)((int)(angle / (360.0 / ((int)InventoryTabEnum::ITE_Invalid))));//convension from angle to tab (ASK LUIS IF YOU NEED AN EXPLANATION)
}

void UInventoryComponent::CalculateSelected()
{
	//use the m_relativeMouseDirection to calculate what item is being pointed to.
	float dotValue = FVector::DotProduct(m_relativeMouseDirection_r.GetSafeNormal(), FVector(0, -1, 0)); //so the reference is the vector down
	float angle = -(dotValue * 0.5f - 0.5f) * 180.0f; //the angle between the two vectors
	if (m_relativeMouseDirection_r.X < 0)
	{
		angle = 360.0f - angle; //in order for quadrant 3 and 4 to be between 180 and 360
	}
	//convert angle to selected item
	//if (angle > 360 - m_exitButtonAngle*0.5f && angle < m_exitButtonAngle*0.5f) //if the angle matches the exit button
	//{
	//	m_selectedPickup_r = -1; //set the selected pickup to an invalid value
	//}
	//else //if a valid item is selected
	//{
	//	//convert angle to selected item 
	//	m_selectedPickup_r = ((int)(angle - m_exitButtonAngle*0.5f)) / ((int)((360 - m_exitButtonAngle) / m_inventoryItems_r[(int)m_displayedTab_r].m_tab.Num())); //Conversion from angle to selected pickup (IF YOU NEED AN EXPLANATION AS LUIS)        
	//}
    m_selectedPickup_r = (int)(angle / (360.0f / 5.0f));
}

void UInventoryComponent::GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	DOREPLIFETIME(UInventoryComponent, m_inventoryItems_r)
	DOREPLIFETIME(UInventoryComponent, m_relativeMouseDirection_r)
	DOREPLIFETIME(UInventoryComponent, m_isDisplayingInventory_r)
	DOREPLIFETIME(UInventoryComponent, m_displayedTab_r)
	DOREPLIFETIME(UInventoryComponent, m_selectedPickup_r)
}
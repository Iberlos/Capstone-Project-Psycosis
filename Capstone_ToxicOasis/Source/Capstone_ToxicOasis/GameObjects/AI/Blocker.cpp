// Fill out your copyright notice in the Description page of Project Settings.

//Header include
#include "Blocker.h"

//Custom Engine Includes
#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Net/UnrealNetwork.h"
#include "TimerManager.h"
#include "Engine/EngineTypes.h"

//Custom Project Includes
#include "GameObjects/AI/AISpawner.h"
#include "EnumMaster/EnumMaster.h"
#include "GameObjects/Interactables/ArtifactInteractable.h"
#include "GameObjects/Interactables/InteractableActor.h"

// Sets default values
ABlocker::ABlocker()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	m_blockerType = BlockerType::BT_Exit;

	m_colliderComponent->SetSimulatePhysics(false);

	m_staticMeshComp_r = CreateDefaultSubobject<UStaticMeshComponent>("BlockerMesh");
	m_staticMeshComp_r->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	m_staticMeshComp_r->SetCollisionProfileName("NoCollision");
	m_staticMeshComp_r->SetupAttachment(RootComponent);

	m_arenaHasBeenCleared = false;

	SetReplicates(true);
}

// Called when the game starts or when spawned
void ABlocker::BeginPlay()
{
	Super::BeginPlay();
	//TurnOff();
}

// Called every frame
void ABlocker::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	///*If the artifact has been picked up set the flag to stop updating the blocker*/
	///*if (m_owningSpawner->GetRemainingEnemies() <= 0)
	//{
	//	m_arenaHasBeenCleared = true;
	//	TurnOff();
	//}*/
	//if (m_owningSpawner->GetSpawnedArtifacted() != nullptr)
	//{
	//	if (m_owningSpawner->GetSpawnedArtifacted()->GetIsActiveInteractable() == false && m_owningSpawner->GetSpawnerHasBeenCleared())
	//	{
	//		m_arenaHasBeenCleared = true;
	//		TurnOff();
	//	}
	//}
	///*Update the blocker to turn off or on*/
	//if(!m_arenaHasBeenCleared)
	//	UpdateBlocker();

}

void ABlocker::StartUpdate()
{
	//PrimaryActorTick.bCanEverTick = true;
}

void ABlocker::StopUpdate()
{
	//PrimaryActorTick.bCanEverTick = false;
}

void ABlocker::TurnOn()
{
	SERVER_TurnOn();
}

void ABlocker::TurnOff()
{
	SERVER_TurnOff();
}

void ABlocker::SERVER_TurnOn_Implementation()
{
	HideMesh(false);
	ToggleCollider(true);
}

void ABlocker::SERVER_TurnOff_Implementation()
{
	/*Hide the mesh in game and set the collision Off*/
	HideMesh(true);
	ToggleCollider(false);
}

void ABlocker::HideMesh(bool a_hide)
{
	Net_HideMesh(a_hide);
}

void ABlocker::Net_HideMesh_Implementation(bool a_hide)
{
	m_staticMeshComp_r->SetHiddenInGame(a_hide);
}

void ABlocker::ToggleCollider(bool collisionEnabled)
{
	Net_ToggleCollider(collisionEnabled);
}

void ABlocker::Net_ToggleCollider_Implementation(bool collisionEnabled)
{
	if (collisionEnabled)
	{
		m_colliderComponent->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		m_colliderComponent->SetCollisionProfileName("ActorCollision");
	}
	else if (!collisionEnabled)
	{
		m_colliderComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		m_colliderComponent->SetCollisionProfileName("NoCollision");
	}
}

void ABlocker::UpdateBlocker()
{
	//FTimerHandle timer;
	///*Check which type the blocker is*/
	//switch (m_blockerType)
	//{
	//case BlockerType::BT_Entrance:
	//	/*if the spawner has activate turn on the blocker*/
	//	if (m_owningSpawner != nullptr)
	//	{
	//		if (m_owningSpawner->GetSpawnerHasStarted())
	//			GetWorldTimerManager().SetTimer(timer, this, &ABlocker::TurnOn, 0.5f, false);
	//		else if (!(m_owningSpawner->GetSpawnerHasStarted()))
	//			TurnOff();
	//	}
	//	break;
	//case BlockerType::BT_Exit:
	//	if (m_owningSpawner != nullptr)
	//	{
	//		if (m_owningSpawner->GetPlayerIsInArena())
	//			TurnOn();
	//		else if (!(m_owningSpawner->GetPlayerIsInArena()))
	//			TurnOff();
	//	}
	//	break;
	//default:
	//	break;
	//}
}

void ABlocker::GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ABlocker, m_staticMeshComp_r)
	
}

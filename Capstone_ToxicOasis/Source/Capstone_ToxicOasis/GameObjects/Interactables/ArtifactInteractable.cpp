// Fill out your copyright notice in the Description page of Project Settings.

//Header include
#include "ArtifactInteractable.h"
#include "Engine/Public/TimerManager.h"

//Custom Engine Includes
#include "Components/BoxComponent.h"

//Custom Project Includes

AArtifactInteractable::AArtifactInteractable()
{
	m_colliderComponent->SetSimulatePhysics(false);

	//Add Cues Related to the InteractableActor base class
	m_soundCues.Add("ArtifactLifetime");
	m_soundCues.Add("ArtifactInteracted");

	//Add partciles Related to the InteractableActor base class
	m_particleFXs.Add("ArtifactLifetime");
	m_particleFXs.Add("ArtifactInteracted");
}

void AArtifactInteractable::BeginPlay()
{
	Super::BeginPlay();
	if (GetLocalRole() == ROLE_Authority)
	{
		FTimerHandle t;
		GetWorldTimerManager().SetTimer(t, this, &AArtifactInteractable::InitLifetime, 3.0f, false);
	}
}

void AArtifactInteractable::SERVER_DisableArtifact_Implementation()
{
	NET_DisableArtifact();
}

void AArtifactInteractable::NET_DisableArtifact_Implementation()
{
	SetActorHiddenInGame(true);
	m_colliderComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void AArtifactInteractable::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (m_beamParticleInfo_r.m_isActiveBeam && m_pointedActor)
	{
		m_beamParticleInfo_r.m_endPoint = m_pointedActor->GetActorLocation() - GetActorLocation();
	}
}

bool AArtifactInteractable::Interact(AActor* a_actor)
{
	if (Super::Interact(a_actor))//if you can't be highlited don't call your functions too
	{
		SetIsActiveInteractable(false);
		ed_ArtifactInteractedWith.Broadcast();
		PlaySound("ArtifactInteracted");
		PlayParticle("ArtifactInteracted");
		m_pointedActor = a_actor;
		m_beamParticleInfo_r.EnableBeam(a_actor->GetActorLocation()-GetActorLocation());
		FTimerHandle t;
		GetWorldTimerManager().SetTimer(t, this, &AArtifactInteractable::SERVER_DisableArtifact, 3.0f, false);
		//FTimerHandle t2;
		//GetWorldTimerManager().SetTimer(t, this, &AArtifactInteractable::StopParticle, 3.0f, false);
		return true;
	}
	else
	{
		return false;
	}
}

void AArtifactInteractable::InitLifetime()
{
	PlaySound("ArtifactLifetime");
	PlayParticle("ArtifactLifetime");
}
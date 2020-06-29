// Fill out your copyright notice in the Description page of Project Settings.

//Header include
#include "InteractableActor.h"

//Custom Engine Includes
#include "Net/UnrealNetwork.h"
#include "Components/SkeletalMeshComponent.h"
#include "Engine.h"

//Custom Project Includes

AInteractableActor::AInteractableActor()
{
	m_isActiveInteractable_r = true;

	//Add Cues Related to the InteractableActor base class
	m_soundCues.Add("Interact");

	//Add partciles Related to the InteractableActor base class
	m_particleFXs.Add("Interact");

	//Add Animation Montages Related to the InteractableActor base class
	m_animationMontages.Add("Interact");
}

void AInteractableActor::BeginPlay()
{
	Super::BeginPlay();
	RegisterMesh(m_skeletalMeshComponent);
}

void AInteractableActor::RegisterMesh(USkeletalMeshComponent* a_mesh)
{
	//a_mesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);//not sure if needed
	m_registeredMeshes_r.Add(a_mesh);
	a_mesh->CustomDepthStencilValue = 1;
}

void AInteractableActor::Focus()
{
	if (m_isActiveInteractable_r)
	{
		for (USkeletalMeshComponent* mesh : m_registeredMeshes_r)
		{
			if (mesh)
			{
				mesh->SetRenderCustomDepth(true);
				if (mesh->CustomDepthStencilValue == 0)
				{
					mesh->SetCustomDepthStencilValue(1);
				}
			}
		}
	}
}

void AInteractableActor::Unfocus()
{
	for (USkeletalMeshComponent* mesh : m_registeredMeshes_r)
	{
		if (mesh)
		mesh->SetRenderCustomDepth(false);
	}
}

bool AInteractableActor::Interact(AActor* a_actor)
{
	if (m_isActiveInteractable_r)//if you can't be highlited don't call your functions too
	{
		PlaySound("Interact", true); //might be looping so attached to the object
		PlayParticle("Interact"); //might be looping so attached to the object
		float leght = PlayAnimationMontage("Interact");
		return true;
	}
	else
	{
		return false;
	}
}

void AInteractableActor::GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AInteractableActor, m_registeredMeshes_r)
	DOREPLIFETIME(AInteractableActor, m_isActiveInteractable_r)
}
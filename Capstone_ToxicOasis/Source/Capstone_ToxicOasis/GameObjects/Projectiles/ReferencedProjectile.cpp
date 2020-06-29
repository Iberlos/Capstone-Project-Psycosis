// Fill out your copyright notice in the Description page of Project Settings.

//Header include
#include "ReferencedProjectile.h"

//Custom Engine Includes

//Custom Project Includes
#include "GameObjects/Interactables/Pickups/Weapons/WeaponPickup.h"

AReferencedProjectile::AReferencedProjectile()
{
	//Add Cues Related to the MyActor base class
	m_soundCues.Add("Projectile Action");

	//Add partciles Related to the MyActor base class
	m_particleFXs.Add("Projectile Action");
}

void AReferencedProjectile::BeginPlay()
{
	Super::BeginPlay();
}

void AReferencedProjectile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (m_weaponOfOrigin)
	{
		if (m_weaponOfOrigin->GetIsFiring() == false)
		{
			Action();
		}
	}
	else
	{
		Action(); //just in case the weapon is destroyed while there are still projectiles referencing it have them perform their actions so they are destroyed
	}
}

void AReferencedProjectile::Action()
{
	PlaySound("Projectile Action", true, GetActorLocation()); 
	PlayParticle("Projectile Action", true, GetActorTransform());
	Destroy();
}

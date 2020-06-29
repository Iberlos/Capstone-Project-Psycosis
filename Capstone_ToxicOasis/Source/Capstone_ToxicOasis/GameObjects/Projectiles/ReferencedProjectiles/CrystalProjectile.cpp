// Fill out your copyright notice in the Description page of Project Settings.

//Header include
#include "CrystalProjectile.h"

//Custom Engine Includes
#include "Engine.h"

//Custom Project Includes


ACrystalProjectile::ACrystalProjectile()
{
}

void ACrystalProjectile::BeginPlay()
{
	Super::BeginPlay();
}

void ACrystalProjectile::Action()
{
	Super::Action();

	//explode
	GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Yellow, TEXT("BUM!"));
}

void ACrystalProjectile::ProjectileHit(AActor* a_actor)
{
	PlaySound("Hit", true, GetActorLocation());
	PlayParticle("Hit", true, GetActorTransform());

	//attach to other actor
	AttachToActor(a_actor, FAttachmentTransformRules::KeepRelativeTransform);
}

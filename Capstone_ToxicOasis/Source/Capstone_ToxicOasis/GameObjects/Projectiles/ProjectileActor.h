// Fill out your copyright notice in the Description page of Project Settings.

//Default Unreal Includes
#pragma once

#include "CoreMinimal.h"
#include "GameObjects/MyActor.h"
//Custom Unreal includes #There shouldn't be any includes in the .h file, but if one is needed it goes here.#

//Custom project includes #There shouldn't be any includes in the .h file, but if one is needed it goes here.#

//Generated include #must be included last because unreal complains if there are any includes between it and the generated body thingy.#
#include "ProjectileActor.generated.h"

/*************************************
Class: ProjectileActor
Author: Luis Filipe Moraes
Created: october 2019
Last Modified: 4/feb/2020
Description: This is the base projectile class that was later greatly expanded by Lucas Felix into the PototypeProjectiles. It allows for basic projectile movemnt and effects.
References:
Change Log
**********
Date: unknown
Author: Luis Filipe Moraes
Verified By: Lucas Felix & Abdul Ali
Changes: Added all the functionality needed to move, hit enemies, play particles and sounds when hit and deal damage. It can also be initilized by the weapon so the effects can vary and a pool can be set at some point.
Date: between 20/jan and 4/feb/2020
Author: Luis Filipe Moraes
Verified By: Lucas Felix & Abdul Ali
Changes: modified the class to allow the initialize function to set the collision presets in case friendly fire is active.
*************************************/
UCLASS()
class CAPSTONE_TOXICOASIS_API AProjectileActor : public AMyActor
{
	GENERATED_BODY()

public:
	//Essentials
	AProjectileActor();
	virtual void Tick(float DeltaTime) override;
	//Other Methods
	virtual void Initialize(class AWeaponPickup* a_weapon, FVector a_forwardDirection, float a_damage, bool a_isFreindlyFire);
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
	virtual void BeginPlay() override;
	//Other Methods
	virtual void DealDamage(class AMyCharacter* a_actor);
	virtual void ProjectileHit(AActor* a_actor);
#pragma region
private:
	//...
protected:
#pragma endregion
	//OnEvents
	UFUNCTION()
		virtual void OnHit
		(
			UPrimitiveComponent* HitComp,
			AActor* OtherActor,
			UPrimitiveComponent* OtherComp,
			FVector NormalImpulse,
			const FHitResult& Hit
		);
	//Components
	UPROPERTY(EditDefaultsOnly, Category = "Dev. Data|Projectile Data|Components")
	class UProjectileMovementComponent* m_projectileComponent;
	//Variables
	class AWeaponPickup* m_weaponOfOrigin;
	float m_damage;
	UPROPERTY(EditDefaultsOnly, Category = "Dev. Data|Projectile Data|Variables")
		float m_speed;
};

// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameObjects/Projectiles/ReferencedProjectile.h"
#include "CrystalProjectile.generated.h"

/**
 * 
 */
UCLASS()
class CAPSTONE_TOXICOASIS_API ACrystalProjectile : public AReferencedProjectile
{
	GENERATED_BODY()
	
public:
	//Essentials
	ACrystalProjectile();
	//Other Methods
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
	virtual void Action() override;
	virtual void ProjectileHit(AActor* a_actor) override;
#pragma region
private:
	//...
protected:
#pragma endregion
	//OnEvents
	//Components
	//Variables
};

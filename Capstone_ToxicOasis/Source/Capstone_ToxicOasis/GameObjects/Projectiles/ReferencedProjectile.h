// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameObjects/Projectiles/ProjectileActor.h"
#include "ReferencedProjectile.generated.h"

/**
 * 
 */
UCLASS()
class CAPSTONE_TOXICOASIS_API AReferencedProjectile : public AProjectileActor
{
	GENERATED_BODY()
	
public:
	//Essentials
	AReferencedProjectile();
	//Other Methods
	void Tick(float DeltaTime) override;
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
	virtual void Action();
#pragma region
private:
	//...
protected:
#pragma endregion
	//OnEvents
	//Components
	//Variables
};

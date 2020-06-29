// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameObjects/AI/AIBaseCharacter.h"
#include "AIBird.generated.h"

/*************************************
Class: AIBird
Author: Erin
Created: october 2019
Last Modified: 3/mar/2020
Description: This clas defines the bird enemy AI
References:
Change Log
**********
Date: unknown
Author: Erin and abdul
Verified By: Lucas Felix & Abdul Ali
Changes: Added all the functionlaity related to the base fucntionality of the class.
--------------------------------------------------------
Date: 3/mar/2020
Author: Luis Filipe Moraes
Verified By: Lucas Felix & Abdul Ali
Changes: Added the beam particle and made the bird use it in its attack.
--------------------------------------------------------
Date: 6/mar/2020
Author: Luis Filipe Moraes
Verified By: Lucas Felix & Abdul Ali
Changes: Attached the particle emmiter to the socket, modified the Fire laser function to be able to be timed by the animations. Applied the correct particle effect to the laser and made it bend so it doesnt missalign with the head too much.
--------------------------------------------------------
*************************************/
UCLASS()
class CAPSTONE_TOXICOASIS_API AAIBird : public AAIBaseCharacter
{
	GENERATED_BODY()
public:
    AAIBird();

    virtual void Tick(float DeltaTime) override;

    virtual void MoveToPlayer() override;
    virtual void PrimaryAttack() override;
    virtual void SecondaryAttack() override;

	UFUNCTION(BlueprintCallable)
    void FireLaser();
	void DisableLaser();
#pragma region
private:
    UFUNCTION(Server, Reliable, WithVAlidation)
        void SERVER_FireLaser();
    bool SERVER_FireLaser_Validate() { return true; }
public:
#pragma endregion
   
	UFUNCTION(BlueprintCallable)
		bool GetIsSeeingPlayer() { return m_currentTarget_r != nullptr; }
	UFUNCTION(BlueprintCallable)
		FVector GetLookDirection();
protected:
    virtual void BeginPlay() override;

    UPROPERTY(EditDefaultsOnly, Category = "Attack")
        class UBoxComponent* m_laserSpawnPoint;

    FVector m_start;
    FVector m_end;
    FHitResult m_outHit;
    FCollisionQueryParams m_collisionParams;
};

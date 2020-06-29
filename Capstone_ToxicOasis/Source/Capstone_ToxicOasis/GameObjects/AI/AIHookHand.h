// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameObjects/AI/AIBaseCharacter.h"
#include "AIHookHand.generated.h"

/*************************************
Class: AIBird
Author: Erin
Created: october 2019
Last Modified: 3/mar/2020
Description: This clas defines the hookhand enemy AI
References:
Change Log
**********
Date: unknown
Author: Erin and abdul
Verified By: Lucas Felix & Abdul Ali
Changes: Added all the functionlaity related to the base fucntionality of the class.
--------------------------------------------------------
Date: 6/mar/2020
Author: Luis Filipe Moraes
Verified By: Lucas Felix & Abdul Ali
Changes: Modified the primary attack to take advantage of the inheritance tree.
*************************************/
UCLASS()
class CAPSTONE_TOXICOASIS_API AAIHookHand : public AAIBaseCharacter
{
	GENERATED_BODY()
public:
    AAIHookHand();

    virtual void Tick(float DeltaTime) override;

    virtual void MoveToPlayer() override;
    virtual void PrimaryAttack() override;
    virtual void SecondaryAttack() override;
protected:
    virtual void BeginPlay() override;

    //UPROPERTY(EditDefaultsOnly, Category = "Attack")
    //    class UBoxComponent* m_startBox;

    //UPROPERTY(EditDefaultsOnly, Category = "Attack")
    //    class UBoxComponent* m_endBox;

        UPROPERTY(EditDefaultsOnly, Category = "Attack")
        class USphereComponent* m_attackHitboxPrimary;
};

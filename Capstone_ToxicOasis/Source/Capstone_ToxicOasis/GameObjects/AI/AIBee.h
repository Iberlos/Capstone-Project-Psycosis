// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameObjects/AI/AIBaseCharacter.h"
#include "AIBee.generated.h"

/**
 * 
 */

class USphereComponent;

UCLASS()
class CAPSTONE_TOXICOASIS_API AAIBee : public AAIBaseCharacter
{
	GENERATED_BODY()
public:
    AAIBee();

    virtual void Tick(float DeltaTime) override;

    virtual void MoveToPlayer() override;
    virtual void PrimaryAttack() override;
    virtual void SecondaryAttack() override;
protected:

    virtual void BeginPlay() override;

    // Attack
    UPROPERTY(EditDefaultsOnly, Category = "AI")
        USphereComponent* m_attackHitbox;
    UPROPERTY(EditDefaultsOnly, Category = "AI")
        float m_attackRadius;

};

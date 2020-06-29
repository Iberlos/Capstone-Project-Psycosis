// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameObjects/AI/AIBaseCharacter.h"
#include "AIChonk.generated.h"

/**
 * 
 */
UCLASS()
class CAPSTONE_TOXICOASIS_API AAIChonk : public AAIBaseCharacter
{
	GENERATED_BODY()
public:
    AAIChonk();

    virtual void Tick(float DeltaTime) override;

    virtual void MoveToPlayer() override;
    virtual void PrimaryAttack() override;
    virtual void SecondaryAttack() override;
protected:
    virtual void BeginPlay() override;

    UPROPERTY(EditDefaultsOnly, Category = "Attack")
        class USphereComponent* m_attackHitboxPrimary;
};

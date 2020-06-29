// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Custom_Components/Boss_State_Components/BossStateBaseComponent.h"
#include "BossStateAngerComponent.generated.h"

/**
 * 
 */
class ATargetPoint;

enum AngerAttack
{
    A_ATK_NONE,
    A_ATK_LEFT,
    A_ATK_RIGHT,
    A_ATK_DOUBLE,
};

UCLASS()
class CAPSTONE_TOXICOASIS_API UBossStateAngerComponent : public UBossStateBaseComponent
{
	GENERATED_BODY()
	
public:
    UBossStateAngerComponent();

    virtual void BeginStateLoop() override;

protected:
    
    void DecideAttack();
    void OnChargeEnd();
    void OnDurationTimerEnd();

    void SingleAttack();
    void DoubleAttack();

    void Attack(FVector a_AttackLocation, float a_AttackRadius);

protected:    
    // Timers
    FTimerHandle m_attackTimer;
    float m_attackDuration; // how long each attack will last for

    // Attack
    int m_attackNum; // amount of times the boss will attack before ending the loop
    int m_attackCount; // amount of times the boss has attacked during the current loop

    int m_currentAttack; // decides which attack the boss will perform

    float m_singleAttackRadius;
    float m_doubleAttackRadius;

    UPROPERTY(EditInstanceOnly)
    ATargetPoint* m_leftArmTarget;
    UPROPERTY(EditInstanceOnly)
    ATargetPoint* m_rightArmTarget;

};

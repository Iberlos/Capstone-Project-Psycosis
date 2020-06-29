// Fill out your copyright notice in the Description page of Project Settings.


#include "BossStateAngerComponent.h"
#include "TimerManager.h"

#include "DrawDebugHelpers.h"
#include "WorldCollision.h"
#include "Engine/TargetPoint.h"

#include "GameObjects/Players/PlayerCharacter.h"

UBossStateAngerComponent::UBossStateAngerComponent()
{
    // Set variables
    m_MinDamage = 1.0f;
    m_MaxDamage = 2.0f;

    m_attackDuration = 1.0f;
    m_attackNum = 3;
    m_attackCount = 0;

    m_singleAttackRadius = 400.0f;
    m_doubleAttackRadius = 300.0f;
}

void UBossStateAngerComponent::BeginStateLoop()
{
    DecideAttack();
    // charge up timer
    GetWorld()->GetTimerManager().SetTimer(m_chargeTimer, this, &UBossStateAngerComponent::OnChargeEnd, m_chargeDuration, false);
}

void UBossStateAngerComponent::DecideAttack()
{
    int currentAttack = FMath::RandRange(A_ATK_LEFT, A_ATK_DOUBLE);
    m_currentAttack = currentAttack;
}

void UBossStateAngerComponent::OnChargeEnd()
{
    GetWorld()->GetTimerManager().ClearTimer(m_chargeTimer);

    if (m_currentAttack == A_ATK_LEFT || m_currentAttack == A_ATK_RIGHT)
    {
        GetWorld()->GetTimerManager().SetTimer(m_attackTimer, this, &UBossStateAngerComponent::SingleAttack, m_attackDuration, false);
    }
    else if (m_currentAttack == A_ATK_DOUBLE)
    {
        GetWorld()->GetTimerManager().SetTimer(m_attackTimer, this, &UBossStateAngerComponent::DoubleAttack, m_attackDuration, false);
    }
    ////attack using decided attack
    //switch (m_currentAttack)
    //{
    //case A_ATK_LEFT:
    //    GetWorld()->GetTimerManager().SetTimer(m_attackTimer, this, &UBossStateAngerComponent::SingleAttack, m_attackDuration, false);
    //    break;
    //case A_ATK_RIGHT:
    //    GetWorld()->GetTimerManager().SetTimer(m_attackTimer, this, &UBossStateAngerComponent::SingleAttack, m_attackDuration, false);
    //    break;
    //default:
    //    break;
    //}
}

void UBossStateAngerComponent::OnDurationTimerEnd()
{
    if (m_attackCount >= m_attackNum)
    {
        m_attackCount = 0;
        EndStateLoop();
    }
    m_attackCount++;
    BeginStateLoop();
}

void UBossStateAngerComponent::SingleAttack()
{
    GetWorld()->GetTimerManager().ClearTimer(m_attackTimer);

    if (m_currentAttack == A_ATK_LEFT)
    {
        //set location of burst to left'
        if (m_leftArmTarget)
        {
            Attack(m_leftArmTarget->GetTargetLocation(), m_singleAttackRadius);
        }
    }
    else if (m_currentAttack == A_ATK_RIGHT)
    {
        if (m_rightArmTarget)
        {
            Attack(m_rightArmTarget->GetTargetLocation(), m_singleAttackRadius);
        }
    }


    OnDurationTimerEnd();
}

void UBossStateAngerComponent::DoubleAttack()
{
    GetWorld()->GetTimerManager().ClearTimer(m_attackTimer);
    Attack(m_leftArmTarget->GetTargetLocation(), m_doubleAttackRadius);
    Attack(m_rightArmTarget->GetTargetLocation(), m_doubleAttackRadius);

    OnDurationTimerEnd();
}

void UBossStateAngerComponent::Attack(FVector a_AttackLocation, float a_AttackRadius)
{
    FVector Start = a_AttackLocation;
    FVector End = a_AttackLocation;

    // do burst
    TArray<FHitResult> OutHits;

    // create a collision sphere
    FCollisionShape CollisionSphere = FCollisionShape::MakeSphere(a_AttackRadius);

    // draw collision sphere
    DrawDebugSphere(GetWorld(), Start, CollisionSphere.GetSphereRadius(), 30, FColor::Red, false, 1.0f);

    // check if something got hit in the sweep
    if (GetWorld()->SweepMultiByChannel(OutHits, Start, End, FQuat::Identity, ECC_WorldStatic, CollisionSphere))
    {
        for (auto& Hit : OutHits)
        {
            if (APlayerCharacter* player = Cast<APlayerCharacter>(Hit.GetActor()))
            {
                float dmg = FMath::RandRange(m_MinDamage, m_MaxDamage);
                player->ApplyDamage(dmg);
            }
        }
    }
}

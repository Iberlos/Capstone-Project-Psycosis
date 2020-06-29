// Fill out your copyright notice in the Description page of Project Settings.


#include "BossStateBargainComponent.h"
#include "Components/BoxComponent.h"
#include "GameFramework/Actor.h"
#include "TimerManager.h"
#include "Engine/EngineTypes.h"
//#include "Engine/World.h"
#include "GameObjects/Players/PlayerCharacter.h"
#include "GameObjects/AI/Boss/BossPawn.h"

UBossStateBargainComponent::UBossStateBargainComponent()
{
#pragma region Setup "Left Arm Hitbox"
    // Setup "Left Arm Hitbox"
    m_HitboxLeftArm = CreateDefaultSubobject<UBoxComponent>("Left Arm Hitbox");
    m_HitboxLeftArm->SetBoxExtent(FVector(10.0f, 10.0f, 10.0f));
    // Collision
    m_HitboxLeftArm->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
    m_HitboxLeftArm->SetCollisionProfileName("OverlapAll");
    // Overlap Event
    m_HitboxLeftArm->OnComponentBeginOverlap.AddDynamic(this, &UBossStateBargainComponent::OnOverlapBegin);
    // Transform and size
    m_HitboxLeftArm->SetBoxExtent(FVector(440.0f, 100.0f, 40.0f));

#pragma endregion

#pragma region Setup "Right Arm Hitbox"
    // Setup "Right Arm Hitbox"
    m_HitboxRightArm = CreateDefaultSubobject<UBoxComponent>("Right Arm Hitbox");
    m_HitboxRightArm->SetBoxExtent(FVector(10.0f, 10.0f, 10.0f));
    // Collision
    m_HitboxRightArm->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
    m_HitboxRightArm->SetCollisionProfileName("OverlapAll");
    // Overlap Event
    m_HitboxRightArm->OnComponentBeginOverlap.AddDynamic(this, &UBossStateBargainComponent::OnOverlapBegin);
    // Transform and Size
    m_HitboxRightArm->SetBoxExtent(FVector(440.0f, 100.0f, 40.0f));

#pragma endregion

    // Set variables
    m_MinDamage = 30.0f;
    m_MaxDamage = 50.0f;

    m_attackDuration = 10.0f;

    m_bCanDamagePlayer = false;
}

void UBossStateBargainComponent::BeginStateLoop()
{
    //beginplay
    //begin wind-up
    GetWorld()->GetTimerManager().SetTimer(m_chargeTimer, this, &UBossStateBargainComponent::OnChargeEnd, m_chargeDuration, false);
}

void UBossStateBargainComponent::OnChargeEnd()
{
    m_bCanDamagePlayer = true;

    GetWorld()->GetTimerManager().ClearTimer(m_chargeTimer);

    //set attack duration timer
    GetWorld()->GetTimerManager().SetTimer(m_attackTimer, this, &UBossStateBargainComponent::OnDurationTimerEnd, m_attackDuration, false);
}

void UBossStateBargainComponent::OnDurationTimerEnd()
{
    GetWorld()->GetTimerManager().ClearTimer(m_attackTimer);
    m_bCanDamagePlayer = false;
    EndStateLoop();
}

void UBossStateBargainComponent::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
    // on overlap
    if (m_bCanDamagePlayer)
    {
        if (OtherActor)
        {
            if (APlayerCharacter* player = Cast<APlayerCharacter>(OtherActor))
            {
                int randRangeDmg = FMath::RandRange(m_MinDamage, m_MaxDamage);
                player->ApplyDamage(randRangeDmg);

                // TODO push player back?
            }
        }
    }
}



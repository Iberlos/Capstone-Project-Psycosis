// Fill out your copyright notice in the Description page of Project Settings.


#include "AIChonk.h"
// Components
#include "Components/SphereComponent.h"
// Ours
#include "Custom_Components/ObjectHealthComponent.h"
#include "GameObjects/AI/AICharacterController.h"
#include "GameObjects/Players/PlayerCharacter.h"

AAIChonk::AAIChonk()
{
    m_attackHitboxPrimary = CreateDefaultSubobject<USphereComponent>("Primary Attack Hitbox");
    m_attackHitboxPrimary->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
    m_attackHitboxPrimary->SetCollisionProfileName("OverlapAll");
    m_attackHitboxPrimary->SetSphereRadius(64.0f);
    m_attackHitboxPrimary->SetRelativeLocation(FVector(50.0f, 0.0f, 0.0f));
    m_attackHitboxPrimary->SetupAttachment(RootComponent);

    GetHealthComponent()->SetMaxHealth(100.0f);
    GetHealthComponent()->SetHealth(100.0f);
}

void AAIChonk::BeginPlay()
{
    Super::BeginPlay();
}

void AAIChonk::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
}

void AAIChonk::MoveToPlayer()
{
    if (m_currentTarget_r == nullptr)
        return;

    if (m_AIController)
    {
        FVector PlayerLocation = m_currentTarget_r->GetActorLocation();
        m_AIController->MoveToLocation(PlayerLocation, 80.0f, true, true, false);
    }

}

void AAIChonk::PrimaryAttack()
{
    // Setup overlap check parameters
    TArray<AActor*> overlappingActors;
    TSubclassOf<APlayerCharacter> ClassFilter;

    // Get all Player Characters overlapping the Attack Hitbox
    m_attackHitboxPrimary->GetOverlappingActors(overlappingActors, ClassFilter);

    // For each overlapping Character
    for (auto overlappingCharacter : overlappingActors)
    {
        // If the overlapping character is a Player Character
        if (APlayerCharacter * overlappingPlayer = Cast<APlayerCharacter>(overlappingCharacter))
        {
            // If the overlapping character is not null, attack it
            if (overlappingCharacter)
            {
                overlappingPlayer->ApplyDamage(m_pAttackStrength);
            }
        }
    }
}

void AAIChonk::SecondaryAttack()
{
}



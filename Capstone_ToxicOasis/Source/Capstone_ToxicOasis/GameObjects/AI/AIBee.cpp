// Fill out your copyright notice in the Description page of Project Settings.


#include "AIBee.h"

#include "Components/SphereComponent.h"

#include "Custom_Components/ObjectHealthComponent.h"
#include "GameObjects/Players/PlayerCharacter.h"
#include "GameObjects/AI/AICharacterController.h"

AAIBee::AAIBee()
{
    // Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
    PrimaryActorTick.bCanEverTick = true;

    // Setup the attack sphere hitbox
    m_attackHitbox = CreateDefaultSubobject<USphereComponent>(TEXT("Attack Radius"));
    m_attackHitbox->SetSphereRadius(m_attackRadius);
    // Collision
    m_attackHitbox->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
    m_attackHitbox->SetCollisionProfileName("OverlapAll");
    // Attach to root
    m_attackHitbox->SetupAttachment(RootComponent);

    GetHealthComponent()->SetMaxHealth(100.0f);
    GetHealthComponent()->SetHealth(100.0f);
  
    // Setup other variables
    m_attackRadius = 32.0f;
}

void AAIBee::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

}

void AAIBee::MoveToPlayer()
{
    if (m_currentTarget_r == nullptr)
        return;
    AAICharacterController* controller = Cast<AAICharacterController>(GetController());
    if (controller)
    {
		GetCharacterMovement()->MaxWalkSpeed = 200.0f;
        FVector PlayerLocation = m_currentTarget_r->GetActorLocation();
        controller->MoveToLocation(PlayerLocation, 80.0f, true, true, false);
    }
}

void AAIBee::PrimaryAttack()
{
    // Setup overlap check parameters
    TArray<AActor*> overlappingActors;
    TSubclassOf<APlayerCharacter> ClassFilter;

    // Get all Player Characters overlapping the Attack Hitbox
    m_attackHitbox->GetOverlappingActors(overlappingActors, ClassFilter);

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

void AAIBee::SecondaryAttack()
{
}

void AAIBee::BeginPlay()
{
    Super::BeginPlay();
}

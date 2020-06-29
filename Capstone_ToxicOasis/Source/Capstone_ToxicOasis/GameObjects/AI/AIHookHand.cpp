// Fill out your copyright notice in the Description page of Project Settings.


#include "AIHookHand.h"
#include "AICharacterController.h"

#include "GameObjects/Players/PlayerCharacter.h"
#include "Custom_Components/ObjectHealthComponent.h"

#include "GameFramework/Pawn.h"
//#include "Components/BoxComponent.h"
#include "Components/SphereComponent.h"
#include "ConstructorHelpers.h"
#include "DrawDebugHelpers.h"

AAIHookHand::AAIHookHand()
{
    //m_startBox = CreateDefaultSubobject<UBoxComponent>("Start Point");
    //m_startBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    //m_startBox->SetCollisionProfileName("NoCollision");
    //m_startBox->SetupAttachment(RootComponent);

    //m_endBox = CreateDefaultSubobject<UBoxComponent>("End Point");
    //m_endBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    //m_endBox->SetCollisionProfileName("NoCollision");
    //m_endBox->SetupAttachment(RootComponent);

    m_attackHitboxPrimary = CreateDefaultSubobject<USphereComponent>("Primary Attack Hitbox");
    m_attackHitboxPrimary->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
    m_attackHitboxPrimary->SetCollisionProfileName("OverlapAll");
    m_attackHitboxPrimary->SetSphereRadius(64.0f);
    m_attackHitboxPrimary->SetRelativeLocation(FVector(50.0f, 0.0f, 0.0f));
    m_attackHitboxPrimary->SetupAttachment(RootComponent);

    GetHealthComponent()->SetMaxHealth(100.0f);
    GetHealthComponent()->SetHealth(100.0f);
}

void AAIHookHand::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
}

void AAIHookHand::MoveToPlayer()
{
    if (m_currentTarget_r == nullptr)
        return;

    if (m_AIController)
    {
        FVector PlayerLocation = m_currentTarget_r->GetActorLocation();
        m_AIController->MoveToLocation(PlayerLocation, 80.0f, true, true, false);
    }
}

void AAIHookHand::PrimaryAttack()
{
    //FHitResult OutHit;
    //FVector Start = m_startBox->GetComponentLocation();
    //FVector End = m_endBox->GetComponentLocation();
    //FQuat Rot = this->GetActorQuat();
    //FCollisionShape CollisionShape;
    //FCollisionQueryParams Params;
    //FCollisionResponseParams ResponsePara;
    //DrawDebugLine(GetWorld(), Start, End, FColor::Green, false, 1, 0, 5);
    //GetWorld()->SweepSingleByChannel(OutHit, Start, End, Rot, ECollisionChannel::ECC_Pawn, CollisionShape, Params, ResponsePara);

    //// If the hit actor is not null
    //if (OutHit.GetActor())
    //{
    //    // If the hit actor is a player character, damage the player
    //    if (APlayerCharacter * hitPlayer = Cast<APlayerCharacter>(OutHit.GetActor()))
    //    {
    //        hitPlayer->ApplyDamage(m_pAttackStrength);
    //    }
    //}

    // Setup overlap check parameters

	Super::PrimaryAttack();

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

void AAIHookHand::SecondaryAttack()
{
}

void AAIHookHand::BeginPlay()
{
    Super::BeginPlay();
}

// Fill out your copyright notice in the Description page of Project Settings.


#include "AIBird.h"
#include "Components/BoxComponent.h"

#include "GameObjects/Players/PlayerCharacter.h"
#include "Custom_Components/ObjectHealthComponent.h"

#include "ConstructorHelpers.h"
#include "DrawDebugHelpers.h"
#include "TimerManager.h"
#include "Engine/World.h"
#include "EngineUtils.h"

AAIBird::AAIBird()
{
    m_laserSpawnPoint = CreateDefaultSubobject<UBoxComponent>("LaserSpawnPoint");
    m_laserSpawnPoint->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    m_laserSpawnPoint->SetCollisionProfileName("NoCollision");
    m_laserSpawnPoint->SetupAttachment(RootComponent);

    GetHealthComponent()->SetMaxHealth(100.0f);
    GetHealthComponent()->SetHealth(100.0f);

	m_particleFXs.Add("Beam");
}
void AAIBird::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
}

void AAIBird::MoveToPlayer()
{
}

void AAIBird::PrimaryAttack()
{
	Super::PrimaryAttack();
    if (m_currentTarget_r == nullptr)
        return;

    m_start = m_particleSystemComponent_MYCHARACTER->GetComponentLocation();
    m_end = m_currentTarget_r->GetActorLocation();
}

void AAIBird::SecondaryAttack()
{
}

void AAIBird::FireLaser()
{
    SERVER_FireLaser();
}

void AAIBird::DisableLaser()
{
	m_beamParticleInfo_r.DisableBeam();
	StopParticle();
}

void AAIBird::SERVER_FireLaser_Implementation()
{
    //DrawDebugLine(GetWorld(), m_start, m_end, FColor::Green, false, 0.75, 0, 5);
	PlayParticle("Beam");
	m_beamParticleInfo_r.EnableBeam(m_end - m_particleSystemComponent_MYCHARACTER->GetComponentLocation(), m_particleSystemComponent_MYCHARACTER->GetForwardVector());

    m_collisionParams.AddIgnoredActor(this);
    GetWorld()->LineTraceSingleByChannel(m_outHit, m_start, m_end, ECC_Pawn, m_collisionParams);

    // If the hit actor is not null
    if (m_outHit.GetActor())
    {
        // If the hit actor is a player character, damage the player
        if (APlayerCharacter* hitPlayer = Cast<APlayerCharacter>(m_outHit.GetActor()))
        {
            hitPlayer->ApplyDamage(m_pAttackStrength);
        }
    }

	FTimerHandle t;
	GetWorldTimerManager().SetTimer(t, this, &AAIBird::DisableLaser, 0.3f, false);
}

FVector AAIBird::GetLookDirection()
{
	if (m_currentTarget_r)
	{
		return m_currentTarget_r->GetActorLocation() - m_particleSystemComponent_MYCHARACTER->GetComponentLocation();
	}
	return FVector(0, 0, 0);
}

void AAIBird::BeginPlay()
{
    Super::BeginPlay();

	if (GetMesh()->DoesSocketExist("LaserSocket"))
	{
		m_particleSystemComponent_MYCHARACTER->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, "LaserSocket");
	}
}

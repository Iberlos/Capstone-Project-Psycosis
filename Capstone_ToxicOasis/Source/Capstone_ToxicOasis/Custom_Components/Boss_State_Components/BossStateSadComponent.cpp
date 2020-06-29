// Fill out your copyright notice in the Description page of Project Settings.


#include "BossStateSadComponent.h"
#include "Engine/TargetPoint.h"
#include "Components/StaticMeshComponent.h"
#include "Components/BoxComponent.h"
#include "math.h"
#include "Engine/World.h"
#include "Containers/Array.h"
#include "GameObjects/Players/PlayerCharacter.h"
#include "Components/StaticMeshComponent.h"
#include "GameObjects/AI/Boss/BossPawn.h"
#include "TimerManager.h"

UBossStateSadComponent::UBossStateSadComponent()
{
	PrimaryComponentTick.bCanEverTick = true;

	m_waterHitbox = CreateDefaultSubobject<UBoxComponent>("Water Hitbox");
	m_waterHitbox->SetBoxExtent(FVector(400.0f, 400.0f, 40.0f));
	m_waterHitbox->SetHiddenInGame(false); 
    
	m_waterMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Water Mesh"));
	m_waterMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	m_waterMesh->SetCollisionProfileName("NoCollision");
	m_waterMesh->SetupAttachment(m_waterHitbox);

    m_platformsShouldRise = false;
    m_waterShouldRise = false;

	m_waterShouldLower = false;
	m_platformsShouldLower = false;

	m_platformMoveTime = 5.0f;

	m_chargeDuration = 1.6;

	m_waterChargeTime = 2.0f;
	m_waterStayTime = 10.0f;

	m_PlatformWaitTime = 2.0f;

	m_platformHeightOffset = 0.0f;

	m_MinDamage = 10;
	m_MaxDamage = 25;

	m_DamageInterval = 1.0f;
}

void UBossStateSadComponent::BeginStateLoop()
{
	if (m_spawnTargetPoints.Num() > 0)
	{
		m_platformHeightOffset = m_spawnTargetPoints[0]->GetActorLocation().Z;
	}
	//Super::BeginStateLoop();
	if (m_platformArray.Num() > 0)
	{
		m_platformDefaultHeight = m_platformArray[0]->GetActorLocation().Z;
		m_platformTargetHeight = m_platformDefaultHeight + m_platformHeightOffset;
	}
	m_waterDefaultHeight = m_waterHitbox->GetComponentLocation().Z;

	RandomizePlatformPositions();
	GetWorld()->GetTimerManager().SetTimer(m_DamageTimer, this, &UBossStateSadComponent::CheckForPlayers, m_DamageInterval, true);
	m_owningBoss->WaterBeginAnimTransition();
	GetWorld()->GetTimerManager().SetTimer(m_chargeTimer, this, &UBossStateSadComponent::OnChargeEnd, m_chargeDuration, false);
}

void UBossStateSadComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	if (m_platformsShouldRise)
	{
		RisePlatforms(m_platformTargetHeight);
	}
	else if (m_platformsShouldLower)
	{
		RisePlatforms(m_platformDefaultHeight);
	}

	if (m_waterShouldRise)
	{
		if(m_waterTarget)
			RiseWater(m_waterTarget->GetActorLocation().Z);
	}
	else if (m_waterShouldLower)
	{
		RiseWater(m_waterDefaultHeight);
	}
}

void UBossStateSadComponent::OnChargeEnd()
{
	GetWorld()->GetTimerManager().ClearTimer(m_chargeTimer);
	m_owningBoss->WaterIdle();
	m_platformsShouldRise = true;
	//m_waterShouldRise = true;
	//water wait time, water rise true;

	// water rise wait timer
	GetWorld()->GetTimerManager().SetTimer(m_waterChargeTimer, this, &UBossStateSadComponent::WaterShouldRaise, m_waterChargeTime, false);
}

void UBossStateSadComponent::RisePlatforms(float a_target)
{
	if (m_platformArray.Num() > 0)
	{
		for (auto i : m_platformArray)
		{
			FVector platformLocation = i->GetActorLocation();
			FVector TargetLocationZ = FVector::ZeroVector;
			TargetLocationZ.Z = a_target;

			i->SetActorLocation(FMath::VInterpConstantTo(platformLocation, platformLocation + TargetLocationZ, GetWorld()->DeltaTimeSeconds, 100));
		}

		if (m_platformArray.Last()->GetActorLocation().Z >= a_target && m_platformsShouldRise == true)
		{
				m_platformsShouldRise = false;
		}
		if (m_platformArray.Last()->GetActorLocation().Z <= a_target && m_platformsShouldLower == true)
		{
				m_platformsShouldLower = false;
		}
	}
}

void UBossStateSadComponent::RiseWater(float a_target)
{
	FVector WaterLocation = m_waterHitbox->GetRelativeLocation();
	FVector TargetLocationZ = FVector::ZeroVector;
	TargetLocationZ.Z = a_target;

	m_waterHitbox->SetRelativeLocation(FMath::VInterpConstantTo(WaterLocation, WaterLocation + TargetLocationZ, GetWorld()->DeltaTimeSeconds, 100));

	if (m_waterHitbox->GetComponentLocation().Z >= a_target && m_waterShouldRise == true)
	{
		m_waterShouldRise = false;
	}
	if (m_waterHitbox->GetComponentLocation().Z <= a_target && m_waterShouldLower == true)
	{
		m_waterShouldLower = false;
		m_owningBoss->WaterEndAnimTransition();
		GetWorld()->GetTimerManager().SetTimer(m_waterChargeTimer, this, &UBossStateSadComponent::EndWaterStateLoop, 1.9f, false);
	}
}

void UBossStateSadComponent::EndWaterStateLoop()
{
	GetWorld()->GetTimerManager().ClearTimer(m_waterChargeTimer);
	EndStateLoop();
}

void UBossStateSadComponent::WaterShouldRaise()
{
	GetWorld()->GetTimerManager().ClearTimer(m_waterChargeTimer);
	m_waterShouldRise = true;
	GetWorld()->GetTimerManager().SetTimer(m_waterStayTimer, this, &UBossStateSadComponent::LowerWater, m_waterStayTime, false);
}
void UBossStateSadComponent::LowerWater()
{
	GetWorld()->GetTimerManager().ClearTimer(m_waterStayTimer);
	m_waterShouldRise = false;
	m_waterShouldLower = true;

	GetWorld()->GetTimerManager().SetTimer(m_platformWaitTimer, this, &UBossStateSadComponent::LowerPlatforms, m_PlatformWaitTime, false);
}

void UBossStateSadComponent::LowerPlatforms()
{
	GetWorld()->GetTimerManager().ClearTimer(m_platformWaitTimer);

	m_platformsShouldLower = true;
}

void UBossStateSadComponent::CheckForPlayers()
{
	TArray<AActor*> overlappingActors;
	TSubclassOf<APlayerCharacter> classFilter;
	m_waterHitbox->GetOverlappingActors(overlappingActors, classFilter);

	if (overlappingActors.Num() > 0)
	{
		for (auto actor : overlappingActors)
		{
			float randDmg = FMath::RandRange(m_MinDamage, m_MaxDamage);
			if (APlayerCharacter* player = Cast<APlayerCharacter>(actor))
			{
				player->ApplyDamage(randDmg);
			}
		}
	}
}

void UBossStateSadComponent::RandomizePlatformPositions()
{
	PickRandomTarget();
}

void UBossStateSadComponent::PickRandomTarget()
{
	if (m_platformArray.Num() > 0)
	{
		for (int i = 0; i < m_platformArray.Num(); i++)
		{
			FVector currentPlatformPosition = m_platformArray[i]->GetActorLocation();
			int maxTargetPoint = m_spawnTargetPoints.Num() - 1;
			int randTarget = FMath::RandRange(0, maxTargetPoint);

			//get point around target
			FVector2D RandPointInCircle = FMath::RandPointInCircle(180.0f);

			FVector pointToMoveTo = m_spawnTargetPoints[randTarget]->GetActorLocation() + FVector(RandPointInCircle.X, RandPointInCircle.Y, 0.0f);

            m_platformTargetLocs.Add(pointToMoveTo);
			m_platformArray[i]->SetActorLocation(FVector(pointToMoveTo.X, pointToMoveTo.Y, currentPlatformPosition.Z));

			// TODO MOVE ON TIMER??
			//MoveActorTo(m_platformArray[i], m_spawnTargetPoints[randTarget]->GetActorLocation());
		}
		m_platformsShouldRise = true;
	}
}
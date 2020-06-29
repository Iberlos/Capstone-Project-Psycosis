// Fill out your copyright notice in the Description page of Project Settings.


#include "BossStateBaseComponent.h"
#include "GameObjects/AI/Boss/BossPawn.h"

// Sets default values for this component's properties
UBossStateBaseComponent::UBossStateBaseComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	m_MinDamage = 0;
	m_MaxDamage = 10;

	m_chargeDuration = 2.0f;
	m_cooldownDuration = 3.0f;
	// ...
}


// Called when the game starts
void UBossStateBaseComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}


void UBossStateBaseComponent::BeginStateLoop()
{
}

void UBossStateBaseComponent::EndStateLoop()
{
	if (m_owningBoss != nullptr)
	{
		m_owningBoss->EndStateLoop();
	}
}

// Called every frame
void UBossStateBaseComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}


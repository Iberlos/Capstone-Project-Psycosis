// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "BossStateBaseComponent.generated.h"

class ABossPawn;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class CAPSTONE_TOXICOASIS_API UBossStateBaseComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UBossStateBaseComponent();

	virtual void BeginStateLoop();

	virtual void EndStateLoop();

	void SetOwningBoss(ABossPawn* aOwningBoss) { m_owningBoss = aOwningBoss; }
	ABossPawn* GetOwningBoss() { return m_owningBoss; }

protected:
	// Called when the game starts
	virtual void BeginPlay() override;




public:
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

protected:

	ABossPawn* m_owningBoss;

	FTimerHandle m_chargeTimer;
	float m_chargeDuration;

	FTimerHandle m_cooldownTimer;
	float m_cooldownDuration;

	int m_MinDamage;
	int m_MaxDamage;
};

// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Custom_Components/Boss_State_Components/BossStateBaseComponent.h"
#include "BossStateSadComponent.generated.h"

/**
 * 
 */
UCLASS()
class CAPSTONE_TOXICOASIS_API UBossStateSadComponent : public UBossStateBaseComponent
{
	GENERATED_BODY()

public:

	UBossStateSadComponent();

	virtual void BeginStateLoop() override;

    virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

protected:

	void OnChargeEnd();

	void RisePlatforms(float a_target);
	void RiseWater(float a_target);

	void EndWaterStateLoop();

	void RandomizePlatformPositions();
	void PickRandomTarget();

	void LowerWater();
	void LowerPlatforms();

	void CheckForPlayers();

	void WaterShouldRaise();

protected:
	UPROPERTY(EditDefaultsOnly)
		class UStaticMeshComponent* m_platformMesh;
	UPROPERTY(EditInstanceOnly)
	TArray<class ATargetPoint*> m_spawnTargetPoints;


	UPROPERTY(EditDefaultsOnly)
		class UBoxComponent* m_waterHitbox;
	UPROPERTY(EditAnywhere)
		UStaticMeshComponent* m_waterMesh;


	UPROPERTY(EditInstanceOnly)
		ATargetPoint* m_waterTarget;

	// Need to create a platform class to keep meshes in :(
	UPROPERTY(EditInstanceOnly)
	TArray<AActor*> m_platformArray;

    TArray<FVector> m_platformTargetLocs; // Target locations for the platforms to move to

	UPROPERTY(EditDefaultsOnly)
		int m_amountOfPlatforms;

	float m_platformMoveTime;

	FTimerHandle m_waterChargeTimer;
	float m_waterChargeTime;
	FTimerHandle m_waterStayTimer;
	float m_waterStayTime;

	FTimerHandle m_PlatformRiseTimer;
	float m_PlatformRiseTime;

	FTimerHandle m_DamageTimer;
	float m_DamageInterval;

	FTimerHandle m_platformWaitTimer;
	float m_PlatformWaitTime;

	//FTimerHandle m_platformChargeTimer; // How long to wait after the attack starts until the platforms start to rise
	//FTimerHandle m_cooldownTimer;		// Timer set after the attack finishes

    bool m_waterShouldRise;
	bool m_waterShouldLower;

	float m_waterDefaultHeight;

    bool m_platformsShouldRise;
	bool m_platformsShouldLower;

	float m_platformHeightOffset;
	float m_platformTargetHeight; // highest point the targets will reach when they go up
	float m_platformDefaultHeight; // height the platforms start at and return to
};

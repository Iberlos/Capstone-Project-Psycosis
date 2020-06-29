// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Custom_Components/Boss_State_Components/BossStateBaseComponent.h"
#include "BossStateBargainComponent.generated.h"

class UBoxComponent;

/**
 * 
 */
UCLASS()
class CAPSTONE_TOXICOASIS_API UBossStateBargainComponent : public UBossStateBaseComponent
{
	GENERATED_BODY()

public:
        UBossStateBargainComponent();

		virtual void BeginStateLoop() override;

private:
    void OnChargeEnd();
    void OnDurationTimerEnd();

    UFUNCTION()
        void OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

    UPROPERTY(EditDefaultsOnly)
        UBoxComponent* m_HitboxLeftArm;
    UPROPERTY(EditAnywhere)
        UBoxComponent* m_HitboxRightArm;

    bool m_bCanDamagePlayer;

    // Timers
    FTimerHandle m_attackTimer;
    float m_attackDuration;

};

// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/SkeletalMeshActor.h"
#include "EnumMaster/EnumMaster.h"
#include "WeaponPartActor.generated.h"

/**
 * 
 */
UCLASS()
class CAPSTONE_TOXICOASIS_API AWeaponPartActor : public ASkeletalMeshActor
{
    GENERATED_BODY()

public:
    //Essentials
    AWeaponPartActor();
    //Other Methods
    void SetupWeaponAttributeArray();
#pragma region
private:
    //...
public:
#pragma endregion
    //OnEvents
    //Components
    //Variables
    //Getters & Setters
    TArray<float> GetWeaponAttributeArray() { return m_weaponAttributeArray; }
    //CustomStartEffects GetProjectileStartEffect() { return m_startEffect; }
    //CustomInFlightEffects GetProjectileInFlightEffect() { return m_inFlightEffect; }
    //CustomOnHitEffects GetProjectileOnHitEffect() { return m_onHitEffect; }

protected:
    //Essentials
    //Other Methods
#pragma region
private:
protected:
#pragma endregion
    //OnEvents
    //Components
    //Variables
    UPROPERTY(Replicated)
        TArray<float> m_weaponAttributeArray;

    UPROPERTY(Replicated, EditDefaultsOnly, Category = "PartAttributes | Stats"/*, meta = (ClampMin = "0.0", ClampMax = "1000.0")*/)
        float m_partDamage = 0;
    UPROPERTY(Replicated, EditDefaultsOnly, Category = "PartAttributes | Stats"/*, meta = (ClampMin = "1.0", ClampMax = "100.0")*/)
        float m_partAccuracy = 0;
    UPROPERTY(Replicated, EditDefaultsOnly, Category = "PartAttributes | Stats"/*, meta = (ClampMin = "5.0", ClampMax = "3000.0")*/)
        float m_partFireRate = 0;
    UPROPERTY(Replicated, EditDefaultsOnly, Category = "PartAttributes | Stats"/*, meta = (ClampMin = "0.0", ClampMax = "100.0")*/)
        float m_partRecoil = 0;
    UPROPERTY(Replicated, EditDefaultsOnly, Category = "PartAttributes | Stats"/*, meta = (ClampMin = "0.0", ClampMax = "100.0")*/)
        float m_partMagSize = 0;
    UPROPERTY(Replicated, EditDefaultsOnly, Category = "PartAttributes | Stats"/*, meta = (ClampMin = "1.0", ClampMax = "10.0")*/)
        float m_partWeight = 0;

    //UPROPERTY(Replicated, EditDefaultsOnly, Category = "PartAttributes | Projectile Start Effect")
    //    CustomStartEffects m_startEffect = CustomStartEffects::CSE_Default;
    //    //ProjectileStartEffects m_startEffect = ProjectileStartEffects::PSE_Invalid;

    //UPROPERTY(Replicated, EditDefaultsOnly, Category = "PartAttributes | Projectile In Flight Effect")
    //    CustomInFlightEffects m_inFlightEffect = CustomInFlightEffects::CIFE_Default;
    //    //ProjectileInFlightEffects m_inFlightEffect = ProjectileInFlightEffects::PIFE_Invalid;

    //UPROPERTY(Replicated, EditDefaultsOnly, Category = "PartAttributes | Projectile On Hit Effect")
    //    //ProjectileOnHitEffects m_onHitEffect = ProjectileOnHitEffects::POHE_Test01;
    //    CustomOnHitEffects m_onHitEffect = CustomOnHitEffects::COHE_Default;
};
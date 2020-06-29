// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameObjects/Projectiles/ProjectileActor.h"
#include "GameObjects/Projectiles/PrototypeProjectileEffect/ProjectileHomingTargetActor.h"
#include "GameObjects/Interactables/Pickups/Weapons/WeaponPickup.h"
#include "PrototypeProjectile.generated.h"

/**
 * 
 */

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FBalloonProjectileFire);

UCLASS()
class CAPSTONE_TOXICOASIS_API APrototypeProjectile : public AProjectileActor
{
    GENERATED_BODY()

      
public:
	//FRIEND DECLARATION AVOID DECLARING A WHOLE CLASS AS A FRIEND. USE THE FRIEND FUNCTION DECLARATION INSTEAD.
	friend void AWeaponPickup::SetProjectileParticlesAndSound(APrototypeProjectile*);

    //APrototypeProjectile();
    APrototypeProjectile();

    virtual void Initialize(class AWeaponPickup* a_weapon, FVector a_forwardDirection, float a_damage, bool a_isFreindlyFire) override;

    void SetUtilizePrimaryEffect(bool ShouldUseEffectAgain) { m_UtilizePrimaryEffectAgain = ShouldUseEffectAgain; }
    void SetProjectileSpeed(float NewSpeed) { m_speed = NewSpeed; }

    void SetProjectileAmmoType(int AmmoType);
    
    //Release 3
    void SetEnableSpawnEffect(bool enabled) { enableSpawnEffect = enabled; }
    void SetEnableFlightEffect(bool enabled) { enableInFlightEffect = enabled; }
    void SetEnableHitEffect(bool enabled) { enableHitEffect = enabled; }

    bool IsSpawnEffectEnabled() { return enableSpawnEffect; }
    bool IsFlightEffectEnabled() { return enableInFlightEffect; }
    bool IsHitEffectEnabled() { return enableHitEffect; }

    void SetSpawnEffect(CustomStartEffects startEff) { m_startEffect = startEff; }
    void SetInFlightEffect(CustomInFlightEffects flightEff) { m_inFlightEffect = flightEff; }
    void SetHitEffect(CustomOnHitEffects hitEff) { m_onHitEffect = hitEff; }

    void ClearEffects();
    UFUNCTION(Server, Reliable, WithValidation)
        void SERVER_EnableBullet();
    bool SERVER_EnableBullet_Validate() { return true; };
    UFUNCTION(Server, Reliable, WithValidation)
        void SERVER_DisableBullet();
    bool SERVER_DisableBullet_Validate() { return true; };
    //---------------------------------------

private:
    bool m_UtilizePrimaryEffectAgain = true;

protected:

    friend class AWeaponEffectActor;

    AActor* m_ObjectThatWasHitByThisProjectile;

	bool m_isFriendlyFire;

    //Added in Release 3
    //Release 3
#pragma region
    FVector DirForward;
    FVector DirRight;
    FVector DirUp;
    int m_projAmmoType = 0;
    float m_activeTime = 0;
    float m_explosiveScale = 1.0f;
    bool m_CrystalMarkedForDestruction = false;
    bool m_CrystalHasHit = false;
    FTimerHandle ScaleTimer;
    FTimerHandle TimerHandle;
    FTimerHandle RadialZapTimer;                // Used as timer to set off radial zap effect
    FTimerHandle CountdownRef;
    UPROPERTY(Replicated)
        float scalingAmount = 1.2f;
    UPROPERTY(Replicated)
        int trailingAmount = 30;

    UPROPERTY(Replicated)
        bool enableSpawnEffect = false;
    UPROPERTY(Replicated)
        bool enableInFlightEffect = false;
    UPROPERTY(Replicated)
        bool enableHitEffect = false;
    UPROPERTY(Replicated)
        FVector startingPos;

    CustomStartEffects m_startEffect = CustomStartEffects::CSE_NumberOfEffects;
    CustomInFlightEffects m_inFlightEffect = CustomInFlightEffects::CIFE_NumberOfEffects;
    CustomOnHitEffects m_onHitEffect = CustomOnHitEffects::COHE_NumberOfEffects;

    void ClearTimers();

    void BalloonProjectileEffect();
    void CrystalProjectileEffect();
    void LaserProjectileEffect();
    void ShockwaveProjectileEffect();

    void NormalHitEffect(APrototypeProjectile* CurrentProj, AActor* ObjectHit);
    void BeamHitEffect(APrototypeProjectile* CurrentProj, AActor* ObjectHit);
    void StickHitEffect(APrototypeProjectile* CurrentProj, AActor* ObjectHit);
    void ExplosiveHitEffect(APrototypeProjectile* CurrentProj, AActor* ObjectHit);
    void ShockwaveHitEffect(APrototypeProjectile* CurrentProj, AActor* ObjectHit);

    void StartEffectMultiShot();
    void FlightEffectRadialZap();
    void HitEffectSplit();

    void ProjectileRotationAdjustment(APrototypeProjectile* ProjToRotate, int AngleToRotate, FVector AxisToRotateOn);
#pragma endregion
    UPROPERTY(EditDefaultsOnly, Category = Resources, Replicated)
        TSubclassOf<class AProjectileHomingTargetActor> m_projTarget;

    AActor* m_targetToFollow;

    virtual void BeginPlay() override;
    UFUNCTION(Category = "Weapon_Trigger_Release")
        void OnTriggerRelease(AWeaponPickup* OwningWeapon);

    virtual void DealDamage(class AMyCharacter* a_actor) override;
    virtual void ProjectileHit(AActor* a_actor) override;
    virtual void Tick(float DeltaTime) override;
    virtual void OnHit(UPrimitiveComponent* HitComp,AActor* OtherActor,UPrimitiveComponent* OtherComp,FVector NormalImpulse,const FHitResult& Hit) override;
	
    UPROPERTY(EditDefaultsOnly, Category = "Crystal Mesh", Replicated)
        class USkeletalMesh* MeshCrystal;
    UPROPERTY(EditDefaultsOnly, Category = "Balloon Mesh", Replicated)
        class USkeletalMesh* MeshBalloon;

    UFUNCTION(NetMulticast, Reliable, WithValidation)
        void SetAltMeshForProjectile(class USkeletalMesh* AltMesh);
};

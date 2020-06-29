// Fill out your copyright notice in the Description page of Project Settings.


#include "PrototypeProjectile.h"
//#include "GameObjects/Interactables/Pickups/Weapons/WeaponPickup.h"
#include "Engine.h"
#include "EnumMaster/EnumMaster.h"
#include "GameObjects/AI/AIBaseCharacter.h"
#include "GameObjects/Players/PlayerCharacter.h"
//#include "Engine/TargetPoint.h"
#include "GameObjects/Projectiles/PrototypeProjectileEffect/ProjectileHomingTargetActor.h"
#include "Kismet/KismetMathLibrary.h"
#include "UnrealNetwork.h"
#include "GameMode/MainGameMode.h"

APrototypeProjectile::APrototypeProjectile()
{
	m_isFriendlyFire = false;

    //Add Cues Related to the projectile base class	
    m_soundCues.Add("Beam");
    //Add partciles Related to the projectile base class	
    m_particleFXs.Add("Beam");
    m_particleFXs.Add("Explosion"); //Particle Used For projectiles that can explode
}

void APrototypeProjectile::Initialize(AWeaponPickup* a_weapon, FVector a_forwardDirection, float a_damage, bool a_isFreindlyFire)
{
    //Decide which projectile effect to use
    Super::Initialize(a_weapon, a_forwardDirection, a_damage, a_isFreindlyFire);
	m_isFriendlyFire = a_isFreindlyFire;
    //Release 3
    if (GetLocalRole() == ROLE_Authority)
    {
        switch (m_projAmmoType)
        {
        case (int)AmmoTypes::AT_Crystal:
            if (MeshCrystal)
            {
                SetAltMeshForProjectile(MeshCrystal);
            }
            /*m_weaponOfOrigin->ed_TriggerRelease.AddDynamic(this, &APrototypeProjectile::OnTriggerRelease);*/
            CrystalProjectileEffect();
            break;

        case (int)AmmoTypes::AT_Balloon:
            if (MeshBalloon)
            {
                SetAltMeshForProjectile(MeshBalloon);
            }
            BalloonProjectileEffect();
            break;

        case (int)AmmoTypes::AT_Ballistic:
            //Default Projectile Type no effect
            break;
        
        case (int)AmmoTypes::AT_Laser:
            LaserProjectileEffect();
            break;
        case (int)AmmoTypes::AT_ShockWave:
            ShockwaveProjectileEffect();
            break;
        default:
            break;
        }

        if (enableSpawnEffect)
        {
            switch (m_startEffect)
            {
            case CustomStartEffects::CSE_MultiShot:
                StartEffectMultiShot();
                break;
            default:
                break;
            }
        }
        if (enableInFlightEffect)
        {
            switch (m_inFlightEffect)
            {
            case CustomInFlightEffects::CIFE_RadialZap:
                FlightEffectRadialZap();
                break;
            default:
                break;
            }
        }
    }
}

void APrototypeProjectile::SetProjectileAmmoType(int AmmoType)
{
    if (GetLocalRole() == ROLE_Authority)
    {
        m_projAmmoType = AmmoType;
    }
}

void APrototypeProjectile::ClearEffects()
{
    m_startEffect = CustomStartEffects::CSE_NumberOfEffects;
    m_inFlightEffect = CustomInFlightEffects::CIFE_NumberOfEffects;
    m_onHitEffect = CustomOnHitEffects::COHE_NumberOfEffects;
}

void APrototypeProjectile::SERVER_EnableBullet_Implementation()
{
    //m_colliderComponent->SetSimulatePhysics(true);	
    SetActorTickEnabled(true);

    //SetReplicates(true);	
    //SetReplicateMovement(true);	
    //Replication	
   /* m_colliderComponent->SetIsReplicated(true);
    m_skeletalMeshComponent->SetIsReplicated(true);
    m_audioComponent->SetIsReplicated(true);
    m_particleSystemComponent->SetIsReplicated(true);*/
    //ComponentTick	
    /*m_colliderComponent->SetComponentTickEnabled(true);
    m_skeletalMeshComponent->SetComponentTickEnabled(true);
    m_audioComponent->SetComponentTickEnabled(true);
    m_particleSystemComponent->SetComponentTickEnabled(true);*/
}

void APrototypeProjectile::SERVER_DisableBullet_Implementation()
{
    //m_colliderComponent->SetSimulatePhysics(false);	
    SetActorTickEnabled(false);
    
    DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
    GetColliderComponent()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);

    m_projectileComponent->bIsHomingProjectile = false;
    m_projectileComponent->HomingAccelerationMagnitude = 0.f;
    m_projectileComponent->HomingTargetComponent = nullptr;
    m_targetToFollow = nullptr;

    DirForward = FVector(0, 0, 0);
    DirRight = FVector(0, 0, 0);
    DirUp = FVector(0, 0, 0);

    m_skeletalMeshComponent->SetWorldScale3D(FVector(1, 1, 1));
    m_colliderComponent->SetWorldScale3D(FVector(1, 1, 1));

    m_activeTime = 0;
    m_explosiveScale = 1.0f;
    m_CrystalMarkedForDestruction = false;

    scalingAmount = 1.2f;
    trailingAmount = 30;

    //SetReplicates(false);	
    //SetReplicateMovement(false);	
    //Replication	
   /* m_colliderComponent->SetIsReplicated(false);
    m_skeletalMeshComponent->SetIsReplicated(false);
    m_audioComponent->SetIsReplicated(false);
    m_particleSystemComponent->SetIsReplicated(false);*/
    //ComponentTick	
   /* m_colliderComponent->SetComponentTickEnabled(false);
    m_skeletalMeshComponent->SetComponentTickEnabled(false);
    m_audioComponent->SetComponentTickEnabled(false);
    m_particleSystemComponent->SetComponentTickEnabled(false);*/
}

void APrototypeProjectile::BeginPlay()
{
    Super::BeginPlay();
}

void APrototypeProjectile::OnTriggerRelease(AWeaponPickup* OwningWeapon)
{
    //GEngine->AddOnScreenDebugMessage(0, 3.f, FColor::Red, "This Projectile Detected Trigger Release");

    //If the projectile is has Crystal On Hit Modifier, detonate on this Event Trigger
        //-Works in tandem with subscription for the trigger release event as custom detonation of projectiles
    if (GetLocalRole() == ROLE_Authority)
    {
        if (m_weaponOfOrigin->IsValidLowLevel())
        {
            if (m_projAmmoType == (int)AmmoTypes::AT_Crystal)
            {
                if (OwningWeapon == m_weaponOfOrigin)
                {
                    //m_weaponOfOrigin->GetWeaponEffectFunctionalityActor()->HitEffectNormal(this, m_ObjectThatWasHitByThisProjectile);
                    if (m_CrystalHasHit)
                    {
                        ExplosiveHitEffect(this, m_ObjectThatWasHitByThisProjectile);
                    }
                    else
                    {
                        m_CrystalMarkedForDestruction = true;
                    }
                    //ExplosiveHitEffect(this, m_ObjectThatWasHitByThisProjectile);
                }
            }
        }
    }
}

void APrototypeProjectile::DealDamage(AMyCharacter* a_actor)
{
    Super::DealDamage(a_actor);
}

void APrototypeProjectile::ProjectileHit(AActor* a_actor)
{
    Super::ProjectileHit(a_actor);
    //PlaySound("Hit", true, GetActorLocation());
    //PlayParticle("Hit", true, GetActorTransform());
    //Destroy();
    Cast<AMainGameMode>(GetWorld()->GetAuthGameMode())->AddNewBulletToPool(this);
}

void APrototypeProjectile::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    if (GetLocalRole() == ROLE_Authority)
    {
        if (m_weaponOfOrigin)
        {
            if (m_weaponOfOrigin->GetRootComponent())
            {
                if (FVector::DistSquared(GetActorLocation(), m_weaponOfOrigin->GetActorLocation()) > 100000000)
                {
                    Cast<AMainGameMode>(GetWorld()->GetAuthGameMode())->AddNewBulletToPool(this);
                    //Destroy();
                }
            }
        }
    }
}

void APrototypeProjectile::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
    //Super::OnHit(HitComp, OtherActor, OtherComp, NormalImpulse, Hit);

    m_ObjectThatWasHitByThisProjectile = OtherActor;

    if (GetLocalRole() == ROLE_Authority)
    {
        if (m_weaponOfOrigin->GetLocalRole() == ROLE_Authority)
        {

            if (enableHitEffect)
            {
                switch (m_onHitEffect)
                {
                case CustomOnHitEffects::COHE_Split:
                    HitEffectSplit();
                    break;
                default:
                    break;
                }
            }

            switch (m_projAmmoType)
            {
            case (int)AmmoTypes::AT_Crystal:
                //StickHitEffect(thisProjectile, OtherActor);
                StickHitEffect(this, OtherActor);
                break;
            case (int)AmmoTypes::AT_Balloon:
                //ExplosiveHitEffect(thisProjectile, OtherActor);
                ExplosiveHitEffect(this, OtherActor);
                break;
            case (int)AmmoTypes::AT_Ballistic:
                //NormalHitEffect(thisProjectile, OtherActor);
                NormalHitEffect(this, OtherActor);
                break;
            case (int)AmmoTypes::AT_Laser:
                //NormalHitEffect(thisProjectile, OtherActor);
                NormalHitEffect(this, OtherActor);
                break;
            default:
                //NormalHitEffect(thisProjectile, OtherActor);
                NormalHitEffect(this, OtherActor);
                break;
            }
        }
    }
}

void APrototypeProjectile::ClearTimers()
{
    GetWorld()->GetTimerManager().ClearTimer(ScaleTimer);
    GetWorld()->GetTimerManager().ClearTimer(TimerHandle);
    GetWorld()->GetTimerManager().ClearTimer(RadialZapTimer);
    GetWorld()->GetTimerManager().ClearTimer(CountdownRef);
}

void APrototypeProjectile::BalloonProjectileEffect()
{
    AActor* target = GetWorld()->SpawnActor<AActor>(m_projTarget, GetActorLocation() + FVector(0.f, 0.f, 5.f), FRotator(0.f, 0.f, 0.f));
    m_projectileComponent->bIsHomingProjectile = true;
    m_projectileComponent->HomingAccelerationMagnitude = 5000.f;
    m_projectileComponent->HomingTargetComponent = Cast<AProjectileHomingTargetActor>(target)->GetTargetComponent();
    m_targetToFollow = target;

    //DirForward = m_weaponOfOrigin->GetActorForwardVector() * 40;
    DirForward = m_weaponOfOrigin->GetActorForwardVector();
    DirRight = m_weaponOfOrigin->GetActorRightVector();
    DirUp = m_weaponOfOrigin->GetActorUpVector();

    int TotalTime = 6;
    FTimerDelegate ScaleTimerDelegate;
    FTimerDelegate TimerDel;

    TimerDel.BindLambda([&]()
        {
		if (this)
		{
			if (m_targetToFollow)
			{
				int deviation = 12;
				//DirForward += GetActorLocation();
				//FVector DirForward = GetActorForwardVector() * trailingAmount;
				//FVector HorizVariation = GetActorRightVector() * FMath::RandRange(-deviation, deviation);
				FVector ForwardRange = (DirForward * 50) + GetActorLocation();
				//FVector ForwardRange = GetActorForwardVector() * 60;
				FVector HorizVariation = DirRight * FMath::RandRange(-deviation, deviation);
				//FVector VertVariation = GetActorUpVector() * FMath::RandRange(-deviation, deviation);
				FVector VertVariation = DirUp * FMath::RandRange(-deviation, deviation);
				//GEngine->AddOnScreenDebugMessage(0, 0.1f, FColor::Red, "Target Moved!");
				//trailingAmount += 20;

				//m_targetToFollow->SetActorLocation((DirForward + HorizVariation + VertVariation) + GetActorLocation());
				m_targetToFollow->SetActorLocation(ForwardRange + HorizVariation + VertVariation);
				FRotator NewRot = UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), m_targetToFollow->GetActorLocation());
				SetActorRotation(NewRot);
			}
		}
        }
    );

    ScaleTimerDelegate.BindLambda([&]()
        {
            if (this)
            {
                m_activeTime += 0.9f;
                FVector CurrScale = m_skeletalMeshComponent->GetComponentScale();
                FVector NewScale;
                NewScale = CurrScale * scalingAmount;
                m_explosiveScale += 0.8f;
                m_skeletalMeshComponent->SetWorldScale3D(NewScale);
                m_colliderComponent->SetWorldScale3D(NewScale);
                if (m_activeTime > 4)
                {
                    ExplosiveHitEffect(this, nullptr);
                }
            }
        }
    );
    GetWorld()->GetTimerManager().SetTimer(TimerHandle, TimerDel, 0.1f, true);              //Changes balloon trajectory
    GetWorld()->GetTimerManager().SetTimer(ScaleTimer, ScaleTimerDelegate, 0.9f, true);     //Changes ballon size before it pops2
}
void APrototypeProjectile::CrystalProjectileEffect()
{
    m_weaponOfOrigin->ed_TriggerRelease.AddUniqueDynamic(this, &APrototypeProjectile::OnTriggerRelease);
    //NEED TO UNBIND THIS
}
void APrototypeProjectile::LaserProjectileEffect()
{
    m_speed = 0.f;
    m_projectileComponent->SetVelocityInLocalSpace(m_speed * FVector::ForwardVector);

    m_skeletalMeshComponent->SetVisibility(false);
    m_colliderComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    AttachToComponent(m_weaponOfOrigin->GetArrowComponent(), FAttachmentTransformRules::SnapToTargetNotIncludingScale);
    FHitResult laserHit;
    FVector laserStartPoint = GetActorLocation();
    FVector laserFireDir = m_weaponOfOrigin->GetActorForwardVector();
    FVector laserEndPoint = (laserFireDir * 3000.f) + laserStartPoint;
    FCollisionQueryParams CollisionParams;
    CollisionParams.AddIgnoredActor(this);
    CollisionParams.AddIgnoredActor(m_weaponOfOrigin);
    GetWorld()->LineTraceSingleByChannel(laserHit, laserStartPoint, laserEndPoint, ECollisionChannel::ECC_Visibility, CollisionParams);

    PlaySound("Beam", true);
    PlayParticle("Beam", true);
    if (laserHit.GetActor())
    {
        //DrawDebugLine(GetWorld(), laserStartPoint, laserHit.ImpactPoint, FColor().Blue, false, 1.5f, (uint8)'\000', 3.f);
        //AttachToActor(laserHit.GetActor(), FAttachmentTransformRules::KeepWorldTransform);	
        //NormalHitEffect(this, laserHit.GetActor());	
        m_beamParticleInfo_r.EnableBeam(laserHit.Location - GetActorLocation());
        BeamHitEffect(this, laserHit.GetActor());
    }
    else
    {
        //DrawDebugLine(GetWorld(), laserStartPoint, laserEndPoint, FColor().Blue, false, 1.5f, (uint8)'\000', 3.f);
        //TeleportTo(laserEndPoint, FRotator(0.f, 0.f, 0.f));	
        //NormalHitEffect(this, nullptr);	
        m_beamParticleInfo_r.EnableBeam(laserEndPoint - GetActorLocation());
        BeamHitEffect(this, nullptr);
    }
    //FTimerHandle t;	
    //GetWorldTimerManager().SetTimer(t, &m_beamParticleInfo_r, &FBeamInfoStruct_A::DisableBeam, 0.5f, false);
}
void APrototypeProjectile::ShockwaveProjectileEffect()
{
    //set mesh invis
    //attach to muzzle
    //hit enemy with multi sphere cast 
    //if objects hit > 0
        //bop enemy back
        //attach projectile
        //any hit effect
    //else
        //teleport this projectile max dist
        //call any hit effect

    m_skeletalMeshComponent->SetVisibility(false);
    //turn off collision
    m_colliderComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    //attach to muzzle
    AttachToComponent(m_weaponOfOrigin->GetArrowComponent(), FAttachmentTransformRules::SnapToTargetNotIncludingScale);


    int shockSphereRadius = 15;
    int LengthOffset = 30;
    int prevRadius = 0;
    TArray<FHitResult> ObjectsHit;
    FVector shockwaveStartPoint = GetActorLocation();
    FVector shockwaveCurrStartPoint;
    FVector shockwaveCurrEndPoint;
    FVector shockwaveDir = m_weaponOfOrigin->GetActorForwardVector();

    //FVector shockwaveEndPoint = (shockwaveDir * 1000.f) + shockwaveStartPoint;
    FCollisionQueryParams CollisionParams;
    CollisionParams.AddIgnoredActor(this);
    CollisionParams.AddIgnoredActor(Cast<AActor>(m_weaponOfOrigin));
    CollisionParams.AddIgnoredActor(Cast<AActor>(m_weaponOfOrigin->GetPlayerCharacter()));

    for (int i = 0; i < 5; i++)
    {
        shockSphereRadius += 25;
        shockwaveCurrStartPoint = (shockwaveDir * (shockSphereRadius + LengthOffset)) + shockwaveStartPoint;
        shockwaveCurrEndPoint = (shockwaveDir * shockSphereRadius) + shockwaveStartPoint;

        GetWorld()->SweepMultiByChannel(
            ObjectsHit, 
            shockwaveStartPoint,
            shockwaveCurrEndPoint, 
            FQuat::Identity, 
            ECollisionChannel::ECC_Visibility, 
            FCollisionShape::MakeSphere(shockSphereRadius), 
            CollisionParams
        );

        //DrawDebugSphere(GetWorld(), shockwaveStartPoint, shockSphereRadius, 50, FColor::Purple, false, 2.f);
        shockwaveStartPoint = shockwaveCurrStartPoint;



        if (ObjectsHit.Num() > 0)
        {
            for (int x = 0; x < ObjectsHit.Num(); x++)
            {
				if (ObjectsHit[x].GetActor())
				{
					ShockwaveHitEffect(this, ObjectsHit[x].GetActor());
					CollisionParams.AddIgnoredActor(Cast<AActor>(ObjectsHit[x].GetActor()));
				}
            }
        }
    }

    ClearTimers();
    //Destroy();	
    Cast<AMainGameMode>(GetWorld()->GetAuthGameMode())->AddNewBulletToPool(this);
}

void APrototypeProjectile::NormalHitEffect(APrototypeProjectile* CurrentProj, AActor* ObjectHit)
{
    //GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Green, "Normal Hit Effect");
    if (ObjectHit)
    {
		AMyCharacter* c;
		if (m_isFriendlyFire)
		{
			c = Cast<AMyCharacter>(ObjectHit);
		}
		else
		{
			c = Cast<AAIBaseCharacter>(ObjectHit);
		}
        if (c)
        {

            CurrentProj->DealDamage(c);
        }
    }
    

    if (m_targetToFollow)
    {
        m_targetToFollow->Destroy();
    }
    ClearTimers();


    CurrentProj->ProjectileHit(ObjectHit);
}
void APrototypeProjectile::BeamHitEffect(APrototypeProjectile* CurrentProj, AActor* ObjectHit)
{
    if (ObjectHit)
    {
        AMyCharacter* c;
        if (m_isFriendlyFire)
        {
            c = Cast<AMyCharacter>(ObjectHit);
        }
        else
        {
            c = Cast<AAIBaseCharacter>(ObjectHit);
        }
        if (c)
        {
            CurrentProj->DealDamage(c);
        }
    }
    FTimerHandle t;
    FTimerDelegate TimerDel;
    TimerDel.BindLambda([&]()
        {
            //Destroy();	
            Cast<AMainGameMode>(GetWorld()->GetAuthGameMode())->AddNewBulletToPool(this);
            m_beamParticleInfo_r.DisableBeam();
            StopParticle();
            if (m_targetToFollow)
            {
                m_targetToFollow->Destroy();
            }
            ClearTimers();
        }
    );
    GetWorld()->GetTimerManager().SetTimer(t, TimerDel, 0.2f, false);
}
void APrototypeProjectile::StickHitEffect(APrototypeProjectile* CurrentProj, AActor* ObjectHit)
{
    //GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Green, "Stick Hit");
    CurrentProj->AttachToActor(ObjectHit, FAttachmentTransformRules::KeepWorldTransform);
    CurrentProj->GetColliderComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);

    

    if (m_CrystalMarkedForDestruction)
    {
        ExplosiveHitEffect(CurrentProj, ObjectHit);
    }
    else
    {
        m_CrystalHasHit = true;
    }
    ClearTimers();
}
void APrototypeProjectile::ExplosiveHitEffect(APrototypeProjectile* CurrentProj, AActor* ObjectHit)
{
    //GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Green, "Explosive Hit Effect");
    //int ExplosiveRadius = 100 * GetActorScale3D().X;
    int ExplosiveRadius = 50 * m_explosiveScale;
    

    //PlaySound("Hit", true, ObjectHit->GetActorLocation());        -Explosive Sound
    //PlayParticle("Hit", true, ObjectHit->GetActorTransform());    -Explosive Particle

    FVector ExplosiveStartPoint = GetActorLocation();
    //int projectiles = m_weaponType_r == WeaponTypeEnum::WTE_Shotgun ? 5 : 1;
    FVector ExplosiveEndPoint = ObjectHit != nullptr ? ObjectHit->GetActorLocation() : ExplosiveStartPoint;

    FVector ExplosiveDir = GetActorForwardVector();

    FCollisionQueryParams CollisionParams;
    CollisionParams.AddIgnoredActor(this);
    CollisionParams.AddIgnoredActor(Cast<AActor>(m_weaponOfOrigin));
    CollisionParams.AddIgnoredActor(Cast<AActor>(m_weaponOfOrigin->GetPlayerCharacter()));
    if (Cast<APlayerCharacter>(ObjectHit))
    {
        CollisionParams.AddIgnoredActor(ObjectHit);
    }

    TArray<FHitResult> SweepHitResults;

    GetWorld()->SweepMultiByChannel(
        SweepHitResults,
        ExplosiveStartPoint,
        ExplosiveEndPoint,
        FQuat::Identity,
        ECollisionChannel::ECC_Visibility,
        FCollisionShape::MakeSphere(ExplosiveRadius),
        CollisionParams
    );

    //DrawDebugSphere(GetWorld(), ExplosiveStartPoint, ExplosiveRadius, 50, FColor::Purple, false, 0.5f);

    if (SweepHitResults.Num() > 0)
    {
        for (int i = 0; i < SweepHitResults.Num(); i++)
        {
			AMyCharacter* c;
			if (m_isFriendlyFire)
			{
				c = Cast<AMyCharacter>(SweepHitResults[i].GetActor());
			}
			else
			{
				c = Cast<AAIBaseCharacter>(SweepHitResults[i].GetActor());
			}
			if (c)
			{
                CurrentProj->DealDamage(c);
            }
        }
    }

    FTransform projWorldTransform = GetActorTransform();
    PlayParticle("Explosion", true, projWorldTransform);

    

    m_CrystalHasHit = false;
    ClearTimers();
    //Destroy();	
    if (m_weaponOfOrigin->ed_TriggerRelease.IsBound())
    {
        m_weaponOfOrigin->ed_TriggerRelease.RemoveDynamic(this, &APrototypeProjectile::OnTriggerRelease);
    }
    CurrentProj->ProjectileHit(ObjectHit);
    //Cast<AMainGameMode>(GetWorld()->GetAuthGameMode())->AddNewBulletToPool(this);
}
void APrototypeProjectile::ShockwaveHitEffect(APrototypeProjectile* CurrentProj, AActor* ObjectHit)
{
    //GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Green, "Shockwave Hit Effect");
    int ImpactForce = 1000;
    float RandomUp = FMath::RandRange(0.f, 1.f);
    PlaySound("Hit", true, ObjectHit->GetActorLocation());
    PlayParticle("Hit", true, ObjectHit->GetActorTransform());
    
    //FVector LaunchVector = m_weaponOfOrigin->GetActorForwardVector() + FVector(0, 0, 1);
    FVector LaunchVector = FVector(m_weaponOfOrigin->GetActorForwardVector().X, m_weaponOfOrigin->GetActorForwardVector().Y, 0.8f);

	AMyCharacter* c;
	if (m_isFriendlyFire)
	{
		c = Cast<AMyCharacter>(ObjectHit);
	}
	else
	{
		c = Cast<AAIBaseCharacter>(ObjectHit);
	}
	if (c)
	{
        c->GetMovementComponent()->StopMovementImmediately();
        c->LaunchCharacter(LaunchVector * ImpactForce, true, true);
        CurrentProj->DealDamage(c);
    }
    //Destroy();	
    //Cast<AMainGameMode>(GetWorld()->GetAuthGameMode())->AddNewBulletToPool(this);
}

void APrototypeProjectile::SetAltMeshForProjectile_Implementation(class USkeletalMesh* AltMesh)
{
    m_skeletalMeshComponent->SetSkeletalMesh(AltMesh);
}
bool APrototypeProjectile::SetAltMeshForProjectile_Validate(class USkeletalMesh* AltMesh) 
{ 
    return true; 
}

//Custom Effects
#pragma region
void APrototypeProjectile::StartEffectMultiShot()
{
    int numProjectilesToSpawn = (int)m_weaponOfOrigin->GetWeaponRarity() + 1;

    FActorSpawnParameters pickupSpawnParams;
    pickupSpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
    AProjectileActor* spawnedProjectile;
    APrototypeProjectile* NewProj;
    TArray<int> PossibleAngles = { -20, -10, 10, 20 };

    for (int i = 0; i < numProjectilesToSpawn; i++)
    {
        int index = FMath::RandRange(0, PossibleAngles.Num() - 1);
        int ChosenAngle = PossibleAngles[index];
        PossibleAngles.RemoveAt(index);

        //Original	
//spawnedProjectile = GetWorld()->SpawnActor<AProjectileActor>(	
//    m_weaponOfOrigin->GetProjectileClass(),	
//    GetActorLocation(),	
//    GetActorRotation(),	
//    pickupSpawnParams);	
        if (Cast<AMainGameMode>(GetWorld()->GetAuthGameMode())->IsBulletPoolValid())
        {
            spawnedProjectile = Cast<AMainGameMode>(GetWorld()->GetAuthGameMode())->GetBulletFromPool();
            spawnedProjectile->TeleportTo(GetActorLocation(), GetActorRotation());

            m_weaponOfOrigin->SetProjectileParticlesAndSound(Cast<APrototypeProjectile>(spawnedProjectile));
        }
        else
        {
            spawnedProjectile = GetWorld()->SpawnActor<AProjectileActor>(m_weaponOfOrigin->GetProjectileClass(), GetActorLocation(), GetActorRotation(), pickupSpawnParams);
            m_weaponOfOrigin->SetProjectileParticlesAndSound(Cast<APrototypeProjectile>(spawnedProjectile));
        }

        NewProj = Cast<APrototypeProjectile>(spawnedProjectile);
        if (NewProj)
        {
            ProjectileRotationAdjustment(NewProj, ChosenAngle, NewProj->GetActorUpVector());
            NewProj->Initialize(m_weaponOfOrigin, NewProj->GetActorForwardVector(), m_damage, m_isFriendlyFire);
            m_weaponOfOrigin->SetProjectileParticlesAndSound(Cast<APrototypeProjectile>(NewProj));
        }
    }
}

void APrototypeProjectile::FlightEffectRadialZap()
{
    float ZapFrequency = 1.f / (int)m_weaponOfOrigin->GetWeaponRarity();
    FTimerDelegate TimerDel;
    
    TimerDel.BindLambda([&]()
        {
            //FHitResult ObjectHit;
            TArray<FHitResult> ObjectsHit;
            FCollisionQueryParams CollisionParams;
            CollisionParams.AddIgnoredActor(this);
            CollisionParams.AddIgnoredActor(Cast<AActor>(m_weaponOfOrigin));
            CollisionParams.AddIgnoredActor(Cast<AActor>(m_weaponOfOrigin->GetPlayerCharacter()));
            int checkRadius = 50 * (int)m_weaponOfOrigin->GetWeaponRarity();

            FVector StartPt = GetActorLocation();
            FVector dir = GetActorForwardVector();
            FVector EndPt = GetActorLocation();
            GetWorld()->SweepMultiByChannel(
                ObjectsHit,
                StartPt,
                EndPt,
                FQuat::Identity,
                ECollisionChannel::ECC_Visibility,
                FCollisionShape::MakeSphere(checkRadius),
                CollisionParams
            );

            //DrawDebugSphere(GetWorld(), StartPt, checkRadius, 20, FColor::Purple, false, 1.f);

            for (int i = 0; i < ObjectsHit.Num() - 1; i++)
            {
                if (Cast<AAIBaseCharacter>(ObjectsHit[i].GetActor()))
                {
                    DealDamage(Cast<AAIBaseCharacter>(ObjectsHit[i].GetActor()));
                    break;
                }
            }
            //if (Cast<AAIBaseCharacter>(ObjectHit.GetActor()))
            //{
            //    DealDamage(Cast<AAIBaseCharacter>(ObjectHit.GetActor()));
            //}
        }
    );

    GetWorld()->GetTimerManager().SetTimer(RadialZapTimer, TimerDel, ZapFrequency, true);
}

void APrototypeProjectile::HitEffectSplit()
{
    if (Cast<AAIBaseCharacter>(m_ObjectThatWasHitByThisProjectile))
    {
        //GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Purple, "HIT EFFECT ACTIVATED");

        AAIBaseCharacter* Target = Cast<AAIBaseCharacter>(m_ObjectThatWasHitByThisProjectile);

        int numProjectilesToSpawn = (int)m_weaponOfOrigin->GetWeaponRarity() + 1;

        FActorSpawnParameters pickupSpawnParams;
        pickupSpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
        AProjectileActor* spawnedProjectile;
        APrototypeProjectile* NewProj;
        TArray<int> PossibleAngles;

        switch (numProjectilesToSpawn)
        {
        case 1:
            PossibleAngles.Add(0);
            break;
        case 2:
            PossibleAngles.Add(-10);
            PossibleAngles.Add(10);
            break;
        case 3:
            PossibleAngles.Add(-10);
            PossibleAngles.Add(0);
            PossibleAngles.Add(10);
            break;
        case 4:
            PossibleAngles.Add(-20);
            PossibleAngles.Add(-10);
            PossibleAngles.Add(10);
            PossibleAngles.Add(20);
            break;
        default:
            PossibleAngles.Add(0);
            break;
        }

        for (int i = 0; i < numProjectilesToSpawn; i++)
        {

            int index = FMath::RandRange(0, PossibleAngles.Num() - 1);
            int ChosenAngle = PossibleAngles[index];
            PossibleAngles.RemoveAt(index);

			FVector NewPos = Target->GetActorLocation() + (GetActorForwardVector() * 100);
			//FVector NewPos = Target->GetActorLocation() + (Target->GetActorForwardVector() * -100);
			//FVector NewPos = (Target->GetActorForwardVector() * -1) * 10;

            //DrawDebugSphere(GetWorld(), NewPos, 50, 50, FColor::Purple, true, 6.f);

                     //Original	
           /* spawnedProjectile = GetWorld()->SpawnActor<AProjectileActor>(
            spawnedProjectile = GetWorld()->SpawnActor<AProjectileActor>(
                m_weaponOfOrigin->GetProjectileClass(),
				NewPos,
                Target->GetActorForwardVector().Rotation() * -1,
                                pickupSpawnParams);*/

            if (Cast<AMainGameMode>(GetWorld()->GetAuthGameMode())->IsBulletPoolValid())
            {
                spawnedProjectile = Cast<AMainGameMode>(GetWorld()->GetAuthGameMode())->GetBulletFromPool();
                spawnedProjectile->TeleportTo(NewPos, GetActorRotation());
            }
            else
            {
                spawnedProjectile = GetWorld()->SpawnActor<AProjectileActor>(m_weaponOfOrigin->GetProjectileClass(), GetActorLocation(), GetActorRotation(), pickupSpawnParams);
            }

            NewProj = Cast<APrototypeProjectile>(spawnedProjectile);
            if (NewProj)
            {
                ProjectileRotationAdjustment(NewProj, ChosenAngle, NewProj->GetActorUpVector());
                NewProj->SetEnableSpawnEffect(false);
                NewProj->SetEnableFlightEffect(false);
                NewProj->SetEnableHitEffect(false);
                NewProj->Initialize(m_weaponOfOrigin, NewProj->GetActorForwardVector(), m_damage, m_isFriendlyFire);
            }
        }
    }
}

void APrototypeProjectile::ProjectileRotationAdjustment(APrototypeProjectile* ProjToRotate, int AngleToRotate, FVector AxisToRotateOn)
{
    ProjToRotate->SetProjectileAmmoType(m_projAmmoType);
    ProjToRotate->SetEnableSpawnEffect(false);
    FVector currDirection = GetActorForwardVector();
    FVector NewRotationDir = currDirection.RotateAngleAxis(AngleToRotate, AxisToRotateOn);

    FRotator NewRot = NewRotationDir.Rotation();
    ProjToRotate->SetActorRotation(NewRot);
}
#pragma endregion

void APrototypeProjectile::GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    DOREPLIFETIME(APrototypeProjectile, MeshCrystal)
    DOREPLIFETIME(APrototypeProjectile, MeshBalloon)
    DOREPLIFETIME(APrototypeProjectile, startingPos)
    DOREPLIFETIME(APrototypeProjectile, scalingAmount)
    DOREPLIFETIME(APrototypeProjectile, trailingAmount)

}
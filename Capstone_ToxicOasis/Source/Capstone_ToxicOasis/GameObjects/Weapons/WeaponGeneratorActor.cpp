// Fill out your copyright notice in the Description page of Project Settings.


#include "WeaponGeneratorActor.h"

#include "UnrealNetwork.h"

#include "WeaponPartActor.h"
#include "../Interactables/Pickups/Weapons/WeaponPickup.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components//BoxComponent.h"
#include "GameObjects/Projectiles/ProjectileActor.h"
#include "GameObjects/Projectiles/PrototypeProjectileEffect/PrototypeProjectile.h"
#include "GameMode/MainGameMode.h"	
#include "Sound/SoundCue.h"

//Functionality Includes
#include "Runtime/Engine/Classes/Engine/Engine.h"
#include "Runtime/AssetRegistry/Public/AssetRegistryModule.h"
#include "Runtime/AssetRegistry/Public/ARFilter.h"
#include "Kismet/KismetMathLibrary.h"
#include "UObject/UObjectGlobals.h"
#include "UObject/ConstructorHelpers.h"


//Release 3 New Features
/**
    - can now specify default projectile type for each available weapon
    - now can adjust which ammo types spawn more often
*/

// Sets default values
AWeaponGeneratorActor::AWeaponGeneratorActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;


    SetReplicates(true);
    bAlwaysRelevant = false;
    for (int i = 0; i < (int)WeaponTypeEnum::WTE_Invalid; i++)
    {
        m_Sounds_ParticleFX_Arr.Add(FWeaponSoundsParticleFX((WeaponTypeEnum)i));
    }
}

void AWeaponGeneratorActor::SetWeaponParticlesAndSound(AWeaponPickup* a_weapon)
{
    //a_weapon->m_soundCues.Add("Hit");
    //a_weapon->m_soundCues.Add("HitCharacter");
    a_weapon->m_soundCues.Add("Beam");

    //a_weapon->m_particleFXs.Add("Hit");
    //a_weapon->m_particleFXs.Add("HitCharacter");
    a_weapon->m_particleFXs.Add("Beam");
    a_weapon->m_particleFXs.Add("Explosion");

    a_weapon->m_soundCues["Hit"].m_sound = m_Sounds_ParticleFX_Arr[a_weapon->GetWeaponType()].Sounds_Particle_Data[ammoType].Hit_S;
    a_weapon->m_soundCues["HitCharacter"].m_sound = m_Sounds_ParticleFX_Arr[a_weapon->GetWeaponType()].Sounds_Particle_Data[ammoType].HitCharacter_S;
    a_weapon->m_soundCues["Beam"].m_sound = m_Sounds_ParticleFX_Arr[a_weapon->GetWeaponType()].Sounds_Particle_Data[ammoType].Beam_S;
    a_weapon->m_soundCues["Fire"].m_sound = m_Sounds_ParticleFX_Arr[a_weapon->GetWeaponType()].Sounds_Particle_Data[ammoType].Fire_S;
    a_weapon->m_soundCues["StartFiringEmpty"].m_sound = m_Sounds_ParticleFX_Arr[a_weapon->GetWeaponType()].Sounds_Particle_Data[ammoType].StartFiringEmpty_S;
    a_weapon->m_soundCues["Reload"].m_sound = m_Sounds_ParticleFX_Arr[a_weapon->GetWeaponType()].Sounds_Particle_Data[ammoType].Reload_S;
    a_weapon->m_soundCues["Equip"].m_sound = m_Sounds_ParticleFX_Arr[a_weapon->GetWeaponType()].Sounds_Particle_Data[ammoType].Equip_S;
    a_weapon->m_soundCues["Unequip"].m_sound = m_Sounds_ParticleFX_Arr[a_weapon->GetWeaponType()].Sounds_Particle_Data[ammoType].Unequip_S;

    a_weapon->m_particleFXs["Hit"].m_particle = m_Sounds_ParticleFX_Arr[a_weapon->GetWeaponType()].Sounds_Particle_Data[ammoType].Hit_P;
    a_weapon->m_particleFXs["HitCharacter"].m_particle = m_Sounds_ParticleFX_Arr[a_weapon->GetWeaponType()].Sounds_Particle_Data[ammoType].HitCharacter_P;
    a_weapon->m_particleFXs["Beam"].m_particle = m_Sounds_ParticleFX_Arr[a_weapon->GetWeaponType()].Sounds_Particle_Data[ammoType].Beam_P;
    a_weapon->m_particleFXs["Fire"].m_particle = m_Sounds_ParticleFX_Arr[a_weapon->GetWeaponType()].Sounds_Particle_Data[ammoType].Fire_P;
    a_weapon->m_particleFXs["Explosion"].m_particle = m_Sounds_ParticleFX_Arr[a_weapon->GetWeaponType()].Sounds_Particle_Data[ammoType].Fire_P;
}

void AWeaponGeneratorActor::GenerateWeaponFromTemplate(AWeaponPickup* WeaponTemplate)
{
    if (GetLocalRole() == ROLE_Authority)
    {
        int RandomWeaponChoice = FMath::RandRange(1, 4);
        //int RandomWeaponChoice = FMath::RandRange(1, 3);
        //int RandomWeaponChoice = 1;

        switch (WeaponTemplate->GetWeaponType())
        {

        case (int)WeaponTypeEnum::WTE_Rifle:
            GenerateRifle(WeaponTemplate);
            break;
        case (int)WeaponTypeEnum::WTE_Pistol:
            GeneratePistol(WeaponTemplate);
            break;
        case (int)WeaponTypeEnum::WTE_Shotgun:
            GenerateShotgun(WeaponTemplate);
            break;
        case (int)WeaponTypeEnum::WTE_PDW:
            GeneratePDWRifle(WeaponTemplate);
            break;
        default:
            //Rifle Weapon Generation/Attachment Logic
            if (RandomWeaponChoice == 1)
            {
                GenerateRifle(WeaponTemplate);
            }
            //Pistol Weapon Generation/Attachment Logic
            else if (RandomWeaponChoice == 2)
            {
                GeneratePistol(WeaponTemplate);
            }
            //Shotgun Weapon Generation/Attachment Logic
            else if (RandomWeaponChoice == 3)
            {
                GenerateShotgun(WeaponTemplate);
            }
            else if (RandomWeaponChoice == 4)
            {
                //PDW Weapon Generation//Attachment Logic
                GeneratePDWRifle(WeaponTemplate);
            }
            break;
        }
        //Manual application of the onStartEffect	
        /* int CustomStartEffect = (int)CustomStartEffects::CSE_MultiShot;
        WeaponTemplate->SetOnProjectileStartEffect((CustomStartEffects)CustomStartEffect);*/


        //Release 3
        //Random Roll for the starting projectile effect
        if (generalAmmoType != (int)AmmoTypes::AT_ShockWave)
        {
            //Roll for custom on start effect
            if (RandomChance(10, ((int)WeaponTemplate->GetWeaponRarity() + 1)))
                //if (GrantEffect)
            {
                int CustomEffect = FMath::RandRange(0, (int)CustomStartEffects::CSE_NumberOfEffects - 1);
                WeaponTemplate->SetOnProjectileStartEffect((CustomStartEffects)CustomEffect);
            }
            else
            {
                WeaponTemplate->SetOnProjectileStartEffect(CustomStartEffects::CSE_NumberOfEffects);
            }
        }

        //Random Roll for the int flight projectile effect
       /* if (generalAmmoType != (int)AmmoTypes::AT_ShockWave && generalAmmoType != (int)AmmoTypes::AT_Laser)
        {
            if (RandomChance(10, ((int)WeaponTemplate->GetWeaponRarity() + 1)))
            {
                int CustomEffect = FMath::RandRange(0, (int)CustomInFlightEffects::CIFE_NumberOfEffects - 1);
                WeaponTemplate->SetInFlightProjectileEffect((CustomInFlightEffects)CustomEffect);
            }
        }*/

        //Random roll for the on hit effects
        //if (generalAmmoType != (int)AmmoTypes::AT_ShockWave)	
        //{	
        //    if (RandomChance(10, ((int)WeaponTemplate->GetWeaponRarity() + 1)))	
        //    {	
        //        int CustomEffect = FMath::RandRange(0, (int)CustomOnHitEffects::COHE_NumberOfEffects - 1);	
        //        WeaponTemplate->SetOnProjectileHitEffect((CustomOnHitEffects)CustomEffect);	
        //    }	
        //    else	
        //    {	
        //        WeaponTemplate->SetOnProjectileHitEffect(CustomOnHitEffects::COHE_NumberOfEffects);	
        //    }	
        //}

        //Release 3	
        if (WeaponTemplate->GetNumberOfWeaponParts() < 5 && WeaponTemplate->GetWeaponType() != (int)WeaponTypeEnum::WTE_Invalid && WeaponTemplate->GetPickupType() != InventoryTabEnum::ITE_Invalid)
        {
            WeaponTemplate->DestroySelf();
        }
        else
        {
            SetWeaponParticlesAndSound(WeaponTemplate);
            WeaponTemplate->SetWeaponProjectile(m_prototypeWeaponProjectile);
        }
    }
    
}

void AWeaponGeneratorActor::PreInitializeComponents()
{
    Super::PreInitializeComponents();

    //if (GetLocalRole() == ROLE_Authority)
    //{
    //    InitializeWeaponSetupArrays();
    //    SERVER_PopulateWeaponPartArrays();
    //    //SERVER_PreGenerateBulletPool();
    //    SERVER_PreGenerateWeaponPool();
    //}
}

void AWeaponGeneratorActor::SERVER_PreGenerateWeaponPool_Implementation()
{
    m_WeaponBPTypes = { m_CommonWeaponTemplate, m_UncommonWeaponTemplate, m_RareWeaponTemplate, m_LegendaryWeaponTemplate };
    //int numWeapons = 100;	
    int NumberOfEachTypeOfWeapon = m_NumberOfWeaponsToSpawn / 4;
    //Will be incremented to determine which pool to place the weapons in	
    int rarityCounter = 0;
    for (int i = 0; i < m_NumberOfWeaponsToSpawn; i++)
    {
        FActorSpawnParameters pickupSpawnParams;
        AWeaponPickup* NewWeapon;
        if (m_WeaponBPTypes[rarityCounter])
        {
            NewWeapon = GetWorld()->SpawnActor<AWeaponPickup>(m_WeaponBPTypes[rarityCounter], FVector(10000, -10000, 0), FRotator(0, 0, 0));
        }
        else
        {
            NewWeapon = GetWorld()->SpawnActor<AWeaponPickup>(FVector(10000, -10000, 0), FRotator(0, 0, 0));
        }
        NewWeapon->SetWeaponRarity((WeaponRarityEnum)rarityCounter);
        GenerateWeaponFromTemplate(NewWeapon);
        Cast<UPrimitiveComponent>(NewWeapon->GetRootComponent())->SetSimulatePhysics(false);

        Cast<AMainGameMode>(GetWorld()->GetAuthGameMode())->AddNewWeaponToPool(NewWeapon);



        if (i == (NumberOfEachTypeOfWeapon * (rarityCounter+1)))
        {
            rarityCounter++;
        }

    }
    //Need to tell game mode the game is ready to start	
    //Cast<AMainGameMode>(GetWorld()->GetAuthGameMode())->SetGameIsReadyToStart();	
}
void AWeaponGeneratorActor::SERVER_PreGenerateBulletPool_Implementation()
{
    int numBullets = 500;
    for (int i = 0; i < numBullets; i++)
    {
        FActorSpawnParameters pickupSpawnParams;
        APrototypeProjectile* NewBullet = GetWorld()->SpawnActor<APrototypeProjectile>(m_prototypeWeaponProjectile, FVector(0, -5000, 0), FRotator(0, 0, 0));
        //FTransform transform;	
        //transform.SetLocation(FVector(0, 0, 0));	
        //transform.SetRotation(FRotator(0, 0, 0).Quaternion());	
        //APrototypeProjectile* NewBullet = GetWorld()->SpawnActorDeferred<APrototypeProjectile>(m_prototypeWeaponProjectile, transform);	
        Cast<AMainGameMode>(GetWorld()->GetAuthGameMode())->AddNewBulletToPool(NewBullet);	
    }
}


//Logic to generate a rifle for weapon template
    //-creates new parts for a rifle to attach to template
void AWeaponGeneratorActor::GenerateRifle(AWeaponPickup* WeaponTemplate)
{
    if (RifleAmmoType != RifleCarbineAmmoTypes::RCAT_NumTypes)
        ammoType = (int)RifleAmmoType;
    else
    {
        TArray<int> listedValues = { m_rifleBallisticWeaponChance , m_rifleLaserWeaponChance , m_rifleCrystalWeaponChance , m_rifleBalloonWeaponChance };
        int maxValue = listedValues[0] + listedValues[1] + listedValues[2] + listedValues[3];
        int randomVal = FMath::RandRange(1, maxValue);
        int val = 0;
        for (int i = 0; i < listedValues.Num(); i++)
        {
            randomVal -= listedValues[i];
            if (randomVal <= 0)
            {
                ammoType = i;
                break;
            }
        }
        //ammoType = FMath::RandRange(0, (int)RifleCarbineAmmoTypes::RCAT_NumTypes - 1);
    }

    int RandY = FMath::RandRange(-400, 400);
    AWeaponPartActor* TempReceiver = GetWorld()->SpawnActor<AWeaponPartActor>
        (GetRandomPart(WeaponTypeVariant::WTV_RifleCarbine, WeaponPartCategories::WPC_Receiver, 0), FVector(0.f, RandY, 190.f), FRotator(0, 0, 0));
    TempReceiver->AttachToActor(WeaponTemplate, FAttachmentTransformRules::SnapToTargetNotIncludingScale);

    AWeaponPartActor* TempBarrel = GetWorld()->SpawnActor<AWeaponPartActor>
        (GetRandomPart(WeaponTypeVariant::WTV_RifleCarbine, WeaponPartCategories::WPC_Barrel, ammoType), FVector(0.f, RandY, 190.f), FRotator(0, 0, 0));
    TempBarrel->AttachToActor(TempReceiver, FAttachmentTransformRules::SnapToTargetNotIncludingScale, FName("Barrel_Socket"));

    AWeaponPartActor* TempMag = GetWorld()->SpawnActor<AWeaponPartActor>
        (GetRandomPart(WeaponTypeVariant::WTV_RifleCarbine, WeaponPartCategories::WPC_Magazine, ammoType), FVector(0.f, RandY, 190.f), FRotator(0, 0, 0));
    TempMag->AttachToActor(TempReceiver, FAttachmentTransformRules::SnapToTargetNotIncludingScale, FName("Magazine_Socket"));

    AWeaponPartActor* TempOptic = GetWorld()->SpawnActor<AWeaponPartActor>
        (GetRandomPart(WeaponTypeVariant::WTV_RifleCarbine, WeaponPartCategories::WPC_Accessory, 0), FVector(0.f, RandY, 190.f), FRotator(0, 0, 0));
    TempOptic->AttachToActor(TempBarrel, FAttachmentTransformRules::SnapToTargetNotIncludingScale, FName("Optic_Socket"));

    AWeaponPartActor* TempStock = GetWorld()->SpawnActor<AWeaponPartActor>
        (GetRandomPart(WeaponTypeVariant::WTV_RifleCarbine, WeaponPartCategories::WPC_ErgoEnhancer, 0), FVector(0.f, RandY, 190.f), FRotator(0, 0, 0));
    TempStock->AttachToActor(TempReceiver, FAttachmentTransformRules::SnapToTargetNotIncludingScale, FName("Stock_Socket"));

    //Release 2 --TEMP--
    //WeaponTemplate->SetOnProjectileStartEffect(TempStock->GetProjectileStartEffect());
    //Release 2 --TEMP--
    //WeaponTemplate->SetInFlightProjectileEffect(TempBarrel->GetProjectileInFlightEffect());
    //Release 2 --TEMP--
    //WeaponTemplate->SetOnProjectileHitEffect(TempMag->GetProjectileOnHitEffect());

    WeaponTemplate->AddNewWeaponPartToWeaponPartArray(TempBarrel);
    WeaponTemplate->AddNewWeaponPartToWeaponPartArray(TempReceiver);
    WeaponTemplate->AddNewWeaponPartToWeaponPartArray(TempMag);
    WeaponTemplate->AddNewWeaponPartToWeaponPartArray(TempStock);
    WeaponTemplate->AddNewWeaponPartToWeaponPartArray(TempOptic);

    WeaponTemplate->RegisterMesh(Cast<USkeletalMeshComponent>(TempBarrel->GetRootComponent()));
    WeaponTemplate->RegisterMesh(Cast<USkeletalMeshComponent>(TempReceiver->GetRootComponent()));
    WeaponTemplate->RegisterMesh(Cast<USkeletalMeshComponent>(TempStock->GetRootComponent()));
    WeaponTemplate->RegisterMesh(Cast<USkeletalMeshComponent>(TempMag->GetRootComponent()));
    WeaponTemplate->RegisterMesh(Cast<USkeletalMeshComponent>(TempOptic->GetRootComponent()));

    //WeaponTemplate->CompleteWeaponAssembly();

    WeaponTemplate->SetPickupType(InventoryTabEnum::ITE_Rifles);
    WeaponTemplate->SetWeaponType(WeaponTypeEnum::WTE_Rifle);

    //Randomization for the weapon rarity multiplier
    int rarityTemp = FMath::RandRange(1, 4);
    if (WeaponTemplate->GetWeaponRarity() == WeaponRarityEnum::WRE_Invalid)
    {
        switch (rarityTemp)
        {
        case 1:
            WeaponTemplate->SetWeaponRarity(WeaponRarityEnum::WRE_Common);
            break;
        case 2:
            WeaponTemplate->SetWeaponRarity(WeaponRarityEnum::WRE_Uncommon);
            break;
        case 3:
            WeaponTemplate->SetWeaponRarity(WeaponRarityEnum::WRE_Rare);
            break;
        case 4:
            WeaponTemplate->SetWeaponRarity(WeaponRarityEnum::WRE_Legendary);
            break;
        default:
            WeaponTemplate->SetWeaponRarity(WeaponRarityEnum::WRE_Common);
            break;
        }
    }

    if (ammoType == (int)RifleCarbineAmmoTypes::RCAT_Balloon)
    {
        WeaponTemplate->SetWeaponFireMode(WeaponFireModeEnum::WFME_Single);
    }
    else if (ammoType == (int)RifleCarbineAmmoTypes::RCAT_Crystal)
    {
        WeaponTemplate->SetWeaponFireMode(WeaponFireModeEnum::WFME_Burst);
    }
    else
    {
        int fireModeTemp = FMath::RandRange(1, 3);
        if (WeaponTemplate->GetWeaponFiremode() == WeaponFireModeEnum::WFME_Invalid)
        {
            switch (fireModeTemp)
            {
            case 1:
                WeaponTemplate->SetWeaponFireMode(WeaponFireModeEnum::WFME_Single);
                break;
            case 2:
                WeaponTemplate->SetWeaponFireMode(WeaponFireModeEnum::WFME_Burst);
                break;
            case 3:
                WeaponTemplate->SetWeaponFireMode(WeaponFireModeEnum::WFME_Auto);
                break;
            default:
                WeaponTemplate->SetWeaponFireMode(WeaponFireModeEnum::WFME_Single);
                break;
            }
        }
    }
    

    //generalAmmoType = 0;

    switch (ammoType)
    {
    case (int)RifleCarbineAmmoTypes::RCAT_Ballistic:
        generalAmmoType = (int)AmmoTypes::AT_Ballistic;
        break;
    case (int)RifleCarbineAmmoTypes::RCAT_Balloon:
        generalAmmoType = (int)AmmoTypes::AT_Balloon;
        break;
    case (int)RifleCarbineAmmoTypes::RCAT_Crystal:
        generalAmmoType = (int)AmmoTypes::AT_Crystal;
        break;
    case (int)RifleCarbineAmmoTypes::RCAT_Laser:
        generalAmmoType = (int)AmmoTypes::AT_Laser;
        break;
    default:
        generalAmmoType = (int)AmmoTypes::AT_Ballistic;
        break;
    }

    WeaponTemplate->SetProjectileType(generalAmmoType);
    WeaponTemplate->CompleteWeaponAssembly();
    
}

void AWeaponGeneratorActor::GeneratePDWRifle(AWeaponPickup* WeaponTemplate)
{
    if (PDWAmmoType != RiflePDWAmmoTypes::RPAT_NumTypes)
        ammoType = (int)RifleAmmoType;
    else
    {
        TArray<int> listedValues = { m_pdwBallisticWeaponChance };
        int maxValue = listedValues[0];
        int randomVal = FMath::RandRange(1, maxValue);
        for (int i = 0; i < listedValues.Num(); i++)
        {
            randomVal -= listedValues[i];
            if (randomVal <= 0)
            {
                ammoType = i;
                break;
            }
        }
        //ammoType = FMath::RandRange(0, (int)RiflePDWAmmoTypes::RPAT_NumTypes - 1);
        //ammoType = FMath::RandRange(0, 0);
    }

    int RandY = FMath::RandRange(-400, 400);
    AWeaponPartActor* TempReceiver = GetWorld()->SpawnActor<AWeaponPartActor>
        (GetRandomPart(WeaponTypeVariant::WTV_RiflePDW, WeaponPartCategories::WPC_Receiver, ammoType), FVector(0.f, RandY, 190.f), FRotator(0, 0, 0));
    TempReceiver->AttachToActor(WeaponTemplate, FAttachmentTransformRules::SnapToTargetNotIncludingScale);

    AWeaponPartActor* TempBarrel = GetWorld()->SpawnActor<AWeaponPartActor>
        (GetRandomPart(WeaponTypeVariant::WTV_RiflePDW, WeaponPartCategories::WPC_Barrel, ammoType), FVector(0.f, RandY, 190.f), FRotator(0, 0, 0));
        //(GetRandomPart((WeaponTypeVariant)1, (WeaponPartCategories)2, 0), FVector(0.f, RandY, 190.f), FRotator(0, 0, 0));
    TempBarrel->AttachToActor(TempReceiver, FAttachmentTransformRules::SnapToTargetNotIncludingScale, FName("Barrel_Socket"));

    AWeaponPartActor* TempMag = GetWorld()->SpawnActor<AWeaponPartActor>
        (GetRandomPart(WeaponTypeVariant::WTV_RiflePDW, WeaponPartCategories::WPC_Magazine, ammoType), FVector(0.f, RandY, 190.f), FRotator(0, 0, 0));
        //(GetRandomPart((WeaponTypeVariant)1, (WeaponPartCategories)3, 0), FVector(0.f, RandY, 190.f), FRotator(0, 0, 0));
    TempMag->AttachToActor(TempReceiver, FAttachmentTransformRules::SnapToTargetNotIncludingScale, FName("Magazine_Socket"));

    AWeaponPartActor* TempGrip = GetWorld()->SpawnActor<AWeaponPartActor>
        (GetRandomPart(WeaponTypeVariant::WTV_RiflePDW, WeaponPartCategories::WPC_Accessory, ammoType), FVector(0.f, RandY, 190.f), FRotator(0, 0, 0));
        //(GetRandomPart((WeaponTypeVariant)1, (WeaponPartCategories)4, 0), FVector(0.f, RandY, 190.f), FRotator(0, 0, 0));
    TempGrip->AttachToActor(TempReceiver, FAttachmentTransformRules::SnapToTargetNotIncludingScale, FName("Grip_Socket"));

    AWeaponPartActor* TempStock = GetWorld()->SpawnActor<AWeaponPartActor>
        (GetRandomPart(WeaponTypeVariant::WTV_RiflePDW, WeaponPartCategories::WPC_ErgoEnhancer, ammoType), FVector(0.f, RandY, 190.f), FRotator(0, 0, 0));
        //(GetRandomPart((WeaponTypeVariant)1, (WeaponPartCategories)1, 0), FVector(0.f, RandY, 190.f), FRotator(0, 0, 0));
    TempStock->AttachToActor(TempReceiver, FAttachmentTransformRules::SnapToTargetNotIncludingScale, FName("Stock_Socket"));

    WeaponTemplate->AddNewWeaponPartToWeaponPartArray(TempBarrel);
    WeaponTemplate->AddNewWeaponPartToWeaponPartArray(TempReceiver);
    WeaponTemplate->AddNewWeaponPartToWeaponPartArray(TempMag);
    WeaponTemplate->AddNewWeaponPartToWeaponPartArray(TempStock);
    WeaponTemplate->AddNewWeaponPartToWeaponPartArray(TempGrip);

    WeaponTemplate->RegisterMesh(Cast<USkeletalMeshComponent>(TempBarrel->GetRootComponent()));
    WeaponTemplate->RegisterMesh(Cast<USkeletalMeshComponent>(TempReceiver->GetRootComponent()));
    WeaponTemplate->RegisterMesh(Cast<USkeletalMeshComponent>(TempStock->GetRootComponent()));
    WeaponTemplate->RegisterMesh(Cast<USkeletalMeshComponent>(TempMag->GetRootComponent()));
    WeaponTemplate->RegisterMesh(Cast<USkeletalMeshComponent>(TempGrip->GetRootComponent()));

    WeaponTemplate->SetPickupType(InventoryTabEnum::ITE_PDWs);
    WeaponTemplate->SetWeaponType(WeaponTypeEnum::WTE_PDW);

    //Randomization for the weapon rarity multiplier
    int rarityTemp = FMath::RandRange(1, 4);
    if (WeaponTemplate->GetWeaponRarity() == WeaponRarityEnum::WRE_Invalid)
    {
        switch (rarityTemp)
        {
        case 1:
            WeaponTemplate->SetWeaponRarity(WeaponRarityEnum::WRE_Common);
            break;
        case 2:
            WeaponTemplate->SetWeaponRarity(WeaponRarityEnum::WRE_Uncommon);
            break;
        case 3:
            WeaponTemplate->SetWeaponRarity(WeaponRarityEnum::WRE_Rare);
            break;
        case 4:
            WeaponTemplate->SetWeaponRarity(WeaponRarityEnum::WRE_Legendary);
            break;
        default:
            WeaponTemplate->SetWeaponRarity(WeaponRarityEnum::WRE_Common);
            break;
        }
    }

    int fireModeTemp = FMath::RandRange(1, 3);
    if (WeaponTemplate->GetWeaponFiremode() == WeaponFireModeEnum::WFME_Invalid)
    {
        switch (fireModeTemp)
        {
        case 1:
            WeaponTemplate->SetWeaponFireMode(WeaponFireModeEnum::WFME_Single);
            break;
        case 2:
            WeaponTemplate->SetWeaponFireMode(WeaponFireModeEnum::WFME_Burst);
            break;
        case 3:
            WeaponTemplate->SetWeaponFireMode(WeaponFireModeEnum::WFME_Auto);
            break;
        default:
            WeaponTemplate->SetWeaponFireMode(WeaponFireModeEnum::WFME_Single);
            break;
        }
    }

    switch (ammoType)
    {
    case (int)RiflePDWAmmoTypes::RPAT_Ballistic:
        generalAmmoType = (int)AmmoTypes::AT_Ballistic;
        break;
    default:
        generalAmmoType = (int)AmmoTypes::AT_Ballistic;
        break;
    }

    WeaponTemplate->SetProjectileType(generalAmmoType);
    WeaponTemplate->CompleteWeaponAssembly();
}

//Logic to generate a Pistol for weapon template
    //-creates new parts for a Pistol to attach to template
void AWeaponGeneratorActor::GeneratePistol(AWeaponPickup* WeaponTemplate)
{
    if (PistolAmmoType != PistolAmmoTypes::PAT_NumTypes)
        ammoType = (int)PistolAmmoType;
    else
    {
        TArray<int> listedValues = { m_pistolBallisticWeaponChance };
        int maxValue = listedValues[0];
        int randomVal = FMath::RandRange(1, maxValue);
        for (int i = 0; i < listedValues.Num(); i++)
        {
            randomVal -= listedValues[i];
            if (randomVal <= 0)
            {
                ammoType = i;
                break;
            }
        }

        //ammoType = FMath::RandRange(0, (int)PistolAmmoTypes::PAT_NumTypes - 1);
    }

    int RandY = FMath::RandRange(-400, 400);
    AWeaponPartActor* TempFrame = GetWorld()->SpawnActor<AWeaponPartActor>
        //(m_pistolFrameParts[FMath::RandRange(0, m_pistolFrameParts.Num() - 1)], FVector(0.f, RandY, 190.f), FRotator(0, 0, 0));
        (GetRandomPart(WeaponTypeVariant::WTV_Pistol, WeaponPartCategories::WPC_Receiver, 0), FVector(0.f, RandY, 190.f), FRotator(0, 0, 0));
    TempFrame->AttachToActor(WeaponTemplate, FAttachmentTransformRules::SnapToTargetNotIncludingScale);

    AWeaponPartActor* TempBarrel = GetWorld()->SpawnActor<AWeaponPartActor>
        //(m_pistolBarrelParts[FMath::RandRange(0, m_pistolBarrelParts.Num() - 1)], FVector(0.f, RandY, 190.f), FRotator(0, 0, 0));
        (GetRandomPart(WeaponTypeVariant::WTV_Pistol, WeaponPartCategories::WPC_Barrel, ammoType), FVector(0.f, RandY, 190.f), FRotator(0, 0, 0));
    TempBarrel->AttachToActor(TempFrame, FAttachmentTransformRules::SnapToTargetNotIncludingScale, FName("Barrel_Socket"));

    AWeaponPartActor* TempMag = GetWorld()->SpawnActor<AWeaponPartActor>
        //(m_pistolMagazineParts[FMath::RandRange(0, m_pistolMagazineParts.Num() - 1)], FVector(0.f, RandY, 190.f), FRotator(0, 0, 0));
        (GetRandomPart(WeaponTypeVariant::WTV_Pistol, WeaponPartCategories::WPC_Magazine, ammoType), FVector(0.f, RandY, 190.f), FRotator(0, 0, 0));
    TempMag->AttachToActor(TempFrame, FAttachmentTransformRules::SnapToTargetNotIncludingScale, FName("Magazine_Socket"));

    AWeaponPartActor* TempSlide = GetWorld()->SpawnActor<AWeaponPartActor>
        //(m_pistolSlideParts[FMath::RandRange(0, m_pistolSlideParts.Num() - 1)], FVector(0.f, RandY, 190.f), FRotator(0, 0, 0));
        (GetRandomPart(WeaponTypeVariant::WTV_Pistol, WeaponPartCategories::WPC_Accessory, ammoType), FVector(0.f, RandY, 190.f), FRotator(0, 0, 0));
    TempSlide->AttachToActor(TempFrame, FAttachmentTransformRules::SnapToTargetNotIncludingScale, FName("Slide_Socket"));

    AWeaponPartActor* TempTrigGuard = GetWorld()->SpawnActor<AWeaponPartActor>
        //(m_pistolTriggerGuardParts[FMath::RandRange(0, m_pistolTriggerGuardParts.Num() - 1)], FVector(0.f, RandY, 190.f), FRotator(0, 0, 0));
        (GetRandomPart(WeaponTypeVariant::WTV_Pistol, WeaponPartCategories::WPC_ErgoEnhancer, ammoType), FVector(0.f, RandY, 190.f), FRotator(0, 0, 0));
    TempTrigGuard->AttachToActor(TempFrame, FAttachmentTransformRules::SnapToTargetNotIncludingScale, FName("TriggerGuard_Socket"));

    //Release 2 --TEMP--
    //WeaponTemplate->SetOnProjectileStartEffect(TempTrigGuard->GetProjectileStartEffect());
    //Release 2 --TEMP--
    //WeaponTemplate->SetInFlightProjectileEffect(TempBarrel->GetProjectileInFlightEffect());
    //Release 2 --TEMP--
    //WeaponTemplate->SetOnProjectileHitEffect(TempMag->GetProjectileOnHitEffect());

    WeaponTemplate->AddNewWeaponPartToWeaponPartArray(TempFrame);
    WeaponTemplate->AddNewWeaponPartToWeaponPartArray(TempBarrel);
    WeaponTemplate->AddNewWeaponPartToWeaponPartArray(TempMag);
    WeaponTemplate->AddNewWeaponPartToWeaponPartArray(TempSlide);
    WeaponTemplate->AddNewWeaponPartToWeaponPartArray(TempTrigGuard);

    WeaponTemplate->RegisterMesh(Cast<USkeletalMeshComponent>(TempFrame->GetRootComponent()));
    WeaponTemplate->RegisterMesh(Cast<USkeletalMeshComponent>(TempBarrel->GetRootComponent()));
    WeaponTemplate->RegisterMesh(Cast<USkeletalMeshComponent>(TempMag->GetRootComponent()));
    WeaponTemplate->RegisterMesh(Cast<USkeletalMeshComponent>(TempSlide->GetRootComponent()));
    WeaponTemplate->RegisterMesh(Cast<USkeletalMeshComponent>(TempTrigGuard->GetRootComponent()));

    //WeaponTemplate->CompleteWeaponAssembly();
    WeaponTemplate->SetPickupType(InventoryTabEnum::ITE_Pistols);
    WeaponTemplate->SetWeaponType(WeaponTypeEnum::WTE_Pistol);
    WeaponTemplate->GetColliderComponent()->SetBoxExtent(FVector(16.f, 7.f, 14.f));

    //Randomization for the weapon rarity multiplier
    int rarityTemp = FMath::RandRange(1, 4);
    if (WeaponTemplate->GetWeaponRarity() == WeaponRarityEnum::WRE_Invalid)
    {
        switch (rarityTemp)
        {
        case 1:
            WeaponTemplate->SetWeaponRarity(WeaponRarityEnum::WRE_Common);
            break;
        case 2:
            WeaponTemplate->SetWeaponRarity(WeaponRarityEnum::WRE_Uncommon);
            break;
        case 3:
            WeaponTemplate->SetWeaponRarity(WeaponRarityEnum::WRE_Rare);
            break;
        case 4:
            WeaponTemplate->SetWeaponRarity(WeaponRarityEnum::WRE_Legendary);
            break;
        default:
            WeaponTemplate->SetWeaponRarity(WeaponRarityEnum::WRE_Common);
            break;
        }
    }

    int fireModeTemp = FMath::RandRange(1, 3);
    if (WeaponTemplate->GetWeaponFiremode() == WeaponFireModeEnum::WFME_Invalid)
    {
        switch (fireModeTemp)
        {
        case 1:
            WeaponTemplate->SetWeaponFireMode(WeaponFireModeEnum::WFME_Single);
            break;
        case 2:
            WeaponTemplate->SetWeaponFireMode(WeaponFireModeEnum::WFME_Burst);
            break;
        case 3:
            WeaponTemplate->SetWeaponFireMode(WeaponFireModeEnum::WFME_Auto);
            break;
        default:
            WeaponTemplate->SetWeaponFireMode(WeaponFireModeEnum::WFME_Single);
            break;
        }
    }
   


    WeaponTemplate->CompleteWeaponAssembly();
}

//Logic to generate a shotgun for weapon template
    //-creates new parts for a shotgun to attach to template
void AWeaponGeneratorActor::GenerateShotgun(AWeaponPickup* WeaponTemplate)
{
    if (ShotgunAmmoType != ShotgunAmmoTypes::SAT_NumTypes)
        ammoType = (int)ShotgunAmmoType;
    else
    {
        TArray<int> listedValues = { m_shotgunBallisticWeaponChance , m_shotgunSoundWaveWeaponChance };
        int maxValue = listedValues[0] + listedValues[1];
        int randomVal = FMath::RandRange(1, maxValue);
        for (int i = 0; i < listedValues.Num(); i++)
        {
            randomVal -= listedValues[i];
            if (randomVal <= 0)
            {
                ammoType = i;
                break;
            }
        }
        //ammoType = FMath::RandRange(0, (int)ShotgunAmmoTypes::SAT_NumTypes - 1);
    }

    //ammoType = FMath::RandRange(0, (int)ShotgunAmmoTypes::SAT_NumTypes - 1);
    //ammoType = (int)ShotgunAmmoTypes::SAT_ShockWave;

    int RandY = FMath::RandRange(-400, 400);
    AWeaponPartActor* TempReceiver = GetWorld()->SpawnActor<AWeaponPartActor>
        //(m_shotgunReceiverParts[FMath::RandRange(0, m_shotgunReceiverParts.Num() - 1)], FVector(0.f, RandY, 190.f), FRotator(0, 0, 0));
        (GetRandomPart(WeaponTypeVariant::WTV_Shotgun, WeaponPartCategories::WPC_Receiver, 0), FVector(0.f, RandY, 190.f), FRotator(0, 0, 0));
    TempReceiver->AttachToActor(WeaponTemplate, FAttachmentTransformRules::SnapToTargetNotIncludingScale);

    AWeaponPartActor* TempBarrel = GetWorld()->SpawnActor<AWeaponPartActor>
        //(m_shotgunBarrelParts[FMath::RandRange(0, m_shotgunBarrelParts.Num() - 1)], FVector(0.f, RandY, 190.f), FRotator(0, 0, 0));
        (GetRandomPart(WeaponTypeVariant::WTV_Shotgun, WeaponPartCategories::WPC_Barrel, ammoType), FVector(0.f, RandY, 190.f), FRotator(0, 0, 0));
    TempBarrel->AttachToActor(TempReceiver, FAttachmentTransformRules::SnapToTargetNotIncludingScale, FName("Barrel_Socket"));

    AWeaponPartActor* TempTube = GetWorld()->SpawnActor<AWeaponPartActor>
        //(m_shotgunTubeParts[FMath::RandRange(0, m_shotgunTubeParts.Num() - 1)], FVector(0.f, RandY, 190.f), FRotator(0, 0, 0));
        (GetRandomPart(WeaponTypeVariant::WTV_Shotgun, WeaponPartCategories::WPC_Magazine, 0), FVector(0.f, RandY, 190.f), FRotator(0, 0, 0));
    TempTube->AttachToActor(TempReceiver, FAttachmentTransformRules::SnapToTargetNotIncludingScale, FName("Tube_Socket"));

    AWeaponPartActor* TempPump = GetWorld()->SpawnActor<AWeaponPartActor>
        //(m_shotgunPumpParts[FMath::RandRange(0, m_shotgunPumpParts.Num() - 1)], FVector(0.f, RandY, 190.f), FRotator(0, 0, 0));
        (GetRandomPart(WeaponTypeVariant::WTV_Shotgun, WeaponPartCategories::WPC_Accessory, 0), FVector(0.f, RandY, 190.f), FRotator(0, 0, 0));
    TempPump->AttachToActor(TempReceiver, FAttachmentTransformRules::SnapToTargetNotIncludingScale, FName("Pump_Socket"));

    AWeaponPartActor* TempStock = GetWorld()->SpawnActor<AWeaponPartActor>
        //(m_shotgunStockParts[FMath::RandRange(0, m_shotgunStockParts.Num() - 1)], FVector(0.f, RandY, 190.f), FRotator(0, 0, 0));
        (GetRandomPart(WeaponTypeVariant::WTV_Shotgun, WeaponPartCategories::WPC_ErgoEnhancer, 0), FVector(0.f, RandY, 190.f), FRotator(0, 0, 0));
    TempStock->AttachToActor(TempReceiver, FAttachmentTransformRules::SnapToTargetNotIncludingScale, FName("Stock_Socket"));


    //Release 2 --TEMP--
    //WeaponTemplate->SetOnProjectileStartEffect(TempStock->GetProjectileStartEffect());
    //Release 2 --TEMP--
    //WeaponTemplate->SetInFlightProjectileEffect(TempBarrel->GetProjectileInFlightEffect());
    //Release 2 --TEMP--
    //WeaponTemplate->SetOnProjectileHitEffect(TempTube->GetProjectileOnHitEffect());

    WeaponTemplate->AddNewWeaponPartToWeaponPartArray(TempReceiver);
    WeaponTemplate->AddNewWeaponPartToWeaponPartArray(TempBarrel);
    WeaponTemplate->AddNewWeaponPartToWeaponPartArray(TempTube);
    WeaponTemplate->AddNewWeaponPartToWeaponPartArray(TempPump);
    WeaponTemplate->AddNewWeaponPartToWeaponPartArray(TempStock);

    WeaponTemplate->RegisterMesh(Cast<USkeletalMeshComponent>(TempReceiver->GetRootComponent()));
    WeaponTemplate->RegisterMesh(Cast<USkeletalMeshComponent>(TempBarrel->GetRootComponent()));
    WeaponTemplate->RegisterMesh(Cast<USkeletalMeshComponent>(TempTube->GetRootComponent()));
    WeaponTemplate->RegisterMesh(Cast<USkeletalMeshComponent>(TempPump->GetRootComponent()));
    WeaponTemplate->RegisterMesh(Cast<USkeletalMeshComponent>(TempStock->GetRootComponent()));

    WeaponTemplate->SetPickupType(InventoryTabEnum::ITE_Shotguns);
    WeaponTemplate->SetWeaponType(WeaponTypeEnum::WTE_Shotgun);

    //Randomization for the weapon rarity multiplier
    int rarityTemp = FMath::RandRange(1, 4);
    if (WeaponTemplate->GetWeaponRarity() == WeaponRarityEnum::WRE_Invalid)
    {
        switch (rarityTemp)
        {
        case 1:
            WeaponTemplate->SetWeaponRarity(WeaponRarityEnum::WRE_Common);
            break;
        case 2:
            WeaponTemplate->SetWeaponRarity(WeaponRarityEnum::WRE_Uncommon);
            break;
        case 3:
            WeaponTemplate->SetWeaponRarity(WeaponRarityEnum::WRE_Rare);
            break;
        case 4:
            WeaponTemplate->SetWeaponRarity(WeaponRarityEnum::WRE_Legendary);
            break;
        default:
            WeaponTemplate->SetWeaponRarity(WeaponRarityEnum::WRE_Common);
            break;
        }
    }
    WeaponTemplate->SetWeaponFireMode(WeaponFireModeEnum::WFME_Single);

    generalAmmoType = 0;

    switch (ammoType)
    {
    case (int)ShotgunAmmoTypes::SAT_Ballistic:
        generalAmmoType = (int)AmmoTypes::AT_Ballistic;
        break;
    case (int)ShotgunAmmoTypes::SAT_ShockWave:
        generalAmmoType = (int)AmmoTypes::AT_ShockWave;
        break;
    default:
        generalAmmoType = (int)AmmoTypes::AT_Ballistic;
        break;
    }

    WeaponTemplate->SetProjectileType(generalAmmoType);
    WeaponTemplate->CompleteWeaponAssembly();
}


void AWeaponGeneratorActor::InitializeWeaponSetupArrays()
{
    Weapons.Init(TArray<TArray<TArray<TSubclassOf<class AWeaponPartActor>>>>(), (int)WeaponTypeVariant::WTV_NumTypes);

//Initialization for Carbine Rifles
#pragma region
    Weapons[(int)WeaponTypeVariant::WTV_RifleCarbine].Init(TArray<TArray<TSubclassOf<class AWeaponPartActor>>>(), (int)WeaponPartCategories::WPC_NumTypes);
    CreateNewWeaponStructureLayer(WeaponTypeVariant::WTV_RifleCarbine, WeaponPartCategories::WPC_Receiver, 1);                                                  //Receiver
    CreateNewWeaponStructureLayer(WeaponTypeVariant::WTV_RifleCarbine, WeaponPartCategories::WPC_ErgoEnhancer, 1);                                              //Stock
    CreateNewWeaponStructureLayer(WeaponTypeVariant::WTV_RifleCarbine, WeaponPartCategories::WPC_Barrel, (int)RifleCarbineAmmoTypes::RCAT_NumTypes);            //Barrel
    CreateNewWeaponStructureLayer(WeaponTypeVariant::WTV_RifleCarbine, WeaponPartCategories::WPC_Magazine, (int)RifleCarbineAmmoTypes::RCAT_NumTypes);          //Magazine
    CreateNewWeaponStructureLayer(WeaponTypeVariant::WTV_RifleCarbine, WeaponPartCategories::WPC_Accessory, 1);                                                 //Optic
#pragma endregion

//Initialization for PDW Rifles
#pragma region
    Weapons[(int)WeaponTypeVariant::WTV_RiflePDW].Init(TArray<TArray<TSubclassOf<class AWeaponPartActor>>>(), (int)WeaponPartCategories::WPC_NumTypes);
    CreateNewWeaponStructureLayer(WeaponTypeVariant::WTV_RiflePDW, WeaponPartCategories::WPC_Receiver, 1);                                                      //Receiver
    CreateNewWeaponStructureLayer(WeaponTypeVariant::WTV_RiflePDW, WeaponPartCategories::WPC_ErgoEnhancer, 1);                                                  //Stock
    CreateNewWeaponStructureLayer(WeaponTypeVariant::WTV_RiflePDW, WeaponPartCategories::WPC_Barrel, (int)RiflePDWAmmoTypes::RPAT_NumTypes);                    //Barrel
    CreateNewWeaponStructureLayer(WeaponTypeVariant::WTV_RiflePDW, WeaponPartCategories::WPC_Magazine, (int)RiflePDWAmmoTypes::RPAT_NumTypes);                  //Magazine
    CreateNewWeaponStructureLayer(WeaponTypeVariant::WTV_RiflePDW, WeaponPartCategories::WPC_Accessory, 1);                                                     //Grip
#pragma endregion

//Initialization for Shotguns
#pragma region
    Weapons[(int)WeaponTypeVariant::WTV_Shotgun].Init(TArray<TArray<TSubclassOf<class AWeaponPartActor>>>(), (int)WeaponPartCategories::WPC_NumTypes);
    CreateNewWeaponStructureLayer(WeaponTypeVariant::WTV_Shotgun, WeaponPartCategories::WPC_Receiver, 1);                                                       //Receiver
    CreateNewWeaponStructureLayer(WeaponTypeVariant::WTV_Shotgun, WeaponPartCategories::WPC_ErgoEnhancer, 1);                                                   //Stock
    CreateNewWeaponStructureLayer(WeaponTypeVariant::WTV_Shotgun, WeaponPartCategories::WPC_Barrel, (int)ShotgunAmmoTypes::SAT_NumTypes);                       //Barrel
    CreateNewWeaponStructureLayer(WeaponTypeVariant::WTV_Shotgun, WeaponPartCategories::WPC_Magazine, 1);                                                       //Magazine   //Tube
    CreateNewWeaponStructureLayer(WeaponTypeVariant::WTV_Shotgun, WeaponPartCategories::WPC_Accessory, 1);                                                      //Pump
#pragma endregion

//Initialization for Pistol
#pragma region
    Weapons[(int)WeaponTypeVariant::WTV_Pistol].Init(TArray<TArray<TSubclassOf<class AWeaponPartActor>>>(), (int)WeaponPartCategories::WPC_NumTypes);
    CreateNewWeaponStructureLayer(WeaponTypeVariant::WTV_Pistol, WeaponPartCategories::WPC_Receiver, 1);                                                        //Frame
    CreateNewWeaponStructureLayer(WeaponTypeVariant::WTV_Pistol, WeaponPartCategories::WPC_ErgoEnhancer, 1);                                                    //Trigga Guard
    CreateNewWeaponStructureLayer(WeaponTypeVariant::WTV_Pistol, WeaponPartCategories::WPC_Barrel, (int)PistolAmmoTypes::PAT_NumTypes);                         //Barrel
    CreateNewWeaponStructureLayer(WeaponTypeVariant::WTV_Pistol, WeaponPartCategories::WPC_Magazine, (int)PistolAmmoTypes::PAT_NumTypes);                       //Magazine
    CreateNewWeaponStructureLayer(WeaponTypeVariant::WTV_Pistol, WeaponPartCategories::WPC_Accessory, 1);                                                       //Slide
#pragma endregion
}

TSubclassOf<class AWeaponPartActor> AWeaponGeneratorActor::GetRandomPart(WeaponTypeVariant WeaponType, WeaponPartCategories WeaponPartCategory, int WeaponAmmoType)
{
    int randPartNum = FMath::RandRange(0, Weapons[(int)WeaponType][(int)WeaponPartCategory][WeaponAmmoType].Num() - 1);
    if (Weapons[(int)WeaponType][(int)WeaponPartCategory][WeaponAmmoType].IsValidIndex(randPartNum))
    {
        return Weapons[(int)WeaponType][(int)WeaponPartCategory][WeaponAmmoType][randPartNum];
    }
    else
    {
        GEngine->AddOnScreenDebugMessage(0, 3.f, FColor::Red, "NO VALID PART");
        return nullptr;
    }
}

void AWeaponGeneratorActor::CreateNewWeaponStructureLayer(WeaponTypeVariant weaponType, WeaponPartCategories partCategory, int numToInitialize)
{
    Weapons[(int)weaponType][(int)partCategory].Init(TArray<TSubclassOf<class AWeaponPartActor>>(), numToInitialize);
}

void AWeaponGeneratorActor::SERVER_PopulateWeaponPartArrays_Implementation()
{
    //Setup registry module
    FAssetRegistryModule& AssetRegistryModule = FModuleManager::LoadModuleChecked<FAssetRegistryModule>("AssetRegistry");

    //Holds the path to all the weapon type folders
    FString TypesOfWeaponPath = "/Game/Art/3D_Models/Non_Organic/Guns/";

    //Sets the part of the path to the specified weapon
    FString WeaponTypePath = "Rifle/";//Default Weapon Type

    //Array that holds the part of the path to the weapon parts
    TArray<FString> RifleWeaponTypePartPath = { "Barrel", "Magazine", "Optic", "Receiver", "Stock" };
    TArray<FString> PistolWeaponTypePartPath = { "Barrel", "Frame", "Magazine", "Slide", "TriggerGuard" };
    TArray<FString> ShotgunWeaponTypePartPath = { "Barrel", "Magazine", "Pump", "Receiver", "Stock" };
    TArray<FString> PDWRifleWeaponTypePartPath = { "PDW_Barrels", "PDW_Magazines", "PDW_Foregrips", "PDW_Receiver", "PDW_Stocks" };

    //Asset data array to hold the results of what gets found
    TArray<FAssetData> AssetData;

    //Filter to sort out criteria for what type of things will be searched for
    FARFilter dataFilter;

    //Telling the filter to only look for objects with the class name of Blueprint
    dataFilter.ClassNames.Add("Blueprint");

    //Filling Rifle part arrays
#pragma region
    WeaponTypePath = "Rifle/";
    for (int i = 0; i < RifleWeaponTypePartPath.Num(); i++)
    {
        FString PartPath = TypesOfWeaponPath + WeaponTypePath + RifleWeaponTypePartPath[i];
        //FString PartPath = "/Game/Art/3D_Models/Non_Organic/Guns/Rifle/Barrel";
        dataFilter.PackagePaths.Add(FName(*PartPath));
        AssetRegistryModule.Get().GetAssets(dataFilter, AssetData);
        if (AssetData.Num() > 0)
        {
            if (RifleWeaponTypePartPath[i] == "Barrel")
            {
                for (int j = 0; j < AssetData.Num(); j++)
                {
                    //Initial attempt to load object from FObjectFinder but StaticLoadObject seems to work, not sure of difference but will look into later
                    //m_rifleBarrelWeaponParts.Add(Cast<UClass>(StaticLoadObject(UObject::StaticClass(), nullptr, *("Blueprint'" + AssetData[j].ObjectPath.ToString() + "_C'")    )));
                    //m_rifleBarrelWeaponParts.Add(StaticLoadClass(AWeaponPartActor::StaticClass(), nullptr, *("Blueprint'" + AssetData[j].ObjectPath.ToString() + "_C'")));


                    if (AssetData[j].ObjectPath.ToString().Contains(TEXT("laser")))
                    {
                        Weapons[(int)WeaponTypeVariant::WTV_RifleCarbine][(int)WeaponPartCategories::WPC_Barrel][(int)RifleCarbineAmmoTypes::RCAT_Laser].
                            Add(StaticLoadClass(AWeaponPartActor::StaticClass(), nullptr,*("Blueprint'" + AssetData[j].ObjectPath.ToString() + "_C'")));
                    }
                    else if (AssetData[j].ObjectPath.ToString().Contains(TEXT("balloon")))
                    {
                        Weapons[(int)WeaponTypeVariant::WTV_RifleCarbine][(int)WeaponPartCategories::WPC_Barrel][(int)RifleCarbineAmmoTypes::RCAT_Balloon].
                            Add(StaticLoadClass(AWeaponPartActor::StaticClass(), nullptr, *("Blueprint'" + AssetData[j].ObjectPath.ToString() + "_C'")));
                    }
                    else if (AssetData[j].ObjectPath.ToString().Contains(TEXT("crystal")))
                    {
                        Weapons[(int)WeaponTypeVariant::WTV_RifleCarbine][(int)WeaponPartCategories::WPC_Barrel][(int)RifleCarbineAmmoTypes::RCAT_Crystal].
                            Add(StaticLoadClass(AWeaponPartActor::StaticClass(), nullptr, *("Blueprint'" + AssetData[j].ObjectPath.ToString() + "_C'")));
                    }
                    else
                    {
                        Weapons[(int)WeaponTypeVariant::WTV_RifleCarbine][(int)WeaponPartCategories::WPC_Barrel][(int)RifleCarbineAmmoTypes::RCAT_Ballistic].
                            Add(StaticLoadClass(AWeaponPartActor::StaticClass(), nullptr, *("Blueprint'" + AssetData[j].ObjectPath.ToString() + "_C'")));
                    }
                }
            }
            if (RifleWeaponTypePartPath[i] == "Magazine")
            {
                for (int j = 0; j < AssetData.Num(); j++)
                {
                    //m_rifleReceiverWeaponParts.Add(Cast<UClass>(StaticLoadObject(UObject::StaticClass(), nullptr, *("Blueprint'" + AssetData[j].ObjectPath.ToString() + "_C'")  )));
                    //m_rifleMagazineWeaponParts.Add(StaticLoadClass(AWeaponPartActor::StaticClass(), nullptr, *("Blueprint'" + AssetData[j].ObjectPath.ToString() + "_C'")));

                    if (AssetData[j].ObjectPath.ToString().Contains(TEXT("laser")))
                    {
                        Weapons[(int)WeaponTypeVariant::WTV_RifleCarbine][(int)WeaponPartCategories::WPC_Magazine][(int)RifleCarbineAmmoTypes::RCAT_Laser].
                            Add(StaticLoadClass(AWeaponPartActor::StaticClass(), nullptr, *("Blueprint'" + AssetData[j].ObjectPath.ToString() + "_C'")));
                    }
                    else if (AssetData[j].ObjectPath.ToString().Contains(TEXT("balloon")))
                    {
                        Weapons[(int)WeaponTypeVariant::WTV_RifleCarbine][(int)WeaponPartCategories::WPC_Magazine][(int)RifleCarbineAmmoTypes::RCAT_Balloon].
                            Add(StaticLoadClass(AWeaponPartActor::StaticClass(), nullptr, *("Blueprint'" + AssetData[j].ObjectPath.ToString() + "_C'")));
                    }
                    else if (AssetData[j].ObjectPath.ToString().Contains(TEXT("crystal")))
                    {
                        Weapons[(int)WeaponTypeVariant::WTV_RifleCarbine][(int)WeaponPartCategories::WPC_Magazine][(int)RifleCarbineAmmoTypes::RCAT_Crystal].
                            Add(StaticLoadClass(AWeaponPartActor::StaticClass(), nullptr, *("Blueprint'" + AssetData[j].ObjectPath.ToString() + "_C'")));
                    }
                    else
                    {
                        Weapons[(int)WeaponTypeVariant::WTV_RifleCarbine][(int)WeaponPartCategories::WPC_Magazine][(int)RifleCarbineAmmoTypes::RCAT_Ballistic].
                            Add(StaticLoadClass(AWeaponPartActor::StaticClass(), nullptr, *("Blueprint'" + AssetData[j].ObjectPath.ToString() + "_C'")));
                    }
                }
            }
            if (RifleWeaponTypePartPath[i] == "Receiver")
            {
                for (int j = 0; j < AssetData.Num(); j++)
                {
                    //m_rifleReceiverWeaponParts.Add(Cast<UClass>(StaticLoadObject(UObject::StaticClass(), nullptr, *("Blueprint'" + AssetData[j].ObjectPath.ToString() + "_C'")  )));
                    //m_rifleReceiverWeaponParts.Add(StaticLoadClass(AWeaponPartActor::StaticClass(), nullptr, *("Blueprint'" + AssetData[j].ObjectPath.ToString() + "_C'")));
                    //m_rifleReceiver = StaticLoadClass(AWeaponPartActor::StaticClass(), nullptr, *("Blueprint'" + AssetData[j].ObjectPath.ToString() + "_C'"));
                    Weapons[(int)WeaponTypeVariant::WTV_RifleCarbine][(int)WeaponPartCategories::WPC_Receiver][(int)RifleCarbineAmmoTypes::RCAT_Ballistic].
                        Add(StaticLoadClass(AWeaponPartActor::StaticClass(), nullptr, *("Blueprint'" + AssetData[j].ObjectPath.ToString() + "_C'")));
                }
            }
            if (RifleWeaponTypePartPath[i] == "Optic")
            {
                for (int j = 0; j < AssetData.Num(); j++)
                {
                    //m_rifleReceiverWeaponParts.Add(Cast<UClass>(StaticLoadObject(UObject::StaticClass(), nullptr, *("Blueprint'" + AssetData[j].ObjectPath.ToString() + "_C'")  )));
                    //m_rifleOpticWeaponParts.Add(StaticLoadClass(AWeaponPartActor::StaticClass(), nullptr, *("Blueprint'" + AssetData[j].ObjectPath.ToString() + "_C'")));

                    Weapons[(int)WeaponTypeVariant::WTV_RifleCarbine][(int)WeaponPartCategories::WPC_Accessory][(int)RifleCarbineAmmoTypes::RCAT_Ballistic].
                        Add(StaticLoadClass(AWeaponPartActor::StaticClass(), nullptr, *("Blueprint'" + AssetData[j].ObjectPath.ToString() + "_C'")));
                }
            }
            if (RifleWeaponTypePartPath[i] == "Stock")
            {
                for (int j = 0; j < AssetData.Num(); j++)
                {
                    //m_rifleStockWeaponParts.Add(Cast<UClass>(StaticLoadObject(UObject::StaticClass(), nullptr, *("Blueprint'" + AssetData[j].ObjectPath.ToString() + "_C'") )));
                    //m_rifleStockWeaponParts.Add(StaticLoadClass(AWeaponPartActor::StaticClass(), nullptr, *("Blueprint'" + AssetData[j].ObjectPath.ToString() + "_C'")));

                    Weapons[(int)WeaponTypeVariant::WTV_RifleCarbine][(int)WeaponPartCategories::WPC_ErgoEnhancer][(int)RifleCarbineAmmoTypes::RCAT_Ballistic].
                        Add(StaticLoadClass(AWeaponPartActor::StaticClass(), nullptr, *("Blueprint'" + AssetData[j].ObjectPath.ToString() + "_C'")));
                }
            }
        }
        AssetData.Empty();
        dataFilter.PackagePaths.Empty();
    }
#pragma  endregion

    //Filling Shotgun part arrays
#pragma region
    WeaponTypePath = "Shotgun/";
    for (int i = 0; i < ShotgunWeaponTypePartPath.Num(); i++)
    {
        FString PartPath = TypesOfWeaponPath + WeaponTypePath + ShotgunWeaponTypePartPath[i];
        dataFilter.PackagePaths.Add(FName(*PartPath));
        AssetRegistryModule.Get().GetAssets(dataFilter, AssetData);
        if (AssetData.Num() > 0)
        {
            if (ShotgunWeaponTypePartPath[i] == "Barrel")
            {
                for (int j = 0; j < AssetData.Num(); j++)
                {
                    //m_shotgunBarrelParts.Add(StaticLoadClass(AWeaponPartActor::StaticClass(), nullptr, *("Blueprint'" + AssetData[j].ObjectPath.ToString() + "_C'")));

                    //Weapons[(int)WeaponTypeVariant::WTV_Shotgun][(int)WeaponPartCategories::WPC_Barrel][(int)ShotgunAmmoTypes::SAT_Ballistic].
                    //    Add(StaticLoadClass(AWeaponPartActor::StaticClass(), nullptr, *("Blueprint'" + AssetData[j].ObjectPath.ToString() + "_C'")));

                    if (AssetData[j].ObjectPath.ToString().Contains(TEXT("sonic")))
                    {
                        Weapons[(int)WeaponTypeVariant::WTV_Shotgun][(int)WeaponPartCategories::WPC_Barrel][(int)ShotgunAmmoTypes::SAT_ShockWave].
                            Add(StaticLoadClass(AWeaponPartActor::StaticClass(), nullptr, *("Blueprint'" + AssetData[j].ObjectPath.ToString() + "_C'")));
                    }
                    else
                    {
                        Weapons[(int)WeaponTypeVariant::WTV_Shotgun][(int)WeaponPartCategories::WPC_Barrel][(int)ShotgunAmmoTypes::SAT_Ballistic].
                            Add(StaticLoadClass(AWeaponPartActor::StaticClass(), nullptr, *("Blueprint'" + AssetData[j].ObjectPath.ToString() + "_C'")));
                    }


                }
            }
            if (ShotgunWeaponTypePartPath[i] == "Magazine")
            {
                for (int j = 0; j < AssetData.Num(); j++)
                {
                    //m_shotgunTubeParts.Add(StaticLoadClass(AWeaponPartActor::StaticClass(), nullptr, *("Blueprint'" + AssetData[j].ObjectPath.ToString() + "_C'")));
                    Weapons[(int)WeaponTypeVariant::WTV_Shotgun][(int)WeaponPartCategories::WPC_Magazine][(int)ShotgunAmmoTypes::SAT_Ballistic].
                        Add(StaticLoadClass(AWeaponPartActor::StaticClass(), nullptr, *("Blueprint'" + AssetData[j].ObjectPath.ToString() + "_C'")));
                }
            }
            if (ShotgunWeaponTypePartPath[i] == "Pump")
            {
                for (int j = 0; j < AssetData.Num(); j++)
                {
                    //m_shotgunPumpParts.Add(StaticLoadClass(AWeaponPartActor::StaticClass(), nullptr, *("Blueprint'" + AssetData[j].ObjectPath.ToString() + "_C'")));
                    Weapons[(int)WeaponTypeVariant::WTV_Shotgun][(int)WeaponPartCategories::WPC_Accessory][(int)ShotgunAmmoTypes::SAT_Ballistic].
                        Add(StaticLoadClass(AWeaponPartActor::StaticClass(), nullptr, *("Blueprint'" + AssetData[j].ObjectPath.ToString() + "_C'")));
                }
            }
            if (ShotgunWeaponTypePartPath[i] == "Receiver")
            {
                for (int j = 0; j < AssetData.Num(); j++)
                {
                    //m_shotgunReceiverParts.Add(StaticLoadClass(AWeaponPartActor::StaticClass(), nullptr, *("Blueprint'" + AssetData[j].ObjectPath.ToString() + "_C'")));
                    Weapons[(int)WeaponTypeVariant::WTV_Shotgun][(int)WeaponPartCategories::WPC_Receiver][(int)ShotgunAmmoTypes::SAT_Ballistic].
                        Add(StaticLoadClass(AWeaponPartActor::StaticClass(), nullptr, *("Blueprint'" + AssetData[j].ObjectPath.ToString() + "_C'")));
                }
            }
            if (ShotgunWeaponTypePartPath[i] == "Stock")
            {
                for (int j = 0; j < AssetData.Num(); j++)
                {
                    //m_shotgunStockParts.Add(StaticLoadClass(AWeaponPartActor::StaticClass(), nullptr, *("Blueprint'" + AssetData[j].ObjectPath.ToString() + "_C'")));
                    Weapons[(int)WeaponTypeVariant::WTV_Shotgun][(int)WeaponPartCategories::WPC_ErgoEnhancer][(int)ShotgunAmmoTypes::SAT_Ballistic].
                        Add(StaticLoadClass(AWeaponPartActor::StaticClass(), nullptr, *("Blueprint'" + AssetData[j].ObjectPath.ToString() + "_C'")));
                }
            }
        }
        AssetData.Empty();
        dataFilter.PackagePaths.Empty();
    }
#pragma  endregion

    //Filling Pistol part arrays
#pragma region
    WeaponTypePath = "Pistol/";
    for (int i = 0; i < PistolWeaponTypePartPath.Num(); i++)
    {
        FString PartPath = TypesOfWeaponPath + WeaponTypePath + PistolWeaponTypePartPath[i];
        //FString PartPath = "/Game/Art/3D_Models/Non_Organic/Guns/Rifle/Barrel";
        dataFilter.PackagePaths.Add(FName(*PartPath));
        AssetRegistryModule.Get().GetAssets(dataFilter, AssetData);
        if (AssetData.Num() > 0)
        {
            if (PistolWeaponTypePartPath[i] == "Barrel")
            {
                for (int j = 0; j < AssetData.Num(); j++)
                {
                   
                    //m_pistolBarrelParts.Add(StaticLoadClass(AWeaponPartActor::StaticClass(), nullptr, *("Blueprint'" + AssetData[j].ObjectPath.ToString() + "_C'")));
                    Weapons[(int)WeaponTypeVariant::WTV_Pistol][(int)WeaponPartCategories::WPC_Barrel][(int)ShotgunAmmoTypes::SAT_Ballistic].
                        Add(StaticLoadClass(AWeaponPartActor::StaticClass(), nullptr, *("Blueprint'" + AssetData[j].ObjectPath.ToString() + "_C'")));
                }
            }
            if (PistolWeaponTypePartPath[i] == "Frame")
            {
                for (int j = 0; j < AssetData.Num(); j++)
                {
                    //m_pistolFrameParts.Add(StaticLoadClass(AWeaponPartActor::StaticClass(), nullptr, *("Blueprint'" + AssetData[j].ObjectPath.ToString() + "_C'")));
                    Weapons[(int)WeaponTypeVariant::WTV_Pistol][(int)WeaponPartCategories::WPC_Receiver][(int)ShotgunAmmoTypes::SAT_Ballistic].
                        Add(StaticLoadClass(AWeaponPartActor::StaticClass(), nullptr, *("Blueprint'" + AssetData[j].ObjectPath.ToString() + "_C'")));
                }
            }
            if (PistolWeaponTypePartPath[i] == "Magazine")
            {
                for (int j = 0; j < AssetData.Num(); j++)
                {
                   // m_pistolMagazineParts.Add(StaticLoadClass(AWeaponPartActor::StaticClass(), nullptr, *("Blueprint'" + AssetData[j].ObjectPath.ToString() + "_C'")));
                    Weapons[(int)WeaponTypeVariant::WTV_Pistol][(int)WeaponPartCategories::WPC_Magazine][(int)ShotgunAmmoTypes::SAT_Ballistic].
                        Add(StaticLoadClass(AWeaponPartActor::StaticClass(), nullptr, *("Blueprint'" + AssetData[j].ObjectPath.ToString() + "_C'")));
                }
            }
            if (PistolWeaponTypePartPath[i] == "Slide")
            {
                for (int j = 0; j < AssetData.Num(); j++)
                {
                    //m_pistolSlideParts.Add(StaticLoadClass(AWeaponPartActor::StaticClass(), nullptr, *("Blueprint'" + AssetData[j].ObjectPath.ToString() + "_C'")));
                    Weapons[(int)WeaponTypeVariant::WTV_Pistol][(int)WeaponPartCategories::WPC_Accessory][(int)ShotgunAmmoTypes::SAT_Ballistic].
                        Add(StaticLoadClass(AWeaponPartActor::StaticClass(), nullptr, *("Blueprint'" + AssetData[j].ObjectPath.ToString() + "_C'")));
                }
            }
            if (PistolWeaponTypePartPath[i] == "TriggerGuard")
            {
                for (int j = 0; j < AssetData.Num(); j++)
                {
                    //m_pistolTriggerGuardParts.Add(StaticLoadClass(AWeaponPartActor::StaticClass(), nullptr, *("Blueprint'" + AssetData[j].ObjectPath.ToString() + "_C'")));
                    Weapons[(int)WeaponTypeVariant::WTV_Pistol][(int)WeaponPartCategories::WPC_ErgoEnhancer][(int)ShotgunAmmoTypes::SAT_Ballistic].
                        Add(StaticLoadClass(AWeaponPartActor::StaticClass(), nullptr, *("Blueprint'" + AssetData[j].ObjectPath.ToString() + "_C'")));
                }
            }
        }
        AssetData.Empty();
        dataFilter.PackagePaths.Empty();
    }
#pragma  endregion

    //Filling PDW Part Arrays
#pragma region
    WeaponTypePath = "Rifle/";
    for (int i = 0; i < PDWRifleWeaponTypePartPath.Num(); i++)
    {
        FString PartPath = TypesOfWeaponPath + WeaponTypePath + PDWRifleWeaponTypePartPath[i];
        dataFilter.PackagePaths.Add(FName(*PartPath));
        AssetRegistryModule.Get().GetAssets(dataFilter, AssetData);
        if (AssetData.Num() > 0)
        {
            if (PDWRifleWeaponTypePartPath[i] == "PDW_Barrels")
            {
                for (int j = 0; j < AssetData.Num(); j++)
                {
                    Weapons[(int)WeaponTypeVariant::WTV_RiflePDW][(int)WeaponPartCategories::WPC_Barrel][(int)RiflePDWAmmoTypes::RPAT_Ballistic].
                        Add(StaticLoadClass(AWeaponPartActor::StaticClass(), nullptr, *("Blueprint'" + AssetData[j].ObjectPath.ToString() + "_C'")));
                }
            }
            if (PDWRifleWeaponTypePartPath[i] == "PDW_Magazines")
            {
                for (int j = 0; j < AssetData.Num(); j++)
                {
                    Weapons[(int)WeaponTypeVariant::WTV_RiflePDW][(int)WeaponPartCategories::WPC_Magazine][(int)RiflePDWAmmoTypes::RPAT_Ballistic].
                        Add(StaticLoadClass(AWeaponPartActor::StaticClass(), nullptr, *("Blueprint'" + AssetData[j].ObjectPath.ToString() + "_C'")));
                }
            }
            if (PDWRifleWeaponTypePartPath[i] == "PDW_Receiver")
            {
                for (int j = 0; j < AssetData.Num(); j++)
                {
                    Weapons[(int)WeaponTypeVariant::WTV_RiflePDW][(int)WeaponPartCategories::WPC_Receiver][(int)RiflePDWAmmoTypes::RPAT_Ballistic].
                        Add(StaticLoadClass(AWeaponPartActor::StaticClass(), nullptr, *("Blueprint'" + AssetData[j].ObjectPath.ToString() + "_C'")));
                }
            }
            if (PDWRifleWeaponTypePartPath[i] == "PDW_Foregrips")
            {
                for (int j = 0; j < AssetData.Num(); j++)
                {
                    Weapons[(int)WeaponTypeVariant::WTV_RiflePDW][(int)WeaponPartCategories::WPC_Accessory][(int)RiflePDWAmmoTypes::RPAT_Ballistic].
                        Add(StaticLoadClass(AWeaponPartActor::StaticClass(), nullptr, *("Blueprint'" + AssetData[j].ObjectPath.ToString() + "_C'")));
                }
            }
            if (PDWRifleWeaponTypePartPath[i] == "PDW_Stocks")
            {
                for (int j = 0; j < AssetData.Num(); j++)
                {
                    Weapons[(int)WeaponTypeVariant::WTV_RiflePDW][(int)WeaponPartCategories::WPC_ErgoEnhancer][(int)RiflePDWAmmoTypes::RPAT_Ballistic].
                        Add(StaticLoadClass(AWeaponPartActor::StaticClass(), nullptr, *("Blueprint'" + AssetData[j].ObjectPath.ToString() + "_C'")));
                }
            }
        }
        AssetData.Empty();
        dataFilter.PackagePaths.Empty();
    }
#pragma endregion

    //Getting Projectiles

    //Release 2 
    //Now only retrieves custom projectile that can have custom effects
    FString ProjectilePath = "/Game/Blueprints/GameObjects/Projectiles/TestProjectiles";
    dataFilter.PackagePaths.Add(FName(*ProjectilePath));
    AssetRegistryModule.Get().GetAssets(dataFilter, AssetData);
    if (AssetData.Num() > 0)
    {
        m_prototypeWeaponProjectile = StaticLoadClass(AProjectileActor::StaticClass(), nullptr, *("Blueprint'" + AssetData[0].ObjectPath.ToString() + "_C'"));
    }
    AssetData.Empty();
    dataFilter.PackagePaths.Empty();
}

// Called when the game starts or when spawned
void AWeaponGeneratorActor::BeginPlay()
{
	Super::BeginPlay();

    if (GetLocalRole() == ROLE_Authority)
    {
        InitializeWeaponSetupArrays();
        SERVER_PopulateWeaponPartArrays();
        //SERVER_PreGenerateBulletPool();
        SERVER_PreGenerateWeaponPool();
    }
}

bool AWeaponGeneratorActor::RandomChance(int BaseValue, int Multiplier)
{
    //((int)WeaponTemplate->GetWeaponRarity() + 1)
    int CustomStartEffectChance = BaseValue * Multiplier;
    int CustomRNG = FMath::RandRange(0, 100);
    bool GrantEffect = CustomRNG > CustomStartEffectChance ? false : true;

    return GrantEffect;
}

// Called every frame
void AWeaponGeneratorActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AWeaponGeneratorActor::PostLoad()
{
    Super::PostLoad();

}
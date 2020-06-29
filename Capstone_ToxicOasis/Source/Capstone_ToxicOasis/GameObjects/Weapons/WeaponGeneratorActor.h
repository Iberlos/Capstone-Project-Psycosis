// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "EnumMaster/EnumMaster.h"
#include "WeaponGeneratorActor.generated.h"

/****************FILL IN DETAILS*********************
Class: WeaponGeneratorActor
Author: LucasFelix
Created: october 2019
Last Modified: 4/feb/2020
Description: This is the player class used in the game, it is responsible for being a medium between all othr classes and the components in this class and its base class by means of interface functions. It also binds all the player input functions and allows for networked interaction of the players with the world.
References:
Change Log
***********FILL IN DETAILS ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
Date: unknown
Author: Luis Filipe Moraes
Verified By: Lucas Felix & Abdul Ali
Changes: Modified the PDW generation to give the weaponPickup the correct enums for weapon type and pickup type
--------------------------------------------------------
*************************************/
USTRUCT(BlueprintType)
struct FSoundsParticleFXData
{
    GENERATED_BODY()
public:
    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
        class USoundCue* Hit_S;
    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
        class USoundCue* HitCharacter_S;
    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
        class USoundCue* Beam_S;
    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
        class USoundCue* Fire_S;
    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
        class USoundCue* StartFiringEmpty_S;
    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
        class USoundCue* Reload_S;
    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
        class USoundCue* Equip_S;
    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
        class USoundCue* Unequip_S;
    //a_weapon->m_soundCues["Hit"].m_sound = null;	
    //a_weapon->m_soundCues["HitCharacter"].m_sound = null;	
    //a_weapon->m_soundCues["Beam"].m_sound = null;	
    //a_weapon->m_soundCues["Fire"].m_sound = null;	
    //a_weapon->m_soundCues["StartFiringEmpty"].m_sound = null;	
    //a_weapon->m_soundCues["Reload"].m_sound = null;	
    //a_weapon->m_soundCues["Equip"].m_sound = null;	
    //a_weapon->m_soundCues["Unequip"].m_sound = null;	
    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
        class UParticleSystem* Hit_P;
    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
        class UParticleSystem* HitCharacter_P;
    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
        class UParticleSystem* Beam_P;
    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
        class UParticleSystem* Fire_P;
    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
        class UParticleSystem* Explosion_P;
    //a_weapon->m_particleFXs["Hit"].m_sound = null;	
    //a_weapon->m_particleFXs["HitCharacter"].m_sound = null;	
    //a_weapon->m_particleFXs["Beam"].m_sound = null;	
    //a_weapon->m_particleFXs["Fire"].m_sound = null;	
    //a_weapon->m_particleFXs["StartFiringEmpty"].m_sound = null;	
    //a_weapon->m_particleFXs["Reload"].m_sound = null;	
    //a_weapon->m_particleFXs["Equip"].m_sound = null;	
    //a_weapon->m_particleFXs["Unequip"].m_sound = null;	
};
USTRUCT(BlueprintType)
struct FWeaponSoundsParticleFX
{
    GENERATED_BODY()
public:
    UPROPERTY(EditDefaultsOnly)
        TArray<FSoundsParticleFXData> Sounds_Particle_Data;
    FWeaponSoundsParticleFX()
    {
    }
    FWeaponSoundsParticleFX(WeaponTypeEnum type)
    {
        switch (type)
        {
        case WeaponTypeEnum::WTE_Pistol:
            for (int i = 0; i < (int)PistolAmmoTypes::PAT_NumTypes; i++)
            {
                Sounds_Particle_Data.Add(FSoundsParticleFXData());
            }
            break;
        case WeaponTypeEnum::WTE_Shotgun:
            for (int i = 0; i < (int)ShotgunAmmoTypes::SAT_NumTypes; i++)
            {
                Sounds_Particle_Data.Add(FSoundsParticleFXData());
            }
            break;
        case WeaponTypeEnum::WTE_Rifle:
            for (int i = 0; i < (int)RifleCarbineAmmoTypes::RCAT_NumTypes; i++)
            {
                Sounds_Particle_Data.Add(FSoundsParticleFXData());
            }
            break;
        case WeaponTypeEnum::WTE_PDW:
            for (int i = 0; i < (int)RiflePDWAmmoTypes::RPAT_NumTypes; i++)
            {
                Sounds_Particle_Data.Add(FSoundsParticleFXData());
            }
            break;
        }
    }
};

UCLASS()
class CAPSTONE_TOXICOASIS_API AWeaponGeneratorActor : public AActor
{
	GENERATED_BODY()
	
public:	
	//FRIEND DECLARATION AVOID DECLARING A WHOLE CLASS AS A FRIEND. USE THE FRIEND FUNCTION DECLARATION INSTEAD.
    void SetWeaponParticlesAndSound(AWeaponPickup* a_weapon); //This function is a friend of the weapon pickup class. Inside this function you will have private access to all its features. BE VERY CAREFULL USE IT ONLY FOR THE ORINAL PURPOSE.
	// Sets default values for this actor's properties
	AWeaponGeneratorActor();

    UFUNCTION(BlueprintCallable, Category = "Weapon Generation")
        void GenerateWeaponFromTemplate(class AWeaponPickup* WeaponTemplate);

    virtual void PreInitializeComponents() override;
    UFUNCTION(Server, Reliable, WithValidation)
        void SERVER_PreGenerateWeaponPool();
    bool SERVER_PreGenerateWeaponPool_Validate() { return true; }
    UFUNCTION(Server, Reliable, WithValidation)
        void SERVER_PreGenerateBulletPool();
    bool SERVER_PreGenerateBulletPool_Validate() { return true; }

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

    UFUNCTION(Server, Reliable, WithValidation)
        void SERVER_PopulateWeaponPartArrays();
    bool SERVER_PopulateWeaponPartArrays_Validate() { return true; };

    void GenerateRifle(class AWeaponPickup* WeaponTemplate);
    void GeneratePDWRifle(class AWeaponPickup* WeaponTemplate);
    void GeneratePistol(class AWeaponPickup* WeaponTemplate);
    void GenerateShotgun(class AWeaponPickup* WeaponTemplate);

    //Release 2
    TArray< //Holds diff weapon types
            TArray< //Holds diff weapon parts
                    TArray<//Holds Diff ammo types for part type
                            TArray<TSubclassOf<class AWeaponPartActor>>>>> //Holds the actors per ammo type
        Weapons;
    
    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
        TArray<FWeaponSoundsParticleFX> m_Sounds_ParticleFX_Arr;

    UPROPERTY(EditDefaultsOnly)
        int m_NumberOfWeaponsToSpawn = 100;

    UPROPERTY(EditDefaultsOnly, Category = "Weapon Rarity Blueprints")
        TSubclassOf<class AWeaponPickup> m_CommonWeaponTemplate;
    UPROPERTY(EditDefaultsOnly, Category = "Weapon Rarity Blueprints")
        TSubclassOf<class AWeaponPickup> m_UncommonWeaponTemplate;
    UPROPERTY(EditDefaultsOnly, Category = "Weapon Rarity Blueprints")
        TSubclassOf<class AWeaponPickup> m_RareWeaponTemplate;
    UPROPERTY(EditDefaultsOnly, Category = "Weapon Rarity Blueprints")
        TSubclassOf<class AWeaponPickup> m_LegendaryWeaponTemplate;

    TArray<TSubclassOf<class AWeaponPickup>> m_WeaponBPTypes;

    void InitializeWeaponSetupArrays();
    TSubclassOf<class AWeaponPartActor> GetRandomPart(WeaponTypeVariant WeaponType, WeaponPartCategories WeaponPartCategory, int WeaponAmmoType); 

    //Initializes a new layer to the nested array structure above ^
        //-Specify the weapon type
        //-Specify the part category
        //-Specify how many type to create
    void CreateNewWeaponStructureLayer(WeaponTypeVariant weaponType, WeaponPartCategories partCategory, int numToInitialize);

    //void StoreNewPart(WeaponTypeVariant WeaponType, WeaponPartCategories WeaponParts);
    //TSubclassOf<class AProjectileActor> m_weaponProjectile;
    TSubclassOf<class APrototypeProjectile> m_prototypeWeaponProjectile;

    //Release 3 
    int ammoType = 0;
    int generalAmmoType = 0;
    UPROPERTY(EditDefaultsOnly, Category = "Weapon Generation | Default Weapon Spawn Types")
        RifleCarbineAmmoTypes RifleAmmoType = RifleCarbineAmmoTypes::RCAT_NumTypes;
    UPROPERTY(EditDefaultsOnly, Category = "Weapon Generation | Default Weapon Spawn Types")
        RiflePDWAmmoTypes PDWAmmoType = RiflePDWAmmoTypes::RPAT_NumTypes;
    UPROPERTY(EditDefaultsOnly, Category = "Weapon Generation | Default Weapon Spawn Types")
        ShotgunAmmoTypes ShotgunAmmoType = ShotgunAmmoTypes::SAT_NumTypes;
    UPROPERTY(EditDefaultsOnly, Category = "Weapon Generation | Default Weapon Spawn Types")
        PistolAmmoTypes PistolAmmoType = PistolAmmoTypes::PAT_NumTypes;
    
    //Rifle Type Chances
#pragma region
    UPROPERTY(EditDefaultsOnly, Category = "Weapon Generation | Default Weapon Spawn Types | Rifle Type Percentage")
        int m_rifleBallisticWeaponChance = 50;
    UPROPERTY(EditDefaultsOnly, Category = "Weapon Generation | Default Weapon Spawn Types | Rifle Type Percentage")
        int m_rifleLaserWeaponChance = 30;
    UPROPERTY(EditDefaultsOnly, Category = "Weapon Generation | Default Weapon Spawn Types | Rifle Type Percentage")
        int m_rifleCrystalWeaponChance = 30;
    UPROPERTY(EditDefaultsOnly, Category = "Weapon Generation | Default Weapon Spawn Types | Rifle Type Percentage")
        int m_rifleBalloonWeaponChance = 10;
#pragma endregion

    //PDW Type Chances
#pragma region
    UPROPERTY(EditDefaultsOnly, Category = "Weapon Generation | Default Weapon Spawn Types | PDW Type Percentage")
        int m_pdwBallisticWeaponChance = 100;
#pragma endregion

    //Shotgun Type Chances
#pragma region
    UPROPERTY(EditDefaultsOnly, Category = "Weapon Generation | Default Weapon Spawn Types | Shotgun Type Percentage")
        int m_shotgunBallisticWeaponChance = 80;
    UPROPERTY(EditDefaultsOnly, Category = "Weapon Generation | Default Weapon Spawn Types | Shotgun Type Percentage")
        int m_shotgunSoundWaveWeaponChance = 20;
#pragma endregion

    //Pistol Type Chances
#pragma region
    UPROPERTY(EditDefaultsOnly, Category = "Weapon Generation | Default Weapon Spawn Types | Pistol Type Percentage")
        int m_pistolBallisticWeaponChance = 100;
#pragma endregion
    bool RandomChance(int BaseValue, int Multiplier);

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

    virtual void PostLoad() override;
};
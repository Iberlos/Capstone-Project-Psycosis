// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

/*************************************
Class: EnumMaster
Author: Luis Filipe Moraes
Created: october 2019
Last Modified: 21/feb/2020
Description: This class is just a holder for the definition of all the enums that might be shared between more than one class.
References:
Change Log
**********
Date: unknown
Author: Luis Filipe Moraes, Lucas Felix, Erin, Abdul Ali
Verified By: Lucas Felix & Abdul Ali
Changes: Added all the enum information needed for the basic functionality of many classes such as the inventory and the weapon egenration and weapon functionality.
----------------------------------------------------------------------------------
Date: unknown
Author: Luis Filipe Moraes
Verified By: Lucas Felix & Abdul Ali
Changes: Modified the existing Inventory Tab Enum to hold an extra index and remove the outdated Artifacts index. did the same to the Weapon Type Enum
----------------------------------------------------------------------------------
*************************************/
UENUM(BlueprintType)		//"BlueprintType" is essential to include
enum class EAIStates : uint8
{
    AI_Idle,
    AI_Attack,
    AI_Chase,
    AI_Dead
};

UENUM(BlueprintType)		//"BlueprintType" is essential to include
enum class EAITypes : uint8
{
    AI_Hook,
    AI_Bee,
    AI_Chonk,
    AI_Bird
};

UENUM(BlueprintType)
enum class EAITaskFunctions : uint8
{
    AIFUNC_PrimaryATK,
    AIFUNC_SecondaryATK,
    //AIFUNC_SearchForPlayer,
    AIFUNC_MoveToPlayer
};

//Inventory and Pickup Enums
UENUM(BlueprintType)		//"BlueprintType" is essential to include
enum class InventoryTabEnum : uint8
{
	ITE_Pistols = 0 UMETA(DisplayName = "Pistols"),
	ITE_Shotguns UMETA(DisplayName = "Shotguns"),
	ITE_Rifles UMETA(DisplayName = "Rifles"),
	ITE_PDWs UMETA(DisplayName = "PDW"),
	ITE_Invalid UMETA(DisplayName = "Invalid")
};

//Chest Enums
UENUM(BlueprintType)		//"BlueprintType" is essential to include
enum class ChestModeEnum : uint8
{
	CME_GenerateFromTable = 0 UMETA(DisplayName = "Generate From Drop Table"),
	CME_GenerateByWeight UMETA(DisplayName = "Generate Random Weighted"),
	CME_GenerateBoth UMETA(DisplayName = "Generate by Table and Then Randomly by Weight"),
	CME_Invalid UMETA(DisplayName = "Invalid")
};

//Weapon Enums
UENUM(BlueprintType)		//"BlueprintType" is essential to include
enum class WeaponTypeEnum : uint8
{
	WTE_Pistol = 0 UMETA(DisplayName = "Pistol"),
	WTE_Shotgun UMETA(DisplayName = "Shotgun"),
	WTE_Rifle UMETA(DisplayName = "Rifle"),
	WTE_PDW UMETA(DisplayName = "PDW"),
	WTE_Invalid UMETA(DisplayName = "Invalid")
};

UENUM(BlueprintType)		//"BlueprintType" is essential to include
enum class WeaponRarityEnum : uint8
{
	WRE_Common = 0 UMETA(DisplayName = "Common"),
	WRE_Uncommon UMETA(DisplayName = "Uncomomn"),
	WRE_Rare UMETA(DisplayName = "Rare"),
	WRE_Legendary UMETA(DisplayName = "Legendary"),
	WRE_Invalid UMETA(DisplayName = "Invalid")
};

UENUM(BlueprintType)		//"BlueprintType" is essential to include
enum class WeaponAttributeEnum : uint8 
{
	WAE_Damage = 0 UMETA(DisplayName = "Damage"),
	WAE_Accuracy UMETA(DisplayName = "Accuracy"),
	WAE_FireRate UMETA(DisplayName = "Fire Rate"),
	WAE_Recoil UMETA(DisplayName = "Recoil"),
	WAE_MagazineSize UMETA(DisplayName = "Magazine Size"),
	WAE_Weight UMETA(DisplayName = "Weight"),
	WAE_Invalid UMETA(DisplayName = "Invalid")
};

UENUM(BlueprintType)		//"BlueprintType" is essential to include
enum class WeaponFireModeEnum : uint8
{
	WFME_Single = 0 UMETA(DisplayName = "Single Shot"),
	WFME_Burst UMETA(DisplayName = "Burst"),
	WFME_Auto UMETA(DisplayName = "FullAuto"),
	WFME_Invalid UMETA(DisplayName = "Invalid")
};


//Projectile effect descriptors, will be changed to actual effect names once
//can be proved they are being properly accessed
UENUM(BlueprintType)
enum class CustomStartEffects : uint8
{
    CSE_MultiShot = 0         UMETA(DisplayName = "Multishot"),
    CSE_NumberOfEffects     UMETA(DisplayName = "NumEffects")
};

UENUM(BlueprintType)
enum class CustomInFlightEffects : uint8
{
    CIFE_RadialZap = 0        UMETA(DisplayName = "RadialZap"),
    CIFE_NumberOfEffects    UMETA(DisplayName = "NumEffects")
};

UENUM(BlueprintType)
enum class CustomOnHitEffects : uint8
{
    COHE_Split = 0        UMETA(DisplayName = "Split"),
    COHE_NumberOfEffects    UMETA(DisplayName = "NumEffects")
};

//Weapon variant Enum
UENUM(BlueprintType)
enum class WeaponTypeVariant : uint8
{

    WTV_RifleCarbine = 0        UMETA(DisplayName = "Rifle_Carbine"),
    WTV_RiflePDW                UMETA(DisplayName = "Rifle_PDW"),
    WTV_Shotgun                 UMETA(DisplayName = "Shotgun_Default"),
    WTV_Pistol                  UMETA(DisplayName = "Pistol_Default"),
    WTV_NumTypes                UMETA(DisplayName = "Number of Weapon Types")
};


UENUM(BlueprintType)
enum class RifleCarbineAmmoTypes : uint8
{

    RCAT_Ballistic = 0          UMETA(DisplayName = "Ballistic"),
    RCAT_Laser                  UMETA(DisplayName = "Laser"),
    RCAT_Crystal                UMETA(DisplayName = "Crystal"),
    RCAT_Balloon                UMETA(DisplayName = "Balloon"),
    RCAT_NumTypes               UMETA(DisplayName = "Number of Ammo Types")
};
UENUM(BlueprintType)
enum class RiflePDWAmmoTypes : uint8
{

    RPAT_Ballistic = 0          UMETA(DisplayName = "Ballistic"),
    RPAT_NumTypes               UMETA(DisplayName = "Number of Ammo Types")

};
UENUM(BlueprintType)
enum class ShotgunAmmoTypes : uint8
{

    SAT_Ballistic = 0           UMETA(DisplayName = "Ballistic"),
    SAT_ShockWave               UMETA(DisplayName = "Shockwave"),
    SAT_NumTypes                UMETA(DisplayName = "Number of Ammo Types")
};
UENUM(BlueprintType)
enum class PistolAmmoTypes : uint8
{

    PAT_Ballistic = 0           UMETA(DisplayName = "Ballistic"),
    PAT_NumTypes                UMETA(DisplayName = "Number of Ammo Types")

};

UENUM(BlueprintType)
enum class AmmoTypes : uint8
{

    AT_Ballistic = 0            UMETA(DisplayName = "Ballistic"),
    AT_Laser                    UMETA(DisplayName = "Laser"),
    AT_Crystal                  UMETA(DisplayName = "Crystal"),
    AT_Balloon                  UMETA(DisplayName = "Balloon"),
    AT_ShockWave                UMETA(Displayname = "ShockWave"),
    AT_NumTypes                 UMETA(DisplayName = "Number of Ammo Types")

};

UENUM(BlueprintType)
enum class WeaponPartCategories : uint8
{

    WPC_Receiver = 0            UMETA(DisplayName = "Receiver"),
    WPC_ErgoEnhancer            UMETA(DisplayName = "Ergonomic_Enhancer"),
    WPC_Barrel                  UMETA(DisplayName = "Barrel"),
    WPC_Magazine                UMETA(DisplayName = "Magazine"),
    WPC_Accessory               UMETA(DisplayName = "Accessory"),
    WPC_NumTypes                UMETA(DisplayName = "Number Of Parts")
};

UENUM(BlueprintType)
enum class BlockerType : uint8
{

    BT_Entrance = 0             UMETA(DisplayName = "Entrance"),
    BT_Exit                     UMETA(DisplayName = "Exit")

};


class CAPSTONE_TOXICOASIS_API EnumMaster
{
public:
	EnumMaster();
	~EnumMaster();
};



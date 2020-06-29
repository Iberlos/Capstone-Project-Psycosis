// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
//Custom Unreal includes #There shouldn't be any includes in the .h file, but if one is needed it goes here.#

//Custom project includes #There shouldn't be any includes in the .h file, but if one is needed it goes here.#
#include "EnumMaster/EnumMaster.h"

//Generated include #must be included last because unreal complains if there are any includes between it and the generated body thingy.#
#include "WeaponGeneratorObject.generated.h"

/**
 * 
 */
UCLASS()
class CAPSTONE_TOXICOASIS_API UWeaponGeneratorObject : public UObject
{
	GENERATED_BODY()
public:
    //Essentials
    UWeaponGeneratorObject();
    //Other Methods

    UFUNCTION(BlueprintCallable, Category = "Weapon Generation")
    void GenerateWeaponFromTemplate(class AWeaponPickup* WeaponTemplate);

#pragma region
private:
    //...
public:
#pragma endregion
    //OnEvents
    //Components
    //Variables

protected:
    //Essentials
    //Other Methods
    UFUNCTION(Server, Reliable, WithValidation)
    void SERVER_PopulateWeaponPartArrays();
    bool SERVER_PopulateWeaponPartArrays_Validate() { return true; };
#pragma region
private:
    //...
protected:
#pragma endregion
    //OnEvents
    //Components
    //Variables

    //Projectiles
    TSubclassOf<class AProjectileActor> m_weaponProjectile;
//Rifle Part arrays
#pragma region
    UPROPERTY(BlueprintReadWrite)
    TArray<TSubclassOf<class AWeaponPartActor>> m_rifleBarrelWeaponParts;
    UPROPERTY(BlueprintReadWrite)
    TArray<TSubclassOf<class AWeaponPartActor>> m_rifleMagazineWeaponParts;


    UPROPERTY(BlueprintReadWrite)
    TArray<TSubclassOf<class AWeaponPartActor>> m_rifleReceiverWeaponParts;

    UPROPERTY(BlueprintReadWrite)
    TArray<TSubclassOf<class AWeaponPartActor>> m_rifleOpticWeaponParts;
    UPROPERTY(BlueprintReadWrite)
    TArray<TSubclassOf<class AWeaponPartActor>> m_rifleStockWeaponParts;
#pragma endregion

//Pistol Part Arrays
#pragma region
    UPROPERTY(BlueprintReadWrite)
        TArray<TSubclassOf<class AWeaponPartActor>> m_pistolBarrelParts;
    UPROPERTY(BlueprintReadWrite)
        TArray<TSubclassOf<class AWeaponPartActor>> m_pistolFrameParts;
    UPROPERTY(BlueprintReadWrite)
        TArray<TSubclassOf<class AWeaponPartActor>> m_pistolMagazineParts;
    UPROPERTY(BlueprintReadWrite)
        TArray<TSubclassOf<class AWeaponPartActor>> m_pistolSlideParts;
    UPROPERTY(BlueprintReadWrite)
        TArray<TSubclassOf<class AWeaponPartActor>> m_pistolTriggerGuardParts;
#pragma endregion

//Shotgun Part Arrays
#pragma region
    UPROPERTY(BlueprintReadWrite)
        TArray<TSubclassOf<class AWeaponPartActor>> m_shotgunBarrelParts;
    UPROPERTY(BlueprintReadWrite)
        TArray<TSubclassOf<class AWeaponPartActor>> m_shotgunTubeParts;
    UPROPERTY(BlueprintReadWrite)
        TArray<TSubclassOf<class AWeaponPartActor>> m_shotgunPumpParts;
    UPROPERTY(BlueprintReadWrite)
        TArray<TSubclassOf<class AWeaponPartActor>> m_shotgunReceiverParts;
    UPROPERTY(BlueprintReadWrite)
        TArray<TSubclassOf<class AWeaponPartActor>> m_shotgunStockParts;
#pragma endregion
};

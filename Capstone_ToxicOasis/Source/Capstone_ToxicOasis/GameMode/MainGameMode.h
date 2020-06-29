// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "EnumMaster/EnumMaster.h"
#include "MainGameMode.generated.h"

/**
 * 
 */
UCLASS()
class CAPSTONE_TOXICOASIS_API AMainGameMode : public AGameMode
{
	GENERATED_BODY()
public:
    AMainGameMode();

    UFUNCTION(BlueprintCallable)
        TArray<class APlayerCharacter*> GetPlayerList() { return m_playerList; }
    UFUNCTION(BlueprintCallable) 
        void AddPlayerToList(class APlayerCharacter* a_player) { m_playerList.Add(a_player); }

    void PostLogin(APlayerController* NewPlayer);

    virtual void StartPlay() override;

    //Release 2
    UFUNCTION(BlueprintCallable)
        class AWeaponGeneratorActor* GetWorldWeaponGenerator();

    void AddNewWeaponToPool(class AWeaponPickup* newWeapon);
    //class AWeaponPickup* GetWeaponFromPool();
    class AWeaponPickup* GetWeaponFromPool(WeaponRarityEnum weaponRarity);


    bool IsWeaponPoolValid();

    void AddNewBulletToPool(class APrototypeProjectile* newBullet);
    class APrototypeProjectile* GetBulletFromPool();
    bool IsBulletPoolValid();


    int GetSizeOfWeaponPool() { return CurrentWeaponPool.Num(); }
    void CreateWeaponPool();
    void SetGameIsReadyToStart();

protected:
    TArray<class APlayerCharacter*> m_playerList;
    FTimerHandle CheckLoadCompleteTimer;
    //Release 2
    void FindWeaponGeneratorInWorld();
    class AWeaponGeneratorActor* m_worldWeaponGenerator;

    TArray<class AWeaponPickup*> CurrentWeaponPool;

    TArray<class AWeaponPickup*> CommonWeaponPool;
    TArray<class AWeaponPickup*> UncommonWeaponPool;
    TArray<class AWeaponPickup*> RareWeaponPool;
    TArray<class AWeaponPickup*> LegendaryWeaponPool;

    TArray<class APrototypeProjectile*> CurrentBulletPool;

    UPROPERTY(EditDefaultsOnly)
        TSubclassOf<class AWeaponGeneratorActor> m_WeaponGenerator;

    void VerifyLoadingComplete();

    UFUNCTION(NetMulticast, Reliable, WithValidation)
        void NETMC_EndLoadingScreen();
    bool NETMC_EndLoadingScreen_Validate() { return true; };
};

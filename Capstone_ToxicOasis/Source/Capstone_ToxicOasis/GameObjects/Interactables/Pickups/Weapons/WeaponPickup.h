// Fill out your copyright notice in the Description page of Project Settings.

//Default Unreal Includes
#pragma once

#include "CoreMinimal.h"
#include "GameObjects/Interactables/Pickups/PickupInteractable.h"
//Custom Unreal includes #There shouldn't be any includes in the .h file, but if one is needed it goes here.#

//Custom project includes #There shouldn't be any includes in the .h file, but if one is needed it goes here.#
#include "EnumMaster/EnumMaster.h"
#include "GameObjects/Weapons/WeaponGeneratorActor.h"


//Generated include #must be included last because unreal complains if there are any includes between it and the generated body thingy.#
#include "WeaponPickup.generated.h"

/*************************************
Class: WeaponPickup
Author: Luis Filipe Moraes
Created: october 2019
Last Modified: 4/feb/2020
Description: This is the only weapon class in the game.It can be customized to fit any weapon style in the game using stats and parts and projectiles with special logic.
References:
Change Log
**********
Date: unknown
Author: Luis Filipe Moraes
Verified By: Lucas Felix & Abdul Ali
Changes: Added all the functionality that allows the weapon to fire, be populated by the generator, be added and removed from the inventory, be equipped an used by the player and etc.
Date: between 20/jan and 4/feb/2020
Author: Luis Filipe Moraes
Verified By: Lucas Felix & Abdul Ali
Changes: modified the class to allow fire, draw, Holster, animation montages, also modified to allow the isFreindlyFire to be passed down from the firingPlayer to the projectiles.
Date: 3/mar/2020
Author: Luis Filipe Moraes
Verified By: Lucas Felix & Abdul Ali
Changes: modified the equip and unequip and added a FinalizeEquip and FinalizeUnequip functions to allow for waiting for the animation notify from the player. Had to add a public variable here.
--------------------------------------------------------
*************************************/

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam (FOnPlayerReleaseTrigger, AWeaponPickup* , OwningWeapon);

UCLASS()
class CAPSTONE_TOXICOASIS_API AWeaponPickup : public APickupInteractable
{
	GENERATED_BODY()
	
public:
	//FRIEND DECLARATION AVOID DECLARING A WHOLE CLASS AS A FRIEND. USE THE FRIEND FUNCTION DECLARATION INSTEAD. THIS FUNCTION MUST BE THE FIRST ONE TO SHOW THAT IT IS NOT A NORMAL FUNCTION.
	friend void AWeaponGeneratorActor::SetWeaponParticlesAndSound(AWeaponPickup*);
	void SetProjectileParticlesAndSound(class APrototypeProjectile* a_projectile); //This function is a friend of the prototype projectile class. Inside this function you will have private access to all its features. BE VERY CAREFULL USE IT ONLY FOR THE ORIGINAL PURPOSE.
	
	//Essentials
	AWeaponPickup();
	void Tick(float DeltaTime) override;

	//Other Methods
	bool Interact(AActor* a_actor) override;
    UFUNCTION(BlueprintCallable)
	bool Use(AActor* a_actor) override;
    UFUNCTION(BlueprintCallable)
	void Drop(AActor* a_actor) override;
	void StartFiring(AActor* a_actor, bool a_isFrendlyFire);

    //Release 2 //COMMENT ADDED BY ANOTHER TEAMMATE (I WOULD NOT MARK THE PASSAGE OF TIME IN THE CODE, SEEMS COUTER PRODUCTIVE)

	void StopFiring();

	void Reload();
    void AddNewWeaponPartToWeaponPartArray(class AWeaponPartActor* WeaponPartToAdd); //Adds a new weapon part to the array of the owned weapon parts
	void DestroySelf();
	void CompleteWeaponAssembly();
#pragma region
private:
	UFUNCTION(NetMulticast, Reliable, WithValidation)
		void NET_CompleteWeaponAssembly();
	bool NET_CompleteWeaponAssembly_Validate() { return true; };
public:
#pragma endregion
	//OnEvents

	//Components

	//Variables
	class APlayerCharacter* m_tempReferenceToCharacterBeingEquipped;

	//Getters and Setters
	UFUNCTION(BlueprintCallable)
		WeaponRarityEnum GetWeaponRarity() { return m_weaponRarity_r; }
	UFUNCTION(BlueprintCallable)
		float GetAttributeTotal(WeaponAttributeEnum a_key); //used to calculate the total of the parts atributes plus the base
    UFUNCTION(BlueprintCallable)
	    float GetBaseAtribute(WeaponAttributeEnum a_key) { return m_totalAttributes[(int)a_key]; } //probably acessed by UI
	void SetBaseAtribute(WeaponAttributeEnum a_key, float a_value) { m_totalAttributes[(int)a_key] = a_value; } //probably only used when weapon is generated
	float GetAmmoPool() { return m_ammoPool; } //probably acessed by UI
	void SetAmmoPool(float a_ammoPool) { m_ammoPool = a_ammoPool; } //probably only used when weapon is generated
	UFUNCTION(BlueprintCallable)
		float GetAmmoInMagazine() { return m_ammoInMagazine; } //probably acessed by UI
	void SetAmmoInMagazine(float a_ammoInMagazine) { m_ammoInMagazine = a_ammoInMagazine; }; //probably never used
    void SetWeaponType(WeaponTypeEnum a_weaponType) { m_weaponType_r = a_weaponType; } // Sets the weapon type
    UFUNCTION(BlueprintCallable)
        int GetWeaponType() { return static_cast<int>(m_weaponType_r); }// returns the weapon type
    void SetWeaponRarity(WeaponRarityEnum a_weaponRarity) { m_weaponRarity_r = a_weaponRarity; } //Sets the weapon rarity
    void SetWeaponFireMode(WeaponFireModeEnum a_weaponFireMode) { m_fireMode_r = a_weaponFireMode; } //Sets the weapon fire mode
	UFUNCTION(BlueprintCallable)
		WeaponFireModeEnum GetWeaponFireMode() { return m_fireMode_r; }
    void SetWeaponProjectile(TSubclassOf<class AProjectileActor> projectile) { m_weaponProjectile = projectile; }
	bool GetIsFiring() { return m_isFiring; }
	void SetIsAiming(bool a_isAiming) {m_isAiming = a_isAiming;}
	class APlayerCharacter* GetFiringPlayerCharacter() { return m_firingPlayerCharacter; }
	int GetNumberOfWeaponParts() { return m_weaponParts_r.Num(); }
    //Release 2 //COMMENT ADDED BY ANOTHER TEAMMATE (I WOULD NOT MARK THE PASSAGE OF TIME IN THE CODE, SEEMS COUTER PRODUCTIVE)
    WeaponFireModeEnum GetWeaponFiremode(){ return m_fireMode_r; }

    void SetOnProjectileStartEffect(CustomStartEffects newProjectileStartEffect) { m_projectileOnStartEffect = newProjectileStartEffect; }
    void SetInFlightProjectileEffect(CustomInFlightEffects newProjectileInFlightEffects) { m_projectileInFlightEffect = newProjectileInFlightEffects; }
    void SetOnProjectileHitEffect(CustomOnHitEffects newProjectileOnHitEffects) { m_projectileOnHitEffect = newProjectileOnHitEffects; }
	CustomStartEffects GetOnProjectileStartEffect() { return m_projectileOnStartEffect; }
	CustomInFlightEffects GetInFlightProjectileEffect() { return m_projectileInFlightEffect; }
	CustomOnHitEffects GetOnProjectileHitEffect() { return m_projectileOnHitEffect; }

    UPROPERTY(BlueprintAssignable, Category = "EventDispatchers")
        FOnPlayerReleaseTrigger ed_TriggerRelease;

	//Release 3 //COMMENT ADDED BY ANOTHER TEAMMATE (I WOULD NOT MARK THE PASSAGE OF TIME IN THE CODE, SEEMS COUTER PRODUCTIVE)
#pragma region
	//THIS REGION WAS ADDED BY ANOTHER TEAMMATE (I USE REGIONS TO HIDE NETWORKED IMPLEMENTATIONS OF FUNCTIONS SO THE CODE IS EASIER TO READ. HAVING THIS REGION HERE BREAKS THAT PATTERN.)
    void SetProjectileType(int AmmoType) { wepAmmoType = AmmoType; }
	UFUNCTION(BlueprintCallable)
    int GetProjectileType() { return wepAmmoType; }
    TSubclassOf<class AProjectileActor> GetProjectileClass() { return m_weaponProjectile; }
	UFUNCTION(BlueprintCallable)
	float GetFiringConeAngle() { return m_accuracyConeAngle; }
	UFUNCTION(Server, Reliable, WithValidation)
		void SERVER_EnableWeapon();
	bool SERVER_EnableWeapon_Validate() { return true; }
	UFUNCTION(Server, Reliable, WithValidation)
		void SERVER_DisableWeapon();
	bool SERVER_DisableWeapon_Validate() { return true; }
#pragma endregion 

	class UArrowComponent* GetArrowComponent() { return m_arrowComponent; }
	class APlayerCharacter* GetPlayerCharacter() { return m_firingPlayerCharacter; }

protected:
	//Essentials
	void BeginPlay();

	//Other Methods
	void Fire();
	void Equip(AActor* a_Actor);
#pragma region
public:
		void FinalizeEquip();
protected:
#pragma endregion
	void Unequip(AActor* a_Actor, bool a_discardTrigered = false);
#pragma region
public:
	void FinalizeUnequip();
protected:
#pragma endregion
	
#pragma region
private:
	UFUNCTION(NetMulticast, Reliable, WithValidation)
		void NET_SetRegisteredMeshVisibilities(bool a_state);
		bool NET_SetRegisteredMeshVisibilities_Validate(bool a_state) { return true; };
protected:
#pragma endregion //Register Mesh Visibility networked functions for internal use
	//OnEvents

	//Components
	UPROPERTY(EditDefaultsOnly, Category = "Dev. Data|Weapon Pickup Data|Components")
		class UArrowComponent* m_arrowComponent;

	//Variables
	bool m_isFriendlyFire;
	UPROPERTY(EditDefaultsOnly, Replicated, Category = "Dev. Data|Weapon Pickup Data|Variables")
		WeaponTypeEnum m_weaponType_r;
	UPROPERTY(EditDefaultsOnly, Replicated, Category = "Dev. Data|Weapon Pickup Data|Variables")
		WeaponRarityEnum m_weaponRarity_r;
	UPROPERTY(EditDefaultsOnly, Replicated, Category = "Dev. Data|Weapon Pickup Data|Variables")
		WeaponFireModeEnum m_fireMode_r;
	UPROPERTY(EditDefaultsOnly, Category = "Dev. Data|Weapon Pickup Data|Variables")
		TSubclassOf<class AProjectileActor> m_weaponProjectile;

	UPROPERTY(EditDefaultsOnly, Replicated, Category = "Dev. Data|Weapon Pickup Data|Variables")
		int m_ammoPool;
	UPROPERTY(EditDefaultsOnly, Replicated, Category = "Dev. Data|Weapon Pickup Data|Variables")
		int m_ammoInMagazine;
	UPROPERTY(EditDefaultsOnly, Replicated, Category = "Dev. Data|Weapon Pickup Data|Variables")
		int m_totalAmmo;


    //Release 2 //COMMENT ADDED BY ANOTHER TEAMMATE (I WOULD NOT MARK THE PASSAGE OF TIME IN THE CODE, SEEMS COUTER PRODUCTIVE)
    UPROPERTY(Replicated)
        CustomStartEffects m_projectileOnStartEffect = CustomStartEffects::CSE_NumberOfEffects;
    UPROPERTY(Replicated)
		CustomInFlightEffects m_projectileInFlightEffect = CustomInFlightEffects::CIFE_NumberOfEffects;
    UPROPERTY(Replicated)
		CustomOnHitEffects m_projectileOnHitEffect = CustomOnHitEffects::COHE_NumberOfEffects;

	//Release 3 //COMMENT ADDED BY ANOTHER TEAMMATE (I WOULD NOT MARK THE PASSAGE OF TIME IN THE CODE, SEEMS COUTER PRODUCTIVE)
	int wepAmmoType = 0;
	float m_accuracyConeAngle = 0;
	float m_accuracyAimMod = 0;
    //--------------------------------------------

    //Used to be EditDefaultsOnly //COMMENT ADDED BY ANOTHER TEAMMATE (I WOULD NOT MARK THE PASSAGE OF TIME IN THE CODE, SEEMS COUTER PRODUCTIVE)
	UPROPERTY(EditDefaultsOnly, Replicated, Category = "Dev. Data|Weapon Pickup Data|Variables")
		TArray<float> m_totalAttributes;
	UPROPERTY(Replicated)
		TArray<class AWeaponPartActor*> m_weaponParts_r;
	FTimerHandle m_fireTimerHandle;
	int m_burstCount;
	class APlayerCharacter* m_firingPlayerCharacter;
	bool m_isFiring;
	bool m_isAiming;
	int m_fireCount = 1;
};

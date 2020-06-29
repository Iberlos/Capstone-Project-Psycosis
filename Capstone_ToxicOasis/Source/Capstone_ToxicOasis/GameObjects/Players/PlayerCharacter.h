// Fill out your copyright notice in the Description page of Project Settings.

//Default Unreal Includes
#pragma once

#include "CoreMinimal.h"
#include "GameObjects/Players/MyCharacter.h"
//Custom Unreal includes #There shouldn't be any includes in the .h file, but if one is needed it goes here.#

//Custom project includes #There shouldn't be any includes in the .h file, but if one is needed it goes here.#
#include "EnumMaster/EnumMaster.h"

//Generated include #must be included last because unreal complains if there are any includes between it and the generated body thingy.#
#include "PlayerCharacter.generated.h"

/************************************* 
Class: PlayerCharacter
Author: Luis Filipe Moraes
Created: october 2019
Last Modified: 4/feb/2020
Description: This is the player class used in the game, it is responsible for being a medium between all othr classes and the components in this class and its base class by means of interface functions. It also binds all the player input functions and allows for networked interaction of the players with the world.
References:
Change Log
**********
Date: unknown
Author: Luis Filipe Moraes
Verified By: Lucas Felix & Abdul Ali
Changes: Added all the functionlaity related to the base fucntionality of the class.
--------------------------------------------------------
Date: between 20/jan and 4/feb/2020
Author: Luis Filipe Moraes
Verified By: Lucas Felix & Abdul Ali
Changes: modified to override some os the functionality of the MyCharacter base class to allow the animation to be played on both the first and third person meshes. Added all the animations needed to play the montages. Added logic to assign the player and the instance an index that allows the player to re assign some of the changes when switching levels. Set up the changes in the collision presets so the friendly fire functionality works networked.
--------------------------------------------------------
Date: 4/feb/2020
Author: Luis Filipe Moraes
Verified By: Lucas Felix & Abdul Ali
Changes: Modified the replicatiion process of the look rotation to reduce the number of network messages sent per frame.
--------------------------------------------------------
Date: 14/feb/2020
Author: Luis Filipe Moraes
Verified By: Lucas Felix & Abdul Ali
Changes: modified the SetupCollisionProfilesForFirendlyFire function to be a generic function that notifies that all players have been initialized
--------------------------------------------------------
Date: 21/feb/2020
Author: Luis Filipe Moraes
Verified By: Lucas Felix & Abdul Ali
Changes: Modified the apply recoil logic to make recoil feel better
--------------------------------------------------------
Date: 2/mar/2020
Author: Luis Filipe Moraes
Verified By: Lucas Felix & Abdul Ali
Changes: Modified the ScrapWeapon logic to allow for waiing for the animations. Added a NotifyHolster function to allow the wait for animation when switching weapons. Had to add a public variable here.
*************************************/

UCLASS()
class CAPSTONE_TOXICOASIS_API APlayerCharacter : public AMyCharacter
{
	GENERATED_BODY()
	
public:
	//Essentials
	APlayerCharacter();
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	virtual void Tick(float DeltaTime) override;
	//Other Methode
	float PlayAnimationMontage(FName a_key, int a_index = 0) override;
#pragma region
private:
		void NET_PlayAnimationMontage(FName a_key, int a_index) override;
public:
#pragma endregion
	UFUNCTION(BlueprintCallable)
		void FallBackIntoLevel();
#pragma region
private:
	UFUNCTION(Server, Reliable, WithValidation)
		void SERVER_FallBackIntoLevel();
		bool SERVER_FallBackIntoLevel_Validate() { return true; }
	UFUNCTION(Client, Reliable, WithValidation)
		void CLIENT_FallBackIntoLevel();
		bool CLIENT_FallBackIntoLevel_Validate() { return true; }
public:
#pragma endregion
	void ApplyRecoil(float a_recoilAmmount);
#pragma region
private:
	UFUNCTION(Client, Reliable, WithValidation)
		void CLIENT_ApplyRecoil(float a_recoilAmmount);
		bool CLIENT_ApplyRecoil_Validate(float a_recoilAmmount) { return true; }
public:
#pragma endregion
	void TrashWeapon();
#pragma region
private:
	UFUNCTION(Server, Reliable, WithValidation)
		void SERVER_TrashWeapon();
		bool SERVER_TrashWeapon_Validate() { return true; }
public:
	UFUNCTION(BlueprintCallable)
		void FinalizeServerThrashWeapon();
#pragma endregion
	UFUNCTION(BlueprintImplementableEvent, Category = "Dev. Data|Player Character Data|Blueprint Events")
		void InitiateFadeFromBlack(APlayerCharacter* a_targetPlayer);
	UFUNCTION(BlueprintCallable)
		void NotifyHolster();
	//OnEvents
	UFUNCTION()
		void OnHit
		(
			UPrimitiveComponent* HitComp,
			AActor* OtherActor,
			UPrimitiveComponent* OtherComp,
			FVector NormalImpulse,
			const FHitResult& Hit
		);
	//Components

	//Variables
	class AWeaponPickup* m_tempReferenceToWeaponBeingEquipped;
	//Getters and Setters
	class APickupInteractable* GetDefaultWeapon();
	UFUNCTION(BlueprintCallable)
		class AInteractableActor* GetCurrentFocusedInteractable() { return m_focusedInteractible[0]; }
    UFUNCTION(BlueprintCallable)
	class AWeaponPickup* GetEquippedWeapon() { return m_equippedWeapon_r; }
    UFUNCTION(BlueprintCallable)
	void SetEquippedWeapon(class AWeaponPickup* a_equippedWeapon) { m_equippedWeapon_r = a_equippedWeapon; }
	USkeletalMeshComponent* GetFPSMesh() { return m_FPSSkeletalMeshComponent; }
	UFUNCTION(BlueprintCallable)
		void SetCurrentCheckpoint(AActor* a_checkpoint) { m_currentCheckpoint_r = a_checkpoint; }
	UFUNCTION(BlueprintCallable)
		AActor* GetCurrentCheckpoint() { return m_currentCheckpoint_r; }
	int GetPlayerIndex() { return m_playerIndex_r; }
protected:
	//Essentials
	virtual void BeginPlay() override;
	//Other Methods
	void Die() override;
	void Respawn() override;
#pragma region
private:
	UFUNCTION(Client, Reliable, WithVAlidation)
		void CLIENT_Respawn();
		bool CLIENT_Respawn_Validate() { return true; }
protected:
#pragma endregion
	void UpdateCameraRotation();
#pragma region
private:
	UFUNCTION(Server, Reliable, WithVAlidation)
		void SERVER_UpdateCameraRotation(FRotator a_viewRotation);
		bool SERVER_UpdateCameraRotation_Validate(FRotator a_viewRotation) { return true; }
	UFUNCTION(NetMulticast, Reliable, WithVAlidation)
		void NET_UpdateCameraRotation(FRotator a_viewRotation);
		bool NET_UpdateCameraRotation_Validate(FRotator a_viewRotation) { return true; }
protected:
#pragma endregion
	void MoveForward(float a_val);
	void MoveSideways(float a_val);
	void MouseInputHorizontal(float a_val);
	void MouseInputVertical(float a_val);
	void LookVertical(float a_val);
	void LookHorizontal(float a_val);
	void PlayerJump();
	void StartFiring();
#pragma region
private:
	UFUNCTION(Server, Reliable, WithVAlidation)
		void SERVER_StartFiring(AActor* a_actor);
		bool SERVER_StartFiring_Validate(AActor* a_actor) { return true; }
protected:
#pragma endregion
	void StopFiring();
#pragma region
private:
	UFUNCTION(Server, Reliable, WithVAlidation)
		void SERVER_StopFiring();
		bool SERVER_StopFiring_Validate() { return true; }
protected:
#pragma endregion
	void StartAiming();
#pragma region
private:
	UFUNCTION(Server, Reliable, WithVAlidation)
		void SERVER_StartAiming(AActor* a_actor);
	bool SERVER_StartAiming_Validate(AActor* a_actor) { return true; }
protected:
#pragma endregion
	void StopAiming();
#pragma region
private:
	UFUNCTION(Server, Reliable, WithVAlidation)
		void SERVER_StopAiming();
	bool SERVER_StopAiming_Validate() { return true; }
protected:
#pragma endregion
	void Reload();
#pragma region
private:
	UFUNCTION(Server, Reliable, WithVAlidation)
		void SERVER_Reload();
		bool SERVER_Reload_Validate() { return true; }
protected:
#pragma endregion
	void RayCastToFocused();
#pragma region
private:
	UFUNCTION(Server, Reliable, WithVAlidation)
		void SERVER_RayCastToFocused();
		bool SERVER_RayCastToFocused_Validate() { return true; }
	UFUNCTION(Client, Reliable, WithVAlidation)
		void CLIENT_RayCastToFocused();
		bool CLIENT_RayCastToFocused_Validate() { return true; }
protected:
#pragma endregion
	void InteractWithFocused();
#pragma region
private:
	UFUNCTION(Server, Reliable, WithVAlidation)
		void SERVER_InteractWithFocused();
		bool SERVER_InteractWithFocused_Validate() { return true; }
protected:
#pragma endregion
	void ShowInventory();
#pragma region
private:
	UFUNCTION(Server, Reliable, WithVAlidation)
		void SERVER_ShowInventory();
		bool SERVER_ShowInventory_Validate() { return true; }
protected:
#pragma endregion
	void HideInventory();
#pragma region
private:
	UFUNCTION(Server, Reliable, WithVAlidation)
		void SERVER_HideInventory();
		bool SERVER_HideInventory_Validate() { return true; }
protected:
#pragma endregion
	void DropSelected();
#pragma region
private:
	UFUNCTION(Server, Reliable, WithVAlidation)
		void SERVER_DropSelected();
		bool SERVER_DropSelected_Validate() { return true; }
protected:
#pragma endregion
    UFUNCTION(BlueprintCallable)
	void DropEquipped();
#pragma region
private:
	UFUNCTION(Server, Reliable, WithVAlidation)
		void SERVER_DropEquipped();
		bool SERVER_DropEquipped_Validate() { return true; }
protected:
#pragma endregion
	void UseSelected();
#pragma region
private:
	UFUNCTION(Server, Reliable, WithVAlidation)
		void SERVER_UseSelected();
		bool SERVER_UseSelected_Validate() { return true; }

#pragma endregion
    UFUNCTION(BlueprintCallable)
    void UseAt(InventoryTabEnum a_tab, int a_tabIndex);
#pragma region
private:
    UFUNCTION(Server, Reliable, WithVAlidation)
        void SERVER_UseAt(InventoryTabEnum a_tab, int a_tabIndex);
        bool SERVER_UseAt_Validate(InventoryTabEnum a_tab, int a_tabIndex) { return true; }

#pragma endregion
	void CheckForGround();
	void SetCollisionForFriendlyFire();
#pragma region
private:
	UFUNCTION(Server, Reliable, WithVAlidation)
	void SERVER_SetCollisionForFriendlyFire(int a_playerIdex);
	bool SERVER_SetCollisionForFriendlyFire_Validate(int a_playerIdex) { return true; }

	UFUNCTION(Client, Reliable, WithVAlidation)
	void CLIENT_SetCollisionForFriendlyFire();
	bool CLIENT_SetCollisionForFriendlyFire_Validate() { return true; }

	UFUNCTION(NetMulticast, Reliable, WithVAlidation)
	void NET_SetCollisionForFriendlyFire(int a_playerIdex);
	bool NET_SetCollisionForFriendlyFire_Validate(int a_playerIdex) { return true; }

#pragma endregion
	void EnableOnLevelStart();
	//OnEvents

	//Components
	UPROPERTY(EditDefaultsOnly, Category = "Dev. Data|Player Character Data|Components", meta = (AllowPrivateAccess = true))
		USkeletalMeshComponent* m_FPSSkeletalMeshComponent;
	UPROPERTY(EditDefaultsOnly, Category = "Dev. Data|Player Character Data|Components", meta = (AllowPrivateAccess = true))
		class UCameraComponent* m_FPSCameraComponent;
	//Variables
	UPROPERTY(EditDefaultsOnly, Category = "Dev. Data|Player Character Data|Variables")
		TSubclassOf<class AWeaponPickup> m_defaultWeaponTemplate;
    UPROPERTY(EditDefaultsOnly, Category = "Dev. Data|Player Character Data|Variables")
        float m_FOVLerpDurantion;
	UPROPERTY(Replicated)
	    class AWeaponPickup* m_equippedWeapon_r;
	class AInteractableActor* m_focusedInteractible[2];
	UPROPERTY(EditDefaultsOnly, Category = "Dev. Data|Player Character Data|Variables")
		float m_interactDistance;
	UPROPERTY(EditDefaultsOnly, Category = "Dev. Data|Player Character Data|Variables")
		float m_interactRadius;
	UPROPERTY(EditDefaultsOnly, Category = "Dev. Data|Player Character Data|Variables")
		float m_maxWeaponWeight;
	UPROPERTY(EditDefaultsOnly, Category = "Dev. Data|Player Character Data|Variables")
		float m_respawnDelay;
	UPROPERTY(Replicated)
		AActor* m_currentCheckpoint_r;
	UPROPERTY(EditDefaultsOnly, Category = "Dev. Data|Player Character Data|Variables")
		float m_fallBackHight;
	UPROPERTY(EditDefaultsOnly, Category = "Dev. Data|Player Character Data|Variables")
		float m_fallBackDamagePercentage;
	UPROPERTY(EditDefaultsOnly, Category = "Dev. Data|Player Character Data|Variables")
		TMap<FName, FAnimMontageLib_C> m_animationMontages_FPM;
	UPROPERTY(Replicated)
		FVector m_lastValidStepLocation_r;
	UPROPERTY(Replicated)
		FRotator m_lastValidStepRotation_r;
	UPROPERTY(Replicated)
		bool m_isFallingBackToLevel_r;
	UPROPERTY(Replicated)
		int m_playerIndex_r;
	FTimerHandle m_updateCameraRotationTimer;
    bool m_isFriendlyFire;
};

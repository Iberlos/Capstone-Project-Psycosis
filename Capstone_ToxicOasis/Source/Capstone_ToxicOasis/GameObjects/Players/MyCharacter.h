// Fill out your copyright notice in the Description page of Project Settings.

//Default Unreal Includes
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
//Custom Unreal includes #There shouldn't be any includes in the .h file, but if one is needed it goes here.#

//Custom project includes #There shouldn't be any includes in the .h file, but if one is needed it goes here.#
#include "EnumMaster/EnumMaster.h"

//Generated include #must be included last because unreal complains if there are any includes between it and the generated body thingy.#
#include "MyCharacter.generated.h"

/*************************************
Class: MyCharacter
Author: Luis Filipe Moraes
Created: october 2019
Last Modified: 4/feb/2020
Description: This is the base class for all character classes such as the enemies and the player and it allows for interaction with all components by means of interface functions such as TakeDamage. It also allows for the same basic functionality as the MyActor inheriting classes that al;lows for netowrked play of particles and sounds as well as mapped storing of the necessary assets.
References:
Change Log
**********
Date: unknown
Author: Luis Filipe Moraes
Verified By: Lucas Felix & Abdul Ali
Changes: Added all the functionlaity related to the base fucntionality of the class.
----------------------------------------------------------------------------------
Date: between 20/jan and 4/feb/2020
Author: Luis Filipe Moraes
Verified By: Lucas Felix & Abdul Ali
Changes: modified the base class to hold a map of animatiuonMontages to be played networked. Created the PlayAnimationMontage function and its networked siblings to allow animation play over the network.
----------------------------------------------------------------------------------
Date: between 2/7/2020
Author: Luis Filipe Moraes
Verified By: Lucas Felix & Abdul Ali
Changes: Normalized the lerped values between the MyCharacterCLass and the MyActorClass
----------------------------------------------------------------------------------
Date: between 2/14/2020
Author: Luis Filipe Moraes
Verified By: Lucas Felix & Abdul Ali
Changes: Reimplemented the Lifetime particle and sound maintenance in a timer delegate currently hard coded to 3 seconds
----------------------------------------------------------------------------------
Date: between 21/14/2020
Author: Luis Filipe Moraes
Verified By: Lucas Felix & Abdul Ali
Changes: Modified the logic for reflection in the lerped values. This implementation is starting to become too specific, but at least it works for the recoil now.
*************************************/

USTRUCT(BlueprintType)
struct FBeamInfoStruct_C
{
	GENERATED_BODY()

public: //REALLY?
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
		bool m_isActiveBeam;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
		FVector m_endPoint;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
		FVector m_startTangent;

	FBeamInfoStruct_C()
	{
		DisableBeam();
	}

	void EnableBeam(FVector a_endPoint, FVector a_startTangent = FVector::ZeroVector)
	{
		m_isActiveBeam = true;
		m_endPoint = a_endPoint;
		m_startTangent = a_startTangent;
	}

	void DisableBeam()
	{
		m_isActiveBeam = false;
		m_startTangent = FVector::ZeroVector;
		m_endPoint = FVector::ZeroVector;
	}
};

USTRUCT(BlueprintType)
struct FSoundStruct_C
{
	GENERATED_BODY()

public: //REALLY?
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
		class USoundCue* m_sound;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
		bool m_isPlayOnce;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
		bool m_isAlwaysPlayTheFirstTime;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
		float m_chanceToPlay;

	bool m_playedOnce = false;
};

USTRUCT(BlueprintType)
struct FParticleStruct_C
{
	GENERATED_BODY()

public: //REALLY?
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
		class UParticleSystem* m_particle;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
		bool m_isPlayOnce;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
		bool m_isAlwaysPlayTheFirstTime;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
		float m_chanceToPlay;

	bool m_playedOnce = false;
};

USTRUCT(BlueprintType)
struct FLerpedValueStruct_C
{
    GENERATED_BODY()

    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
    float m_currentValue;
    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
    float m_min;
    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
    float m_max;
    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
    float m_alpha;
    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
    float m_initialAlpha;
    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
    float m_targetAlpha;
    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
    float m_alphaSpeed;
    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
    bool m_reflect;
    //UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
    //float* m_updatedValue;
public:
    void Tick(float DeltaTime)
    {
        if (!FMath::IsNearlyEqual(m_alpha, m_targetAlpha))
        {
            m_alpha += FMath::Sign(m_targetAlpha - m_alpha) * m_alphaSpeed * DeltaTime;
        }
        else
        {
            if (m_alpha != m_targetAlpha)
            {
                m_alpha = m_targetAlpha;
            }
            else if (m_reflect)
            {
                //float temp = m_targetAlpha;
                //m_targetAlpha = m_initialAlpha;
                //m_initialAlpha = temp;
				m_alpha = 0;
				m_min = -m_min*0.5f;
                m_reflect = false;
            }
        }
        m_alpha = FMath::Clamp(m_alpha, 0.0f, 1.0f);
        m_currentValue = FMath::Lerp(m_min, m_max, m_alpha);
        //if (m_updatedValue) //FUTURE IMPLEMENTATION
        //{
        //    *m_updatedValue = m_currentValue;
        //}
    }

    float Get() { return m_currentValue; }
    void Init(float a_min, float a_max, float a_durationInSeconds = 1.0f, float a_initialAlpha = 0, float a_targetAlpha = 1, bool a_reflect = false/*, float* a_updateValue = nullptr*/)
    {
        m_min = a_min;
        m_max = a_max;
        SetLerpDuration(a_durationInSeconds);
        m_initialAlpha = FMath::Clamp(a_initialAlpha, 0.0f, 1.0f);
        m_alpha = m_initialAlpha;
        m_targetAlpha = FMath::Clamp(a_targetAlpha, 0.0f, 1.0f);
        m_reflect = a_reflect;

        //m_updatedValue = a_updateValue;
    }

    void SetTargetAlpha(float a_targetAlpha)
    {
        m_targetAlpha = FMath::Clamp(a_targetAlpha, 0.0f, 1.0f);
    }

    void SetAlphaSpeed(float a_alphaSpeed)
    {
        a_alphaSpeed = FMath::Clamp(a_alphaSpeed, 0.000001f, 999999.0f);
        m_alphaSpeed = a_alphaSpeed;
    }

    void SetLerpDuration(float a_durationInSeconds)
    {
        a_durationInSeconds = FMath::Clamp(a_durationInSeconds, 0.000001f, 999999.0f);
        m_alphaSpeed = 1.0f / a_durationInSeconds;
    }
};

USTRUCT(BlueprintType)
struct FAnimMontageLib_C
{
	GENERATED_BODY()
public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
		TArray<class UAnimMontage*> m_montages;
};

UCLASS()
class CAPSTONE_TOXICOASIS_API AMyCharacter : public ACharacter
{
	GENERATED_BODY()
public:
	//Essentials
	AMyCharacter();
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	virtual void Tick(float DeltaTime) override;
	//Other Methods
	void PlaySound(FName a_Key, bool a_playToAll, FVector a_location = FVector::ZeroVector);
#pragma region
private:
	UFUNCTION(Server, Reliable, WithVAlidation)
		void SERVER_PlaySound(FName a_Key, bool a_playToAll, FVector a_location = FVector::ZeroVector);
	bool SERVER_PlaySound_Validate(FName a_Key, bool a_playToAll, FVector a_location = FVector::ZeroVector) { return true; }
	UFUNCTION(Client, Reliable, WithVAlidation)
		void CLIENT_PlaySound(FName a_Key, float a_chanceValue, FVector a_location = FVector::ZeroVector);
	bool CLIENT_PlaySound_Validate(FName a_Key, float a_chanceValue, FVector a_location = FVector::ZeroVector) { return true; }
	UFUNCTION(NetMulticast, Reliable, WithVAlidation)
		void NET_PlaySound(FName a_Key, float a_chanceValue, FVector a_location = FVector::ZeroVector);
	bool NET_PlaySound_Validate(FName a_Key, float a_chanceValue, FVector a_location = FVector::ZeroVector) { return true; }
public:
#pragma endregion
    void StopSound();
#pragma region
private:
    UFUNCTION(Server, Reliable, WithVAlidation)
        void SERVER_StopSound();
    bool SERVER_StopSound_Validate() { return true; }
    UFUNCTION(NetMulticast, Reliable, WithVAlidation)
        void NET_StopSound();
    bool NET_StopSound_Validate() { return true; }
public:
#pragma endregion
	void PlayParticle(FName a_Key, bool a_playToAll = true, FTransform a_transform = FTransform::Identity);
#pragma region
private:
	UFUNCTION(Server, Reliable, WithVAlidation)
		void SERVER_PlayParticle(FName a_Key, bool a_playToAll = true, FTransform a_transform = FTransform::Identity);
	bool SERVER_PlayParticle_Validate(FName a_Key, bool a_playToAll = true, FTransform a_transform = FTransform::Identity) { return true; }
	UFUNCTION(Client, Reliable, WithVAlidation)
		void CLIENT_PlayParticle(FName a_Key, float a_chanceValue, FTransform a_transform = FTransform::Identity);
	bool CLIENT_PlayParticle_Validate(FName a_Key, float a_chanceValue, FTransform a_transform = FTransform::Identity) { return true; }
	UFUNCTION(NetMulticast, Reliable, WithVAlidation)
		void NET_PlayParticle(FName a_Key, float a_chanceValue, FTransform a_transform = FTransform::Identity);
	bool NET_PlayParticle_Validate(FName a_Key, float a_chanceValue, FTransform a_transform = FTransform::Identity) { return true; }
public:
#pragma endregion
	void StopParticle();
#pragma region
private:
	UFUNCTION(Server, Reliable, WithVAlidation)
		void SERVER_StopParticle();
	bool SERVER_StopParticle_Validate() { return true; }
	UFUNCTION(NetMulticast, Reliable, WithVAlidation)
		void NET_StopParticle();
	bool NET_StopParticle_Validate() { return true; }
public:
#pragma endregion
	virtual float PlayAnimationMontage(FName a_key, int a_index = -1);
#pragma region
protected:
	UFUNCTION(Server, Reliable, WithVAlidation)
		void SERVER_PlayAnimationMontage(FName a_key, int a_index);
	bool SERVER_PlayAnimationMontage_Validate(FName a_key, int a_index) { return true; }
	UFUNCTION(NetMulticast, Reliable, WithVAlidation)
		virtual void NET_PlayAnimationMontage(FName a_key, int a_index);
	bool NET_PlayAnimationMontage_Validate(FName a_key, int a_index) { return true; }
public:
#pragma endregion
	void ApplyDamage(float a_damageAmount, bool percentileDamage = false);
#pragma region
private:
	UFUNCTION(Server, Reliable, WithValidation)
		void SERVER_ApplyDamage(float a_damageAmount, bool percentileDamage);
	bool SERVER_ApplyDamage_Validate(float a_damageAmount, bool percentileDamage) { return true; }
public:
#pragma endregion
	void HealHealth(float a_healAmount, bool percentileDamage = false);
#pragma region
private:
	UFUNCTION(Server, Reliable, WithValidation)
		void SERVER_HealHealth(float a_healAmount, bool percentileDamage);
	bool SERVER_HealHealth_Validate(float a_healAmount, bool percentileDamage) { return true; }
public:
#pragma endregion
	//OnEvents

	//Components

	//Variables

	//Getters and Setters
    UFUNCTION(BlueprintCallable)
		class UInventoryComponent* GetInventoryComponent() { return m_inventoryComponent; }
    UFUNCTION(BlueprintCallable)
		class UObjectHealthComponent* GetHealthComponent() { return m_healthComponent; }
	bool GetHasTickedOnce() { return m_hasTickedOnce_r; }

protected:
	//Essentials
	virtual void BeginPlay() override;
	//Other Methods
	virtual void Die();
	UFUNCTION(BlueprintCallable)
		virtual void FinalizeDie();
	virtual void Respawn();
	void SetInputEnabled(bool a_state);
#pragma region
private:
	UFUNCTION(Client, Reliable, WithValidation)
		void CLIENT_SetInputEnabled(bool a_state);
	bool CLIENT_SetInputEnabled_Validate(bool a_state) { return true; }
protected:
#pragma endregion
#pragma region
private:
	UFUNCTION(Server, Reliable, WithValidation)
		void SERVER_NotifyFirstTick();
	bool SERVER_NotifyFirstTick_Validate() { return true; }
protected:
#pragma endregion
	void MaintainLifetimeParticleAndSound();
	//OnEvents

	//Components
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category = "Dev. Data|My Character Data|Components")
		class UAudioComponent* m_audioComponent_MYCHARACTER;
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category = "Dev. Data|My Character Data|Components")
		class UParticleSystemComponent* m_particleSystemComponent_MYCHARACTER;
	UPROPERTY(EditDefaultsOnly, Category = "Dev. Data|My Character Data|Components")
		class UObjectHealthComponent* m_healthComponent;
	UPROPERTY(EditDefaultsOnly, Category = "Dev. Data|My Character Data|Components")
		class UInventoryComponent* m_inventoryComponent;
	//Variables
	UPROPERTY(EditDefaultsOnly, Category = "Dev. Data|My Character Data|Variables")
		TMap<FName, FSoundStruct_C> m_soundCues;
	UPROPERTY(EditDefaultsOnly, Category = "Dev. Data|My Character Data|Variables")
		TMap<FName, FParticleStruct_C> m_particleFXs;
	UPROPERTY(EditDefaultsOnly, Category = "Dev. Data|My Character Data|Variables")
		TMap<FName, FAnimMontageLib_C> m_animationMontages_TPM;
	UPROPERTY(Replicated)
		FBeamInfoStruct_C m_beamParticleInfo_r;
	UPROPERTY(EditDefaultsOnly, Category = "Dev. Data|My Character Data|Variables")
		float m_baseTurnRate;
	UPROPERTY(EditDefaultsOnly, Category = "Dev. Data|My Character Data|Variables")
		float m_baseMoveSpeed;
        TMap<FName, FLerpedValueStruct_C> m_lerpedValues;
	bool m_hasDied;
	UPROPERTY(Replicated)
		bool m_hasTickedOnce_r;
};

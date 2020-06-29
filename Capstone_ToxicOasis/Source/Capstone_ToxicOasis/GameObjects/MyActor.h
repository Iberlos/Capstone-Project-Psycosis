// Fill out your copyright notice in the Description page of Project Settings.

//Default Unreal Includes
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
//Custom Unreal includes #There shouldn't be any includes in the .h file, but if one is needed it goes here.#

//Custom project includes #There shouldn't be any includes in the .h file, but if one is needed it goes here.#
#include "EnumMaster/EnumMaster.h"

//Generated include #must be included last because unreal complains if there are any includes between it and the generated body thingy.#
#include "MyActor.generated.h"

/*************************************
Class: MyActor
Author: Luis Filipe Moraes
Created: october 2019
Last Modified: 31/jan/2020
Description: Base class for all actor classes in the game such as interactables, this class allows the subclasses to store a map of Audio, Animations, Particles etc and play them in a networked setting.
References:
Change Log
**********
Date: unknown
Author: Luis Filipe Moraes
Verified By: Lucas Felix & Abdul Ali
Changes: Added all the data and functionality for playing the Sounds and particles needed.
--------------------------------------------------------------------
Date: between 10/jan and 31/jan/2020
Author: Luis Filipe Moraes
Verified By: Lucas Felix & Abdul Ali
Changes: modified the class to allow fire, draw, Holster, animation montages, also modified to allow the isFreindlyFire to be passed down from the firingPlayer to the projectiles.
----------------------------------------------------------------------------------
Date: between 2/72020
Author: Luis Filipe Moraes
Verified By: Lucas Felix & Abdul Ali
Changes: Modified the lerped values workings by adding functionality to set them and tick them correctly
*************************************/

USTRUCT(BlueprintType)
struct FBeamInfoStruct_A
{
	GENERATED_BODY()

public: //REALLY?
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
		bool m_isActiveBeam;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
		FVector m_endPoint;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
		FVector m_startTangent;

	FBeamInfoStruct_A()
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
struct FSoundStruct_A
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
struct FParticleStruct_A
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
struct FLerpedValueStruct_A
{
	GENERATED_BODY()

	//for some reason structs are private by default in Unreal
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
	//STORING A FUNCTION POINTER TO THE SETTER DEFINED BY THE CLIENT WOULD MAKE THIS IMPLEMENTATION COMPLETE IN MY OPPINION
	//PERHAPS HAVING A WAY FOR THE LerpedValue TO SELF DEESTRUCT AND CLEAN UP ATER ITSELF IF IT IS CONFIGURED TO SO SO WOULD BE GOOD TOO
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
                float temp = m_targetAlpha;
                m_targetAlpha = m_initialAlpha;
                m_initialAlpha = m_targetAlpha;
                m_reflect = false;
            }
        }
        m_alpha = FMath::Clamp(m_alpha, 0.0f, 1.0f);
        m_currentValue = FMath::Lerp(m_min, m_max, m_alpha);
        //IF(SETTER FUNCTION)
        //{
        //    CALL SETTER FUNCTION PASSING IN m_currentValue;
        //}
    }

    float Get() { return m_currentValue; }
    void Init(float a_min, float a_max, float a_durationInSeconds = 1.0f, float a_initialAlpha = 0, float a_targetAlpha = 1, bool a_reflect = false)
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
struct FAnimMontageLib_A
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
		TArray<class UAnimMontage*> m_montages;
};

UCLASS()
class CAPSTONE_TOXICOASIS_API AMyActor : public AActor
{
	GENERATED_BODY()
	
public:
	//Essentials
	AMyActor();
	virtual void Tick(float DeltaTime) override;
	//Other Methods
	void PlaySound(FName a_Key, bool a_playToAll = true, FVector a_location = FVector::ZeroVector);
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
	//OnEvents

	//Components

	//Variables

    //Getters/Setters
    class UBoxComponent* GetColliderComponent() { return m_colliderComponent; }

protected:
	//Essentials
	virtual void BeginPlay() override;

	//Other Methods
	void SetParticleAtKey(FName a_Key, UParticleSystem* a_particleFX)
	{
		if (m_particleFXs.Contains(a_Key))
			m_particleFXs[a_Key].m_particle = a_particleFX;
	}

	void SetSoundAtKey(FName a_Key, USoundCue* a_soundCue)
	{
		if (m_soundCues.Contains(a_Key))
			m_soundCues[a_Key].m_sound = a_soundCue;
	}
	//OnEvents

	//Components
	UPROPERTY(EditDefaultsOnly, Category = "Dev. Data|My Actor Data|Components")
		class UBoxComponent* m_colliderComponent;
	UPROPERTY(EditDefaultsOnly, Category = "Dev. Data|My Actor Data|Components")
		class USkeletalMeshComponent* m_skeletalMeshComponent;
	UPROPERTY(EditDefaultsOnly, Category = "Dev. Data|My Actor Data|Components")
		class UAudioComponent* m_audioComponent;
	UPROPERTY(EditDefaultsOnly, Category = "Dev. Data|My Actor Data|Components")
		class UParticleSystemComponent* m_particleSystemComponent;

	//Variables
	UPROPERTY(EditDefaultsOnly, Category = "Dev. Data|My Actor Data|Variables")
		TMap<FName, FSoundStruct_A> m_soundCues;
	UPROPERTY(EditDefaultsOnly, Category = "Dev. Data|My Actor Data|Variables")
		TMap<FName, FParticleStruct_A> m_particleFXs;
	UPROPERTY(EditDefaultsOnly, Category = "Dev. Data|My Actor Data|Variables")
		TMap<FName, FAnimMontageLib_A> m_animationMontages;
	UPROPERTY(Replicated)
		FBeamInfoStruct_A m_beamParticleInfo_r;
		TMap<FName, FLerpedValueStruct_A> m_lerpedValues;
};

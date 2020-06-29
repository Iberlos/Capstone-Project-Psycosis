// Fill out your copyright notice in the Description page of Project Settings.

//Default Unreal Includes
#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
//Custom Unreal includes #There shouldn't be any includes in the .h file, but if one is needed it goes here.#

//Custom project includes #There shouldn't be any includes in the .h file, but if one is needed it goes here.#
#include "EnumMaster/EnumMaster.h"

//Generated include #must be included last because unreal complains if there are any includes between it and the generated body thingy.#
#include "MyActorComponent.generated.h"

USTRUCT(BlueprintType)
struct FBeamInfoStruct_AC
{
	GENERATED_BODY()

public: //REALLY?
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
		bool m_isActiveBeam;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
		FVector m_endPoint;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
		FVector m_startTangent;

	FBeamInfoStruct_AC()
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
struct FSoundStruct_AC
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
struct FParticleStruct_AC
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

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class CAPSTONE_TOXICOASIS_API UMyActorComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	//Essentials
	UMyActorComponent();
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
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
	//OnEvents
	//Components
	//Variables

protected:
	//Essentials
	virtual void BeginPlay() override;
	//Other Methods
#pragma region
private:
	//...
protected:
#pragma endregion
	//OnEvents
	//Components
	UPROPERTY(EditDefaultsOnly, Category = "Dev. Data")
		class UAudioComponent* m_audioComponent;
	UPROPERTY(EditDefaultsOnly, Category = "Dev. Data")
		class UParticleSystemComponent* m_particleSystemComponent;
	//Variables
	UPROPERTY(EditDefaultsOnly, Category = "Dev. Data")
		TMap<FName, FSoundStruct_AC> m_soundCues;
	UPROPERTY(EditDefaultsOnly, Category = "Dev. Data")
		TMap<FName, FParticleStruct_AC> m_particleFXs;
	UPROPERTY(Replicated)
		FBeamInfoStruct_AC m_beamParticleInfo_r;
};

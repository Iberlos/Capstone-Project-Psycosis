// Fill out your copyright notice in the Description page of Project Settings.

//Header include
#include "MyActor.h"

//Custom Engine Includes
#include "Animation/AnimInstance.h"
#include "Animation/AnimMontage.h"
#include "Components/BoxComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/AudioComponent.h"
#include "Particles/ParticleSystemComponent.h"
#include "Sound/SoundCue.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"

//Custom Project Includes

// Sets default values
AMyActor::AMyActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	//Setup engine components
	m_colliderComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("Main Collider"));
	m_colliderComponent->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	m_colliderComponent->SetCollisionProfileName("ActorCollision");
	m_colliderComponent->SetSimulatePhysics(true);
	m_colliderComponent->SetIsReplicated(true);
	RootComponent = m_colliderComponent;
	m_skeletalMeshComponent = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Main Mesh"));
	m_skeletalMeshComponent->SetIsReplicated(true);
	m_skeletalMeshComponent->SetupAttachment(RootComponent);
	m_audioComponent = CreateDefaultSubobject<UAudioComponent>("Audio Component");
	m_audioComponent->SetupAttachment(RootComponent);
	m_audioComponent->SetIsReplicated(true);
	m_particleSystemComponent = CreateDefaultSubobject <UParticleSystemComponent >("Particle Component");
	m_particleSystemComponent->SetupAttachment(RootComponent);
	m_particleSystemComponent->SetIsReplicated(true);

	//Add Cues Related to the MyActor base class
	m_soundCues.Add("Spawn");
	m_soundCues.Add("Lifetime");
	m_soundCues.Add("Destroy");

	//Add partciles Related to the MyActor base class
	m_particleFXs.Add("Spawn");
	m_particleFXs.Add("Lifetime");
	m_particleFXs.Add("Destroy");

	//Setup Replication
	bAlwaysRelevant = false;
	NetPriority = 1;
	SetReplicates(true);
	SetReplicateMovement(true);
}

// Called when the game starts or when spawned
void AMyActor::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void AMyActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	//Apply Beam info to particle system if active
	if (m_beamParticleInfo_r.m_isActiveBeam)
	{
		m_particleSystemComponent->SetBeamTargetPoint(0, m_particleSystemComponent->GetComponentLocation() + m_beamParticleInfo_r.m_endPoint, 0);
		if (m_beamParticleInfo_r.m_startTangent != FVector::ZeroVector)
		{
			m_particleSystemComponent->SetBeamSourceTangent(0, m_beamParticleInfo_r.m_startTangent, 0);
		}
	}

	//Tick all the lerped values in the m_LerpedValues map
	TArray<FName> outKeys;
	m_lerpedValues.GetKeys(outKeys);
	for (FName key : outKeys)
	{
		m_lerpedValues[key].Tick(DeltaTime);
	}
}

void AMyActor::PlaySound(FName a_Key, bool a_playToAll, FVector a_location)
{
	if (m_soundCues.Contains(a_Key))
	{
		if (m_soundCues[a_Key].m_sound)
		{
			SERVER_PlaySound(a_Key, a_playToAll, a_location);
		}
	}
}

void AMyActor::SERVER_PlaySound_Implementation(FName a_Key, bool a_playToAll, FVector a_location)
{
	int chance = FMath::FRandRange(0, 100);
	if (a_playToAll)
	{
		NET_PlaySound(a_Key, chance, a_location);
	}
	else
	{
		CLIENT_PlaySound(a_Key, chance, a_location);
	}
}

void AMyActor::CLIENT_PlaySound_Implementation(FName a_Key, float a_chanceValue, FVector a_location)
{
	if (m_soundCues[a_Key].m_isPlayOnce && m_soundCues[a_Key].m_playedOnce)
	{
		return;
	}
	else
	{
		if (a_chanceValue < m_soundCues[a_Key].m_chanceToPlay || (m_soundCues[a_Key].m_isAlwaysPlayTheFirstTime && !m_soundCues[a_Key].m_playedOnce))
		{
			m_soundCues[a_Key].m_playedOnce = true;
			if (a_location != FVector::ZeroVector)
			{
				UGameplayStatics::PlaySoundAtLocation(this , m_soundCues[a_Key].m_sound, a_location);
			}
			else
			{
				m_audioComponent->SetSound(m_soundCues[a_Key].m_sound);
				m_audioComponent->Play();
			}
		}
	}
}

void AMyActor::NET_PlaySound_Implementation(FName a_Key, float a_chanceValue, FVector a_location)
{
	if (m_soundCues[a_Key].m_isPlayOnce && m_soundCues[a_Key].m_playedOnce)
	{
		return;
	}
	else
	{
		if (a_chanceValue < m_soundCues[a_Key].m_chanceToPlay || (m_soundCues[a_Key].m_isAlwaysPlayTheFirstTime && !m_soundCues[a_Key].m_playedOnce))
		{
			m_soundCues[a_Key].m_playedOnce = true;
			if (a_location != FVector::ZeroVector)
			{
				UGameplayStatics::PlaySoundAtLocation(this, m_soundCues[a_Key].m_sound, a_location);
			}
			else
			{
				m_audioComponent->SetSound(m_soundCues[a_Key].m_sound);
				m_audioComponent->Play();
			}
		}
	}
}

void AMyActor::StopSound()
{
    SERVER_StopSound();
}

void AMyActor::SERVER_StopSound_Implementation()
{
    NET_StopSound();
}

void AMyActor::NET_StopSound_Implementation()
{
    m_audioComponent->Stop();
}

void AMyActor::PlayParticle(FName a_Key, bool a_playToAll, FTransform a_transform)
{
	if (m_particleFXs.Contains(a_Key))
	{
		if (m_particleFXs[a_Key].m_particle)
		{
			SERVER_PlayParticle(a_Key, a_playToAll, a_transform);
		}
	}
}

void AMyActor::SERVER_PlayParticle_Implementation(FName a_Key, bool a_playToAll, FTransform a_transform)
{
	int chance = FMath::FRandRange(0, 100);
	if (a_playToAll)
	{
		NET_PlayParticle(a_Key, chance, a_transform);
	}
	else
	{
		CLIENT_PlayParticle(a_Key, chance, a_transform);
	}
}

void AMyActor::CLIENT_PlayParticle_Implementation(FName a_Key, float a_chanceValue, FTransform a_transform)
{
	if (m_particleFXs[a_Key].m_isPlayOnce && m_particleFXs[a_Key].m_playedOnce)
	{
		return;
	}
	else
	{
		if (a_chanceValue < m_particleFXs[a_Key].m_chanceToPlay || (m_particleFXs[a_Key].m_isAlwaysPlayTheFirstTime && !m_particleFXs[a_Key].m_playedOnce))
		{
			m_particleFXs[a_Key].m_playedOnce = true;
			if (a_transform.GetLocation() != FTransform::Identity.GetLocation())
			{
				UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), m_particleFXs[a_Key].m_particle, a_transform);
			}
			else
			{
				m_particleSystemComponent->SetTemplate(m_particleFXs[a_Key].m_particle);
			}
		}
	}
}

void AMyActor::NET_PlayParticle_Implementation(FName a_Key, float a_chanceValue, FTransform a_transform)
{
	if (m_particleFXs[a_Key].m_isPlayOnce && m_particleFXs[a_Key].m_playedOnce)
	{
		return;
	}
	else
	{
		if (a_chanceValue < m_particleFXs[a_Key].m_chanceToPlay || (m_particleFXs[a_Key].m_isAlwaysPlayTheFirstTime && !m_particleFXs[a_Key].m_playedOnce))
		{
			m_particleFXs[a_Key].m_playedOnce = true;
			if (a_transform.GetLocation() != FTransform::Identity.GetLocation())
			{
				UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), m_particleFXs[a_Key].m_particle, a_transform);
			}
			else
			{
				m_particleSystemComponent->SetTemplate(m_particleFXs[a_Key].m_particle);
			}
		}
	}
}

void AMyActor::StopParticle()
{
	SERVER_StopParticle();
}

void AMyActor::SERVER_StopParticle_Implementation()
{
	NET_StopParticle();
}

void AMyActor::NET_StopParticle_Implementation()
{
	m_particleSystemComponent->DeactivateSystem();
	m_particleSystemComponent->SetBeamSourcePoint(0, FVector::ZeroVector, 0);
	m_particleSystemComponent->SetBeamTargetPoint(0, FVector::ZeroVector, 0);
	m_beamParticleInfo_r.DisableBeam();
}

float AMyActor::PlayAnimationMontage(FName a_key, int a_index)
{
	if (m_animationMontages.Contains(a_key))
	{
		if (a_index == -1 && m_animationMontages[a_key].m_montages.Num() != 0)
		{
			a_index = FMath::RandRange(0, m_animationMontages[a_key].m_montages.Num() - 1);
		}
		if (m_animationMontages[a_key].m_montages.IsValidIndex(a_index) && m_animationMontages[a_key].m_montages[a_index] != nullptr)
		{
			SERVER_PlayAnimationMontage(a_key, a_index);
			return m_animationMontages[a_key].m_montages[a_index]->GetPlayLength();
		}
		return 0.0f;
	}
	return 0.0f;
}

void AMyActor::SERVER_PlayAnimationMontage_Implementation(FName a_key, int a_index)
{
	NET_PlayAnimationMontage(a_key, a_index);
}

void AMyActor::NET_PlayAnimationMontage_Implementation(FName a_key, int a_index)
{
	if (m_animationMontages.Contains(a_key) && m_animationMontages[a_key].m_montages.IsValidIndex(a_index) && m_animationMontages[a_key].m_montages[a_index] != nullptr)
	{
		UAnimInstance* animInstance = m_skeletalMeshComponent->GetAnimInstance();
		if (animInstance)
		{
			animInstance->Montage_Play(m_animationMontages[a_key].m_montages[a_index], 1.0f);
		}
	}
}

void AMyActor::GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AMyActor, m_beamParticleInfo_r)
}


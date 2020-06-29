// Fill out your copyright notice in the Description page of Project Settings.

//Header include
#include "MyActorComponent.h"

//Custom Engine Includes
#include "Components/AudioComponent.h"
#include "Particles/ParticleSystemComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"

//Custom Project Includes


UMyActorComponent::UMyActorComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	//Setup engine components
	m_audioComponent = CreateDefaultSubobject<UAudioComponent>("Audio Component");
	m_audioComponent->SetIsReplicated(true);
	m_particleSystemComponent = CreateDefaultSubobject <UParticleSystemComponent >("Particle Component");
	m_particleSystemComponent->SetIsReplicated(true);

	//Add Cues Related to the MyActorComponent base class
	m_soundCues.Add("Spawn");
	m_soundCues.Add("Lifetime");
	m_soundCues.Add("Destroy");

	//Add partciles Related to the MyActorComponent base class
	m_particleFXs.Add("Spawn");
	m_particleFXs.Add("Lifetime");
	m_particleFXs.Add("Destroy");
}

// Called when the game starts
void UMyActorComponent::BeginPlay()
{
	Super::BeginPlay();

	m_audioComponent->AttachToComponent(GetOwner()->GetRootComponent(), FAttachmentTransformRules::SnapToTargetIncludingScale);
	m_particleSystemComponent->AttachToComponent(GetOwner()->GetRootComponent(), FAttachmentTransformRules::SnapToTargetIncludingScale);
}

// Called every frame
void UMyActorComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	//Apply Beam info to particle system if active
	if (m_beamParticleInfo_r.m_isActiveBeam)
	{
		m_particleSystemComponent->SetBeamTargetPoint(0, m_particleSystemComponent->GetComponentLocation() + m_beamParticleInfo_r.m_endPoint, 0);
		if (m_beamParticleInfo_r.m_startTangent != FVector::ZeroVector)
		{
			m_particleSystemComponent->SetBeamSourceTangent(0, m_beamParticleInfo_r.m_startTangent, 0);
		}
	}
}

void UMyActorComponent::PlaySound(FName a_Key, bool a_playToAll, FVector a_location)
{
	if (m_soundCues.Contains(a_Key))
	{
		if (m_soundCues[a_Key].m_sound)
		{
			SERVER_PlaySound(a_Key, a_playToAll, a_location);
		}
	}
}

void UMyActorComponent::SERVER_PlaySound_Implementation(FName a_Key, bool a_playToAll, FVector a_location)
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

void UMyActorComponent::CLIENT_PlaySound_Implementation(FName a_Key, float a_chanceValue, FVector a_location)
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
				//UGameplayStatics::PlaySoundAtLocation(GetOwner(), m_soundCues[a_Key].m_sound, a_location);
			}
			else
			{
				//m_audioComponent->SetSound(m_soundCues[a_Key].m_sound);
				//m_audioComponent->Play();
			}
		}
	}
}

void UMyActorComponent::NET_PlaySound_Implementation(FName a_Key, float a_chanceValue, FVector a_location)
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
				//UGameplayStatics::PlaySoundAtLocation(GetOwner(), m_soundCues[a_Key].m_sound, a_location);
			}
			else
			{
				//m_audioComponent->SetSound(m_soundCues[a_Key].m_sound);
				//m_audioComponent->Play();
			}
		}
	}
}

void UMyActorComponent::StopSound()
{
    SERVER_StopSound();
}

void UMyActorComponent::SERVER_StopSound_Implementation()
{
    NET_StopSound();
}

void UMyActorComponent::NET_StopSound_Implementation()
{
    m_audioComponent->Stop();
}

void UMyActorComponent::PlayParticle(FName a_Key, bool a_playToAll, FTransform a_transform)
{
	if (m_particleFXs.Contains(a_Key))
	{
		if (m_particleFXs[a_Key].m_particle)
		{
			SERVER_PlayParticle(a_Key, a_playToAll, a_transform);
		}
	}
}

void UMyActorComponent::SERVER_PlayParticle_Implementation(FName a_Key, bool a_playToAll, FTransform a_transform)
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

void UMyActorComponent::CLIENT_PlayParticle_Implementation(FName a_Key, float a_chanceValue, FTransform a_transform)
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
				UGameplayStatics::SpawnEmitterAtLocation(GetOwner()->GetWorld(), m_particleFXs[a_Key].m_particle, a_transform);
			}
			else
			{
				m_particleSystemComponent->SetTemplate(m_particleFXs[a_Key].m_particle);
			}
		}
	}
}

void UMyActorComponent::NET_PlayParticle_Implementation(FName a_Key, float a_chanceValue, FTransform a_transform)
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
				UGameplayStatics::SpawnEmitterAtLocation(GetOwner()->GetWorld(), m_particleFXs[a_Key].m_particle, a_transform);
			}
			else
			{
				m_particleSystemComponent->SetTemplate(m_particleFXs[a_Key].m_particle);
			}
		}
	}
}


void UMyActorComponent::StopParticle()
{
	SERVER_StopParticle();
}

void UMyActorComponent::SERVER_StopParticle_Implementation()
{
	NET_StopParticle();
}

void UMyActorComponent::NET_StopParticle_Implementation()
{
	m_particleSystemComponent->DeactivateSystem();
	m_particleSystemComponent->SetBeamSourcePoint(0, FVector::ZeroVector, 0);
	m_particleSystemComponent->SetBeamTargetPoint(0, FVector::ZeroVector, 0);
	m_beamParticleInfo_r.DisableBeam();
}

void UMyActorComponent::GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UMyActorComponent, m_beamParticleInfo_r)
}


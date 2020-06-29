// Fill out your copyright notice in the Description page of Project Settings.

//Header include
#include "MyCharacter.h"

//Custom Engine Includes
#include "Components/AudioComponent.h"
#include "Particles/ParticleSystemComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"

//Custom Project Includes
#include "../../Custom_Components/ObjectHealthComponent.h"
#include "../../Custom_Components/InventoryComponent.h"

// Sets default values
AMyCharacter::AMyCharacter()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	//Setup engine components
	GetCapsuleComponent()->SetCollisionProfileName("CharacterCollision");
	Cast<UPrimitiveComponent>(RootComponent)->bReplicatePhysicsToAutonomousProxy = false;
	m_audioComponent_MYCHARACTER = CreateDefaultSubobject<UAudioComponent>("Audio Component");
	m_audioComponent_MYCHARACTER->SetupAttachment(RootComponent);
	m_audioComponent_MYCHARACTER->SetIsReplicated(true);
	m_particleSystemComponent_MYCHARACTER = CreateDefaultSubobject <UParticleSystemComponent >("Particle Component");
	m_particleSystemComponent_MYCHARACTER->SetupAttachment(RootComponent);
	m_particleSystemComponent_MYCHARACTER->SetIsReplicated(true);

	//Initialize Custom Components
	m_healthComponent = CreateDefaultSubobject<UObjectHealthComponent>("Health Component");
    m_healthComponent->SetNetAddressable();
    m_healthComponent->SetIsReplicated(true);
	m_inventoryComponent = CreateDefaultSubobject<UInventoryComponent>("Inventory Component");
	m_inventoryComponent->SetIsReplicated(true);

	//Add Cues Related to the MyCharacter base class
	m_soundCues.Add("Spawn");
	m_soundCues.Add("Lifetime");
	m_soundCues.Add("Destroy");
	m_soundCues.Add("Hurt");
	m_soundCues.Add("Die");

	//Add partciles Related to the MyCharacter base class
	m_particleFXs.Add("Spawn");
	m_particleFXs.Add("Lifetime");
	m_particleFXs.Add("Destroy");
	m_particleFXs.Add("Hurt");
	m_particleFXs.Add("Die");

	//Add anim montages related to the MyCharacter base class
	m_animationMontages_TPM.Add("Spawn");
	m_animationMontages_TPM.Add("Die");

	//Setup Replication
	bAlwaysRelevant = false;
	NetPriority = 2.5;
	SetReplicates(true);
	SetReplicateMovement(true);


}

// Called to bind functionality to input
void AMyCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

// Called when the game starts or when spawned
void AMyCharacter::BeginPlay()
{
	Super::BeginPlay();

	Respawn();

	FTimerHandle t;
	GetWorldTimerManager().SetTimer(t, this, &AMyCharacter::MaintainLifetimeParticleAndSound, 3.0f, true, 3.0f);
}

// Called every frame
void AMyCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (GetController() && GetController()->IsLocalController())
	{
		if (!m_hasTickedOnce_r)
		{
			m_hasTickedOnce_r = true;
			SERVER_NotifyFirstTick();
		}
	}

	if (GetLocalRole() == ROLE_Authority)
	{
		if (m_healthComponent->CheckIfDead() && !m_hasDied)
		{
			Die();
		}
	}

	//Apply Beam info to particle system if active
	if (m_beamParticleInfo_r.m_isActiveBeam)
	{
		m_particleSystemComponent_MYCHARACTER->SetBeamTargetPoint(0, m_particleSystemComponent_MYCHARACTER->GetComponentLocation() + m_beamParticleInfo_r.m_endPoint, 0);
		if (m_beamParticleInfo_r.m_startTangent != FVector::ZeroVector)
		{
			m_particleSystemComponent_MYCHARACTER->SetBeamSourceTangent(0, m_beamParticleInfo_r.m_startTangent, 0);
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

void AMyCharacter::PlaySound(FName a_Key, bool a_playToAll, FVector a_location)
{
	if (m_soundCues.Contains(a_Key))
	{
		if (m_soundCues[a_Key].m_sound)
		{
			SERVER_PlaySound(a_Key, a_playToAll, a_location);
		}
	}
}

void AMyCharacter::SERVER_PlaySound_Implementation(FName a_Key, bool a_playToAll, FVector a_location)
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

void AMyCharacter::CLIENT_PlaySound_Implementation(FName a_Key, float a_chanceValue, FVector a_location)
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
				m_audioComponent_MYCHARACTER->SetSound(m_soundCues[a_Key].m_sound);
				m_audioComponent_MYCHARACTER->Play();
			}
		}
	}
}

void AMyCharacter::NET_PlaySound_Implementation(FName a_Key, float a_chanceValue, FVector a_location)
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
				m_audioComponent_MYCHARACTER->SetSound(m_soundCues[a_Key].m_sound);
				m_audioComponent_MYCHARACTER->Play();
			}
		}
	}
}

void AMyCharacter::StopSound()
{
    SERVER_StopSound();
}

void AMyCharacter::SERVER_StopSound_Implementation()
{
    NET_StopSound();
}

void AMyCharacter::NET_StopSound_Implementation()
{
    m_audioComponent_MYCHARACTER->Stop();
}

void AMyCharacter::PlayParticle(FName a_Key, bool a_playToAll, FTransform a_transform)
{
	if (m_particleFXs.Contains(a_Key))
	{
		if (m_particleFXs[a_Key].m_particle)
		{
			SERVER_PlayParticle(a_Key, a_playToAll, a_transform);
		}
	}
}

void AMyCharacter::SERVER_PlayParticle_Implementation(FName a_Key, bool a_playToAll, FTransform a_transform)
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

void AMyCharacter::CLIENT_PlayParticle_Implementation(FName a_Key, float a_chanceValue, FTransform a_transform)
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
				m_particleSystemComponent_MYCHARACTER->SetTemplate(m_particleFXs[a_Key].m_particle);
			}
		}
	}
}

void AMyCharacter::NET_PlayParticle_Implementation(FName a_Key, float a_chanceValue, FTransform a_transform)
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
				m_particleSystemComponent_MYCHARACTER->SetTemplate(m_particleFXs[a_Key].m_particle);
			}
		}
	}
}

void AMyCharacter::StopParticle()
{
	SERVER_StopParticle();
}

void AMyCharacter::SERVER_StopParticle_Implementation()
{
	NET_StopParticle();
}

void AMyCharacter::NET_StopParticle_Implementation()
{
	m_particleSystemComponent_MYCHARACTER->DeactivateSystem();
	m_particleSystemComponent_MYCHARACTER->SetBeamSourcePoint(0, FVector::ZeroVector, 0);
	m_particleSystemComponent_MYCHARACTER->SetBeamTargetPoint(0, FVector::ZeroVector, 0);
	m_beamParticleInfo_r.DisableBeam();
}

float AMyCharacter::PlayAnimationMontage(FName a_key, int a_index)
{
	if (m_animationMontages_TPM.Contains(a_key))
	{
		if (a_index == -1 && m_animationMontages_TPM[a_key].m_montages.Num() != 0)
		{
			a_index = FMath::RandRange(0, m_animationMontages_TPM[a_key].m_montages.Num() - 1);
		}
		if (m_animationMontages_TPM[a_key].m_montages.IsValidIndex(a_index) && m_animationMontages_TPM[a_key].m_montages[a_index] != nullptr)
		{
			SERVER_PlayAnimationMontage(a_key, a_index);
			return m_animationMontages_TPM[a_key].m_montages[a_index]->GetPlayLength();
		}
		return 0.0f;
	}
	return 0.0f;
}

void AMyCharacter::SERVER_PlayAnimationMontage_Implementation(FName a_key, int a_index)
{
	NET_PlayAnimationMontage(a_key, a_index);
}

void AMyCharacter::NET_PlayAnimationMontage_Implementation(FName a_key, int a_index)
{
	if (m_animationMontages_TPM.Contains(a_key) && m_animationMontages_TPM[a_key].m_montages.IsValidIndex(a_index) && m_animationMontages_TPM[a_key].m_montages[a_index] != nullptr)
	{
		UAnimInstance* animInstance = GetMesh()->GetAnimInstance();
		if (animInstance)
		{
			animInstance->Montage_Play(m_animationMontages_TPM[a_key].m_montages[a_index], 1.0f);
		}
	}
}

void AMyCharacter::ApplyDamage(float a_damageAmount, bool percentileDamage)
{
	SERVER_ApplyDamage(a_damageAmount, percentileDamage);
}

void AMyCharacter::SERVER_ApplyDamage_Implementation(float a_damageAmount, bool percentileDamage)
{
	PlayParticle("Hurt");
	PlaySound("Hurt", true);
    m_healthComponent->SubtractHealth(a_damageAmount, percentileDamage);   
}

void AMyCharacter::HealHealth(float a_healAmount, bool percentileDamage)
{
	SERVER_HealHealth(a_healAmount, percentileDamage);
}

void AMyCharacter::SERVER_HealHealth_Implementation(float a_healAmount, bool percentileDamage)
{
    m_healthComponent->AddHealth(a_healAmount, percentileDamage);
}

void AMyCharacter::Die()
{
	m_hasDied = true;
	PlaySound("Die", true, GetActorLocation());
	PlayParticle("Die", true, GetActorTransform());
	PlayAnimationMontage("Die");
}

void AMyCharacter::FinalizeDie()
{
}

void AMyCharacter::Respawn()
{
	m_hasDied = false;
	PlaySound("Spawn", true, GetActorLocation());
	PlayParticle("Spawn", true, GetActorTransform());
	PlayAnimationMontage("Spawn");
}

void AMyCharacter::SetInputEnabled(bool a_state)
{
	CLIENT_SetInputEnabled(a_state);
}

void AMyCharacter::CLIENT_SetInputEnabled_Implementation(bool a_state)
{
	if (a_state)
	{
		EnableInput(Cast<APlayerController>(GetController()));
	}
	else
	{
		DisableInput(Cast<APlayerController>(GetController()));
	}
}


void AMyCharacter::SERVER_NotifyFirstTick_Implementation()
{
	m_hasTickedOnce_r = true;
}

void AMyCharacter::MaintainLifetimeParticleAndSound()
{
	//Maintain Lifetime sound and particles
	if (!m_particleSystemComponent_MYCHARACTER->Template)
	{
		PlayParticle("Lifetime", true);
	}
	if (!m_audioComponent_MYCHARACTER->IsPlaying())
	{
		PlaySound("Lifetime", true);
	}
}

void AMyCharacter::GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AMyCharacter, m_beamParticleInfo_r)
		DOREPLIFETIME(AMyCharacter, m_hasTickedOnce_r)
}
// Fill out your copyright notice in the Description page of Project Settings.


#include "BossPawn.h"
// Components
#include "Components/BoxComponent.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
//// Mine
#include "Custom_Components/Boss_State_Components/BossStateBaseComponent.h"
//#include "Custom_Components/Boss_State_Components/BossStateDenialComponent.h"
#include "Custom_Components/Boss_State_Components/BossStateAngerComponent.h"
#include "Custom_Components/Boss_State_Components/BossStateBargainComponent.h"
#include "Custom_Components/Boss_State_Components/BossStateSadComponent.h"
#include "Custom_Components//ObjectHealthComponent.h"
#include "GameObjects/Players/PlayerCharacter.h"

#include "Animation/AnimationAsset.h"

#include "Engine/World.h"
#include "TimerManager.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/SphereComponent.h"

// Sets default values
ABossPawn::ABossPawn()
{
	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

// Set up member variables
#pragma region Boss Mesh
	// Boss Mesh (Temporary)
	m_BossTempMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Temp Skeletal Mesh"));
	SetRootComponent(m_BossTempMesh);
#pragma endregion

#pragma region Basic Collision Box
	// Boss Basic Collision Hitbox
	m_bossBasicCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("Basic Collision Box"));
	// Size and Location
	m_bossBasicCollision->SetBoxExtent(FVector(400.0f, 400.0f, 400.0f));
	m_bossBasicCollision->SetRelativeLocation(FVector(0.0f, 0.0f, 400.0f));
	// collision
	m_bossBasicCollision->SetCollisionProfileName("BlockAll");
	m_bossBasicCollision->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	// SET TO BLOCK EVERYTHING EXCEPT PROJECTILES
	
	m_bossBasicCollision->SetupAttachment(RootComponent);
#pragma endregion

#pragma region Activation Trigger Box
	// Activation trigger box
	m_ActivateTriggerSphere = CreateDefaultSubobject<USphereComponent>(TEXT("Activation Trigger"));
	// Size and Location
	m_ActivateTriggerSphere->SetRelativeLocation(FVector(0.0f, 0.0f, 270.0f));
	m_ActivateTriggerSphere->SetSphereRadius(1600.0f);	
	// collision
	m_ActivateTriggerSphere->SetCollisionProfileName("OverlapAll");
	m_ActivateTriggerSphere->SetCollisionEnabled(ECollisionEnabled::QueryOnly);

	m_ActivateTriggerSphere->SetupAttachment(RootComponent);
#pragma endregion

#pragma region Crystal Hitbox
	m_CrystalDamageSphere = CreateDefaultSubobject<USphereComponent>(TEXT("Crystal Hitbox"));
	// Size and location
	m_CrystalDamageSphere->SetRelativeLocation(FVector(0.0f, 20.0f, 800.0f));
	m_CrystalDamageSphere->SetSphereRadius(100.0f);
	// collision
	m_CrystalDamageSphere->SetCollisionProfileName("BlockAll");
	m_CrystalDamageSphere->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);

	m_CrystalDamageSphere->SetupAttachment(RootComponent);
#pragma endregion

#pragma region States
	m_BossStateSad = CreateDefaultSubobject<UBossStateSadComponent>(TEXT("Sad State Component"));
	m_BossStateSad->SetOwningBoss(this);

	m_BossStateBargain = CreateDefaultSubobject<UBossStateBargainComponent>(TEXT("Bargain State Component"));
	m_BossStateBargain->SetOwningBoss(this);

	m_BossStateAnger = CreateDefaultSubobject<UBossStateAngerComponent>(TEXT("Anger State Component"));
	m_BossStateAnger->SetOwningBoss(this);

#pragma endregion

	m_healthComponent = CreateDefaultSubobject<UObjectHealthComponent>(TEXT("Health Component"));

	m_IdleAnim = CreateDefaultSubobject<UAnimationAsset>(TEXT("Idle Anim"));
	m_VulnerableEnterAnim = CreateDefaultSubobject<UAnimationAsset>(TEXT("Vulnerable Enter Anim"));
	m_VulnerableIdleAnim = CreateDefaultSubobject<UAnimationAsset>(TEXT("Vulnerable Idle Anim"));
	m_VulnerableExitAnim = CreateDefaultSubobject<UAnimationAsset>(TEXT("Vulnerable Exit Anim"));

	m_WaterEnterAnim = CreateDefaultSubobject<UAnimationAsset>(TEXT("Water Enter Anim"));
	m_WaterIdleAnim = CreateDefaultSubobject<UAnimationAsset>(TEXT("Water Idle Anim"));
	m_WaterExitAnim = CreateDefaultSubobject<UAnimationAsset>(TEXT("Water Exit Anim"));

	m_DeathEnterAnim = CreateDefaultSubobject<UAnimationAsset>(TEXT("Death Enter Anim"));
	m_DeathIdleAnim = CreateDefaultSubobject<UAnimationAsset>(TEXT("Death Idle Anim"));
#pragma region Misc Variables
	// Vulnerability
	//ToggleVulnerableState(false);
	SetVulnerableState(false);
	SetVulnerabilityDuration(5.0f);
	// Health
	//SetHealth(100.0f);
	//m_BossTempMesh->SetAnimation();
	//m_healthComponent->SetHealth(0.0f);
	SetActive(false);
	m_health = 1;
#pragma endregion
}

// Called when the game starts or when spawned
void ABossPawn::BeginPlay()
{
	Super::BeginPlay();

	m_healthComponent->SetMaxHealth(1.0f);
	m_healthComponent->SetHealth(1.0f);
	FAttachmentTransformRules rules = FAttachmentTransformRules(EAttachmentRule::SnapToTarget, true);

	m_CrystalDamageSphere->AttachToComponent(m_BossTempMesh, rules, "Crystal_Socket");
	//m_CrystalDamageSphere->SetupAttachment(m_BossTempMesh->GetSocketByName("Crystal_Socket"));
	//m_CrystalDamageSphere->AttachTo(, m_BossTempMesh->GetSocketByName("Crystal_Socket"))
	SetActive(true);
	m_BossTempMesh->SetAnimation(m_IdleAnim);
	m_BossTempMesh->PlayAnimation(m_IdleAnim, true);
	// TODO REMOVE
	OnActivate();
}

void ABossPawn::OnActivate()
{
	SetBossState();
	//ToggleVulnerableState(false);
	SetVulnerableState(false);
	SetActive(true);
}

void ABossPawn::SetBossState()
{
	if (m_active)
	{	//if (m_health >= m_health * 0.75) // 75%+ health
	//{
	//	SetBossState(m_BossStateDenial);
	//}
	//else if (m_health >= m_health * 0.5) // 50%+ health
	//{
	//	SetBossState(m_BossStateAnger);
	//}
	//else if (m_health >= m_health * 0.25) // 25%+ health
	//{
	//	SetBossState(m_BossStateBargain);
	//}
		/*else*/ if (m_health > 0) // greater than 0 health
		{
			SetBossState(m_BossStateSad);
			if (GetBossState())
			{
				m_CurrentBossState->BeginStateLoop();
			}
		}
		else if (m_health <= 0) // no health
		{
			DieAnim();
		}

	}
}

//void ABossPawn::SetHealth(float a_health, int a_operation)
//{
//	//switch (a_operation)
//	//{
//	//case OP_ADD:
//	//	//m_healthComponent->AddHealth()
//	//	//m_health += a_health;
//	//	break;
//	//case OP_SUBTRACT:
//	//	m_health -= a_health;
//	//	break;
//	//default:
//	//	break;
//	//}
//}

void ABossPawn::BeginStateLoop()
{
	//SetBossState();
	if(m_health > 0)
		m_CurrentBossState->BeginStateLoop();
}

void ABossPawn::EndStateLoop()
{
	if (m_health > 0)
	{
		VulnerableBeginAnimTransition();
	}
	else
	{
		DieAnim();
	}
}

void ABossPawn::ToggleVulnerableState(bool a_vulnerable)
{
	//if (a_vulnerable)
	//{
	//	//m_CrystalDamageSphere->SetCollisionProfileName("BlockAll");
	//	//m_CrystalDamageSphere->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	//	//GetWorldTimerManager().SetTimer(m_vulnerabilityTimer, this, ABossPawn::VulnerableEnd, m_vulnerabilityDuration, false, 0.0f);
	//}
	//else if (!a_vulnerable)
	//{
	//	/*m_CrystalDamageSphere->SetCollisionProfileName("NoCollision");
	//	m_CrystalDamageSphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	//	BeginStateLoop();*/
	//}
}

void ABossPawn::VulnerableBeginAnimTransition()
{
	m_BossTempMesh->SetAnimation(m_VulnerableEnterAnim);
	m_BossTempMesh->PlayAnimation(m_VulnerableEnterAnim, true);
	SetVulnerableState(true);
	GetWorldTimerManager().SetTimer(m_vulnerabilityTimer, this, &ABossPawn::VulnerableBegin, 1.4, false);
}

void ABossPawn::VulnerableBegin()
{

	GetWorldTimerManager().ClearTimer(m_vulnerabilityTimer);

	m_BossTempMesh->SetAnimation(m_VulnerableIdleAnim);
	m_BossTempMesh->PlayAnimation(m_VulnerableIdleAnim, true);
	//toggle
	//timer 
	GetWorldTimerManager().SetTimer(m_vulnerabilityTimer, this, &ABossPawn::VulnerableEndAnimTransition, m_vulnerabilityDuration, false);

}

void ABossPawn::VulnerableEndAnimTransition()
{
	GetWorldTimerManager().ClearTimer(m_vulnerabilityTimer);

	m_BossTempMesh->SetAnimation(m_VulnerableExitAnim);
	m_BossTempMesh->PlayAnimation(m_VulnerableExitAnim, true);

	GetWorldTimerManager().SetTimer(m_vulnerabilityTimer, this, &ABossPawn::VulnerableEnd, 1.6, false);
}

void ABossPawn::VulnerableEnd()
{
	GetWorldTimerManager().ClearTimer(m_vulnerabilityTimer);

	m_BossTempMesh->SetAnimation(m_IdleAnim);
	m_BossTempMesh->PlayAnimation(m_IdleAnim, true);

	//ToggleVulnerableState(false);
	SetVulnerableState(false);
	BeginStateLoop();
}

// Called every frame
void ABossPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	//GEngine->AddOnScreenDebugMessage(-1, 0.5f, FColor::Yellow, "BOSS HEALTH" + FString::FromInt(m_health));

}

// Called to bind functionality to input
void ABossPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void ABossPawn::BeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor)
	{
		if(APlayerCharacter* player = Cast<APlayerCharacter>(OtherActor) )
		{
			if (!GetActive())
			{
				OnActivate();
			}
		}
	}
}

void ABossPawn::TakeDamage(float a_damage)
{
	if (m_vulnerable)
	{
		GetHealthComponent()->SubtractHealth(a_damage, false);
		m_health -= a_damage;
		if (m_health <= 0)
		{
			DieAnim();
		}
	}
}

void ABossPawn::DieAnim()
{	
	//play die anim
	m_BossTempMesh->SetAnimation(m_DeathEnterAnim);
	m_BossTempMesh->PlayAnimation(m_DeathEnterAnim, true);
	//timer
	GetWorldTimerManager().SetTimer(m_deathTimer, this, &ABossPawn::VulnerableEndAnimTransition, 9.4, false);
}

void ABossPawn::Die()
{
	GetWorldTimerManager().ClearTimer(m_deathTimer);
	//boss death idle
	m_BossTempMesh->SetAnimation(m_DeathIdleAnim);
	m_BossTempMesh->PlayAnimation(m_DeathIdleAnim, true);
}

void ABossPawn::WaterBeginAnimTransition()
{
	m_BossTempMesh->SetAnimation(m_WaterEnterAnim);
	m_BossTempMesh->PlayAnimation(m_WaterEnterAnim, true);
}
void ABossPawn::WaterIdle()
{
	m_BossTempMesh->SetAnimation(m_WaterIdleAnim);
	m_BossTempMesh->PlayAnimation(m_WaterIdleAnim, true);
}
void ABossPawn::WaterEndAnimTransition()
{
	m_BossTempMesh->SetAnimation(m_WaterExitAnim);
	m_BossTempMesh->PlayAnimation(m_WaterExitAnim, true);
}


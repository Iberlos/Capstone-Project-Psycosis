// Fill out your copyright notice in the Description page of Project Settings.

//Header include
#include "ProjectileActor.h"

//Custom Engine Includes
#include "Components/BoxComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "GameObjects/AI/AIBaseCharacter.h"
#include "Components/CapsuleComponent.h"

//Custom Project Includes
#include "GameObjects/Interactables/Pickups/Weapons/WeaponPickup.h"
#include "GameObjects/Players/PlayerCharacter.h"
#include "GameObjects/AI/Boss/BossPawn.h"
#include "GameMode/MainGameMode.h"

AProjectileActor::AProjectileActor()
{
	m_projectileComponent = CreateDefaultSubobject<UProjectileMovementComponent>("Projectile Movement");
	m_projectileComponent->ProjectileGravityScale = 0.0f;
	m_colliderComponent->SetCollisionProfileName("ProjectileCollision1");
	m_colliderComponent->OnComponentHit.AddDynamic(this, &AProjectileActor::OnHit);
	m_colliderComponent->SetSimulatePhysics(false);

	//Add Cues Related to the MyActor base class
	m_soundCues.Add("Hit");
	m_soundCues.Add("HitCharacter");

	//Add partciles Related to the MyActor base class
	m_particleFXs.Add("Hit");
	m_particleFXs.Add("HitCharacter");

	//Setup Replication
	bAlwaysRelevant = false;
	NetPriority = 2;
}

void AProjectileActor::BeginPlay()
{
	Super::BeginPlay();
}

void AProjectileActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AProjectileActor::Initialize(class AWeaponPickup* a_weapon, FVector a_forwardDirection, float a_damage, bool a_isFreindlyFire)
{
	//After spawning, set the forward diection, the speed to move and the damage to deal
    if (GetLocalRole() == ROLE_Authority)
    {
        m_weaponOfOrigin = a_weapon;
        m_damage = a_damage;
        //m_projectileComponent->SetVelocityInLocalSpace(m_speed * FVector::ForwardVector);
        m_projectileComponent->Activate();
		m_projectileComponent->Velocity = m_speed * a_forwardDirection;
		m_projectileComponent->SetUpdatedComponent(RootComponent);
        PlaySound("Lifetime", true); //probably looping so play in this objects particle system
        PlayParticle("Lifetime", true); //probably looping so play in this objects particle system

		if (a_isFreindlyFire)
		{
			APlayerCharacter* firingPlayer = a_weapon->GetPlayerCharacter();
			FString profileName = "ProjectileCollision";
			if (firingPlayer->GetPlayerIndex() != -1)
			{
				profileName.AppendInt(firingPlayer->GetPlayerIndex());
			}
			else
			{
				profileName.AppendInt(1);
			}
			m_colliderComponent->SetCollisionProfileName(*profileName);
		}
    }
}

void AProjectileActor::DealDamage(AMyCharacter* a_actor)
{
	//calls deal damage on the actor passing in m_damage
    if (Cast<AWeaponPickup>(this->GetOwner()))
    {
        AWeaponPickup* OwningWeapon = Cast<AWeaponPickup>(this->GetOwner());
        Cast<AAIBaseCharacter>(a_actor)->SetAttackingPlayer(OwningWeapon->GetFiringPlayerCharacter());
        a_actor->ApplyDamage(m_damage);
    }

	//Leter on there might be effects linked to the projectiles so this would also be done here.
	//a_actor->ApplyEffect(m_effect);
}

void AProjectileActor::ProjectileHit(AActor* a_actor)
{
	if (Cast<AMyCharacter>(a_actor))
	{
		PlaySound("HitCharacter", true, GetActorLocation());
		PlayParticle("HitCharacter", true, GetActorTransform());
		//Destroy();
	}
	else
	{
		PlaySound("Hit", true, GetActorLocation());
		PlayParticle("Hit", true, GetActorTransform());
		//Destroy();
	}
}

void AProjectileActor::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	//if colliding with enemies call the deal damage function.If colliding with anything else just destroy yourself.
	AAIBaseCharacter* enemy = Cast<AAIBaseCharacter>(OtherActor);
	if (enemy)
	{
		DealDamage(enemy);	
	}	
	ABossPawn* boss = Cast<ABossPawn>(OtherActor);
	if (boss)
	{
		AWeaponPickup* OwningWeapon = Cast<AWeaponPickup>(this->GetOwner());
		boss->TakeDamage(m_damage);
	}
	ProjectileHit(OtherActor);
}
// Fill out your copyright notice in the Description page of Project Settings.

//Header include
#include "WeaponPickup.h"

//Custom Engine Includes
#include "Engine.h"
#include "Engine/Public/TimerManager.h"
#include "Math/UnrealMathUtility.h"
#include "Net/UnrealNetwork.h"
#include "Kismet/GameplayStatics.h"
#include "Components/ArrowComponent.h"

//Custom Project Includes
#include "GameObjects/Players/PlayerCharacter.h"
#include "GameObjects/Weapons/WeaponPartActor.h"
#include "GameObjects/Projectiles/ProjectileActor.h"
#include "Gameobjects/Projectiles/PrototypeProjectileEffect/PrototypeProjectile.h"
#include "GameMode/MainGameMode.h"

void AWeaponPickup::SetProjectileParticlesAndSound(APrototypeProjectile* a_projectile) //THIS FUNSTION IS A FRIEND FUNCTION IT HAS BEEN EXCLUDED FROM THE NORMAL RULES OF POSITIONING TO BRING ATTENTION TO THAT FACT
{
	//This function is a friend of the prototype projectile class. Inside this function you will have private access to all its features. BE VERY CAREFULL USE IT ONLY FOR THE ORIGINAL PURPOSE.
	//SoundFX	
	if (m_soundCues.Contains("Hit"))
	{
		a_projectile->m_soundCues["Hit"].m_sound = m_soundCues["Hit"].m_sound;
	}
	if (m_soundCues.Contains("HitCharacter"))
	{
		a_projectile->m_soundCues["HitCharacter"].m_sound = m_soundCues["HitCharacter"].m_sound;
	}
	if (m_soundCues.Contains("Beam"))
	{
		a_projectile->m_soundCues["Beam"].m_sound = m_soundCues["Beam"].m_sound;

	}

	//ParticleFX
	if (m_particleFXs.Contains("Hit"))
	{
		a_projectile->m_particleFXs["Hit"].m_particle = m_particleFXs["Hit"].m_particle;
	}
	if (m_particleFXs.Contains("HitCharacter"))
	{
		a_projectile->m_particleFXs["HitCharacter"].m_particle = m_particleFXs["HitCharacter"].m_particle;
	}
	if (m_particleFXs.Contains("Beam"))
	{
		a_projectile->m_particleFXs["Beam"].m_particle = m_particleFXs["Beam"].m_particle;
	}
	if (m_particleFXs.Contains("Explosion"))
	{
		a_projectile->m_particleFXs["Explosion"].m_particle = m_particleFXs["Explosion"].m_particle;
	}
}

AWeaponPickup::AWeaponPickup()
{
	//Initialize components
	m_arrowComponent = CreateDefaultSubobject<UArrowComponent>("Muzzle Direction");
	m_audioComponent->SetupAttachment(m_arrowComponent);
	m_particleSystemComponent->SetupAttachment(m_arrowComponent);

	//Initialize Variables
	m_pickupType = InventoryTabEnum::ITE_Invalid; // If invalid it should be mopdified by the generator.
	m_weaponType_r = WeaponTypeEnum::WTE_Invalid; //If invalid it should be mopdified by the generator.
	m_weaponRarity_r = WeaponRarityEnum::WRE_Invalid; //If invalid it should be mopdified by the generator.
	m_fireMode_r = WeaponFireModeEnum::WFME_Invalid; //If invalid it should be mopdified by the generator.
	m_weaponProjectile = nullptr; //If nullptr it should be mopdified by the generator.
	m_ammoPool = 0; //If 0 it should be mopdified by the generator.

	for (int i = 0; i < (int)WeaponAttributeEnum::WAE_Invalid; i++)
	{
		m_totalAttributes.Add(0);//initializing all the base atributes to zero to guarantee they exist, will be overriden by the generator
	}

	//Add Cues Related to the MyActor base class
	m_soundCues.Add("Fire");
	m_soundCues.Add("StartFiringEmpty");
	m_soundCues.Add("Reload");
	m_soundCues.Add("Equip");
	m_soundCues.Add("Unequip");
    m_soundCues.Add("TrashWeapon");

	m_soundCues.Add("Hit"); //SOULD NOT EXIST BUT CANNOT BE REMOVED BECAUSE THE BLUEPRINTS HAVE BEEN GENERATED WITH THIS IN THE CONSTRUCTOR
	m_soundCues.Add("HitCharacter"); //SOULD NOT EXIST BUT CANNOT BE REMOVED BECAUSE THE BLUEPRINTS HAVE BEEN GENERATED WITH THIS IN THE CONSTRUCTOR

	//Add partciles Related to the MyActor base class
	m_particleFXs.Add("Fire");
	m_particleFXs.Add("StartFiringEmpty");
	m_particleFXs.Add("Reload");
	m_particleFXs.Add("Equip");
	m_particleFXs.Add("Unequip");
    m_particleFXs.Add("TrashWeapon");

	m_particleFXs.Add("Hit"); //SOULD NOT EXIST BUT CANNOT BE REMOVED BECAUSE THE BLUEPRINTS HAVE BEEN GENERATED WITH THIS IN THE CONSTRUCTOR
	m_particleFXs.Add("HitCharacter"); //SOULD NOT EXIST BUT CANNOT BE REMOVED BECAUSE THE BLUEPRINTS HAVE BEEN GENERATED WITH THIS IN THE CONSTRUCTOR


	//Setup Replication
	bAlwaysRelevant = false;
	NetPriority = 2;
    SetReplicates(true);
}

void AWeaponPickup::SERVER_EnableWeapon_Implementation() //FUNCTION ADDED BY ANOTHER TEAM MEMBER TO ENABLE POOLING OF THE WEAPONS (IT IS NOT POSITIONED CORRECTLY ACCORDING TO THE CODING STANDARDS WE HAD IN PLACE)
{
	SetReplicates(true);
	SetReplicateMovement(true);
	SetActorTickEnabled(true);
	for (auto part : m_weaponParts_r)
	{
		part->SetReplicates(true);
		part->SetActorTickEnabled(true);
	};
	NET_SetRegisteredMeshVisibilities(true);
	m_ammoPool = m_totalAmmo;
}

void AWeaponPickup::SERVER_DisableWeapon_Implementation() //FUNCTION ADDED BY ANOTHER TEAM MEMBER TO ENABLE POOLING OF THE WEAPONS (IT IS NOT POSITIONED CORRECTLY ACCORDING TO THE CODING STANDARDS WE HAD IN PLACE)
{
	SetReplicates(false);
	SetReplicateMovement(false);
	SetActorTickEnabled(false);
	for (auto part : m_weaponParts_r)
	{
		part->SetReplicates(false);
		part->SetActorTickEnabled(false);
	};
	NET_SetRegisteredMeshVisibilities(false);
}

void AWeaponPickup::BeginPlay()
{
    Super::BeginPlay();
}

void AWeaponPickup::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

bool AWeaponPickup::Interact(AActor* a_actor)
{
	if (Super::Interact(a_actor)) //The base class function call has the logic to define if the opbejct can be interacted with and play any necessary particles, sounds and animation montages
	{
		APlayerCharacter* character = Cast<APlayerCharacter>(a_actor);
		if (character)
		{
			USkeletalMeshComponent* FPSMesh = character->GetFPSMesh();
			if (FPSMesh->DoesSocketExist("Weapon-Socket"))//Attach yourself to the weapon socket
			{
				AttachToComponent(FPSMesh, FAttachmentTransformRules::SnapToTargetIncludingScale, "Weapon-Socket");
			}
			SetActorRelativeRotation(FQuat::Identity);
			if (!character->GetEquippedWeapon() || character->GetEquippedWeapon() == character->GetDefaultWeapon()) //IF THE PLAYER HAS NO WEAPON OR IF THE DEFAULT WEAPON IS EQQUIPED EQUIP THIS WEAPON INSTEAD
			{
				Equip(a_actor);
			}
			return true;
		}
	}
	return false;
}

bool AWeaponPickup::Use(AActor* a_actor)
{
	Equip(a_actor);

	return true;
}

void AWeaponPickup::Drop(AActor* a_actor)
{
	Unequip(a_actor, true); //Unequip (the unequip function already checks to see if this is the equipped weapon, if its not it just returns)
	DetachFromActor(FDetachmentTransformRules::KeepRelativeTransform);
	Super::Drop(a_actor);
}

void AWeaponPickup::StartFiring(AActor* a_actor, bool a_isFrendlyFire)
{
	m_isFriendlyFire = a_isFrendlyFire;
	m_firingPlayerCharacter = Cast<APlayerCharacter>(a_actor);
	m_fireCount = 0;
	if (m_ammoInMagazine > 0 && !m_fireTimerHandle.IsValid())
	{
		switch (m_fireMode_r) //Modify the parameters slightly to get different behaviour from the same weapon depending on the configuration that the generator gave it
		{
		case WeaponFireModeEnum::WFME_Single:
		{
			m_burstCount = 1;
			break;
		}
		case WeaponFireModeEnum::WFME_Burst:
		{
			m_burstCount = 3;
			break;
		}
		case WeaponFireModeEnum::WFME_Auto:
		{
			m_burstCount = -1;
			break;
		}
		}
		float fireDelay = 1.0f / m_totalAttributes[(int)WeaponAttributeEnum::WAE_FireRate];
		GetWorldTimerManager().SetTimer(m_fireTimerHandle, this, &AWeaponPickup::Fire, fireDelay, true, fireDelay);
        Fire();
	}
	else
	{
		PlaySound("StartFiringEmpty", true); //could be played in the object or on the world, makes no difference
		PlayParticle("StartFiringEmpty", true); //could be played in the object or on the world, makes no difference
	}
	m_isFiring = true;
}

void AWeaponPickup::StopFiring()
{
	//stop the m_fireTimerHandle timer
	if(m_fireMode_r == WeaponFireModeEnum::WFME_Auto)
		GetWorldTimerManager().ClearTimer(m_fireTimerHandle);

	m_isFiring = false;

	//May Need to move
	m_accuracyConeAngle = 0;

    //Release 2
	ed_TriggerRelease.Broadcast(this);
}

void AWeaponPickup::Reload()
{
	int ammoToReload = FMath::Clamp(((int)m_totalAttributes[(int)WeaponAttributeEnum::WAE_MagazineSize] - m_ammoInMagazine), 0, m_ammoPool); //calculate how much ammo is needed to fill the magazine but never more than what is left in the ammo pool
	if (ammoToReload != 0)
	{
		//THE GAME ENDED UP NOT HAVING RELOAD OPTIONS. INSTEAD ONLY THE SECOND PART OF THIS IFSTATEMENT TRIGGERS EVERYTIME TRIGGERING THE TrasWeapon() FUNCTION. THIS WAS DONE TO MAKE THE PLAYER TRY MANY DIFFERENT TYPES OF GUNS IN THE SHORT TIME WE HAD FOR DEMONSTRATIONS
		PlaySound("Reload", true); //could be played in the object or on the world, makes no difference
		PlayParticle("Reload"); //could be played in the object or on the world, makes no difference
		m_ammoPool -= ammoToReload; //remove the ammo from the pool
		m_ammoInMagazine += ammoToReload; //add the ammo to the magazine
	}
	else
	{
        if (m_firingPlayerCharacter && m_ammoInMagazine <= 0)
        {
            PlaySound("TrashWeapon", true, GetActorLocation()); //could be played in the object or on the world, makes no difference
            PlayParticle("TrashWeapon"); //could be played in the object or on the world, makes no difference
            m_firingPlayerCharacter->TrashWeapon(); //I am hoping this is still the player that just fired it. It should be.
        }
	}
}

void AWeaponPickup::AddNewWeaponPartToWeaponPartArray(AWeaponPartActor* WeaponPartToAdd)
{
	//FUNCTION USED BY THE GENERATOR TO REGISTER NEW PARTS IN THE PROCESS OF CONFIGURING THE WEAPON
    m_weaponParts_r.Add(WeaponPartToAdd);
    WeaponPartToAdd->SetOwner(this);
}

void AWeaponPickup::DestroySelf()
{
	Cast<AMainGameMode>(GetWorld()->GetAuthGameMode())->AddNewWeaponToPool(this);// CODE ADDED BY ANOTHER TEAM MEMBER TO ENABLE POOLING OF THE WEAPONS (I WOULD HAVE MADE A SEPARATE FUNCTION TO DEAL WITH THIS.)

	//ORIGINAL CODE IN THIS FUNCTION
	//Destroy();	
	//for (AWeaponPartActor* part : m_weaponParts_r)	
	//{	
	//	part->Destroy();	
	//}
	//ORIGINAL CODE END

    //Release 2
    ed_TriggerRelease.Broadcast(this); //CODE ADDED BY ANOTHER TEAM MEMBER TO ENABLE REFERENCED PROJECTILES TO BE NOTIFIED WHEN THE WEAPO IS DESTROYED OR RETURNED TO THE POOL.
}

void AWeaponPickup::CompleteWeaponAssembly()
{
	//THIS FUNCTION EXTRACTS THE ATTRIBUTES FROM THE WEAPON PARTS AND STORES THE TOTALS IN ITSELF FOR EASY ACCESS.
    if (m_weaponParts_r.Num() > 0) 
    {
        for (auto part : m_weaponParts_r)
        {
            part->SetupWeaponAttributeArray();
            m_totalAttributes[(int)WeaponAttributeEnum::WAE_Damage] += (part->GetWeaponAttributeArray()[(int)WeaponAttributeEnum::WAE_Damage] * ((int)m_weaponRarity_r * 0.2 + 1));
			//You shouldn't be very acurate if you are a shotgun.
			if(m_weaponType_r == WeaponTypeEnum::WTE_Shotgun) m_totalAttributes[(int)WeaponAttributeEnum::WAE_Accuracy] = 1;
            else m_totalAttributes[(int)WeaponAttributeEnum::WAE_Accuracy] += (part->GetWeaponAttributeArray()[(int)WeaponAttributeEnum::WAE_Accuracy] * ((int)m_weaponRarity_r + 1));
			//
            m_totalAttributes[(int)WeaponAttributeEnum::WAE_FireRate] += part->GetWeaponAttributeArray()[(int)WeaponAttributeEnum::WAE_FireRate];
            m_totalAttributes[(int)WeaponAttributeEnum::WAE_Recoil] += (part->GetWeaponAttributeArray()[(int)WeaponAttributeEnum::WAE_Recoil] / ((int)m_weaponRarity_r + 1));
            m_totalAttributes[(int)WeaponAttributeEnum::WAE_MagazineSize] += (part->GetWeaponAttributeArray()[(int)WeaponAttributeEnum::WAE_MagazineSize]);
            m_totalAttributes[(int)WeaponAttributeEnum::WAE_Weight] += part->GetWeaponAttributeArray()[(int)WeaponAttributeEnum::WAE_Weight];
        }

		m_totalAttributes[(int)WeaponAttributeEnum::WAE_Recoil] = FMath::Clamp(m_totalAttributes[(int)WeaponAttributeEnum::WAE_Recoil], 1.0f, 13.0f);
    }
	FTimerHandle t;
	GetWorldTimerManager().SetTimer(t, this, &AWeaponPickup::NET_CompleteWeaponAssembly, 2.0f);
    m_ammoPool = m_totalAttributes[(int)WeaponAttributeEnum::WAE_MagazineSize];
	m_totalAmmo = m_ammoPool;

	m_accuracyConeAngle = 0;
}

void AWeaponPickup::NET_CompleteWeaponAssembly_Implementation()
{
	bool socketFound = false;
	for (USkeletalMeshComponent* mesh : m_registeredMeshes_r)
	{
		if (mesh == nullptr)
		{
			FTimerHandle t;
			GetWorldTimerManager().SetTimer(t, this, &AWeaponPickup::NET_CompleteWeaponAssembly, 2.0f);
			return;
		}
		if (!socketFound)
		{
			socketFound = mesh->DoesSocketExist("Projectile_Socket");
			if (socketFound)//Attach yourself to the weapon socket
			{
				m_arrowComponent->AttachToComponent(mesh, FAttachmentTransformRules::SnapToTargetIncludingScale, "Projectile_Socket");
			}
		}
		mesh->SetCustomDepthStencilValue((int)m_weaponRarity_r + 1);
	}
	if (!socketFound) GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Red, TEXT("NO SOCKET FOUND FOR PROJECTILES!"));
}

//Release 2 //COMMENT ADDED BY ANOTHER TEAMMATE (I WOULD NOT MARK THE PASSAGE OF TIME IN THE CODE, SEEMS COUTER PRODUCTIVE)

float AWeaponPickup::GetAttributeTotal(WeaponAttributeEnum a_key)
{
    return m_totalAttributes[(int)a_key];
}

void AWeaponPickup::Fire()
{
	if (m_burstCount == 0)//if there are no more bullets in this burst stop firing, full auto bullet count is -1 thus it will fire untill it is out of bullets
	{
		GetWorldTimerManager().ClearTimer(m_fireTimerHandle);
		return;
	}

	PlaySound("Fire", true);
	PlayParticle("Fire", true);
	m_firingPlayerCharacter->PlayAnimationMontage("Shoot", (int)GetWeaponType()); //Notify the player that it should react to this bullet being shot

	//Aiming modifiers
	float accAimMod = 0;
	float recoilAimMod = 0;

	//Actually spawn the projectiles or take them from the pool
	int projectiles = m_weaponType_r == WeaponTypeEnum::WTE_Shotgun ? 5 : 1;//if you are a shotgun spawn 5 projectiles instead of one
	if (wepAmmoType == (int)AmmoTypes::AT_ShockWave) //ADDED BY ANOTHER TEAMMATE TO ENABLE THE SOUND GUN VARIETY (I WOULD HAVE REFACTORED THE SYSTEM TO ALLOW THE WEAPON TO DEFINE THE NUMBER OF PROJECTILES BASED ON THE PARTS IT HAS, BUT THIS WORKS TOO.)
	{
		projectiles = 1;
	}
	for (int i = 0; i < projectiles; i++)
	{
		//Aim modifier calc (Having this in here causes shotguns to be less affected by aiming since one shot is more than 5 the recoil won't be affected by aiming)
		m_fireCount++;
		if (m_isAiming && m_fireCount < 3) //If aiming and fire count is below 5 set the modifiers based on fire count
		{
			accAimMod = m_totalAttributes[(int)WeaponAttributeEnum::WAE_Accuracy] * 0.5f / m_fireCount; //first shot has 1.5 times the accuracy, second has 1.25 times, and tird has 1.155 times
			//m_accuracyAimMod = accAimMod;
			recoilAimMod = -m_totalAttributes[(int)WeaponAttributeEnum::WAE_Recoil] / (m_fireCount + 1); //first shot has half recoil, second has 2/3 tird has 3/4
		}
		//fire
		FActorSpawnParameters projectileSpawnParams;
        projectileSpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
		float halfAngle = PI * (5.0f / (m_totalAttributes[(int)WeaponAttributeEnum::WAE_Accuracy] + accAimMod)) / 180.0f;
		m_accuracyConeAngle = FMath::RadiansToDegrees(halfAngle);//ADDED BY ANOTHER TEAMMEMBER TO ALLOW FOR AN AIMING CROSS

		FVector direction = m_arrowComponent->GetForwardVector();
		direction = FMath::VRandCone(direction, halfAngle);

		//ORIGINAL CODE BEFORE PROJECTILE POOLING	
		//AProjectileActor* projectile = GetWorld()->SpawnActor<AProjectileActor>(m_weaponProjectile, m_arrowComponent->GetComponentLocation(), direction.Rotation(), pickupSpawnParams);
		//ORIGINAL CODE END

		//ADDED BY ANOTHER TEAMMEMBER TO ALLO FOR PROJECTILE POOLING
		AProjectileActor* projectile = Cast<AMainGameMode>(GetWorld()->GetAuthGameMode())->GetBulletFromPool();
		if (projectile)
		{
			projectile->TeleportTo(m_arrowComponent->GetComponentLocation(), direction.Rotation());	
		}
		else 
		{
			projectile = GetWorld()->SpawnActor<AProjectileActor>(m_weaponProjectile, m_arrowComponent->GetComponentLocation(), direction.Rotation(), projectileSpawnParams);
		}
		//ADDITION END

		
		if (projectile != nullptr)
		{
			//ADDITION FROM ANOTHERTEAMMEMBER TO ALLOW FOR HIS PROJECTILE SYSTEM TO WORK
			//Setup the activation for effects(start, in flight and on hit)
			if (m_projectileOnStartEffect != CustomStartEffects::CSE_NumberOfEffects)
			{
				Cast<APrototypeProjectile>(projectile)->SetEnableSpawnEffect(true);
				Cast<APrototypeProjectile>(projectile)->SetSpawnEffect(m_projectileOnStartEffect);
			}
			if (m_projectileInFlightEffect != CustomInFlightEffects::CIFE_NumberOfEffects)
			{
				Cast<APrototypeProjectile>(projectile)->SetEnableFlightEffect(true);
				Cast<APrototypeProjectile>(projectile)->SetInFlightEffect(m_projectileInFlightEffect);
			}
			if (m_projectileOnHitEffect != CustomOnHitEffects::COHE_NumberOfEffects)
			{
				Cast<APrototypeProjectile>(projectile)->SetEnableHitEffect(true);
				Cast<APrototypeProjectile>(projectile)->SetHitEffect(m_projectileOnHitEffect);
			}


			Cast<APrototypeProjectile>(projectile)->SetProjectileAmmoType(wepAmmoType);
			Cast<APrototypeProjectile>(projectile)->SetOwner(this);
			//ADDITION END
			SetProjectileParticlesAndSound(Cast<APrototypeProjectile>(projectile)); //FRIEND FUCNTION CALL TO ALLOW SETUP OF THE PROJECTILES OF DIFFERENT TYPES
            Cast<APrototypeProjectile>(projectile)->Initialize(this, direction, m_totalAttributes[(int)WeaponAttributeEnum::WAE_Damage], m_isFriendlyFire);
		}

	}
	//Notify the player that recoil should be applied for this shot
	m_firingPlayerCharacter->ApplyRecoil(m_totalAttributes[(int)WeaponAttributeEnum::WAE_Recoil] + recoilAimMod);
	//decrease the burst count, it allows you to know if you should shoot more or not.
	m_burstCount--;
	//decrese the ammount of ammmo in your magazine
	m_ammoInMagazine--;

	if (m_ammoInMagazine <= 0)//If there are no more bullets stop firing and call reload
	{
		GetWorldTimerManager().ClearTimer(m_fireTimerHandle);
		Reload();
		return;
	}
}

void AWeaponPickup::Equip(AActor* a_Actor)
{
	APlayerCharacter* character = Cast<APlayerCharacter>(a_Actor);
	if (character)
	{
		if (character->GetEquippedWeapon() != this )
		{
			//Unequip the equpped weapon
			if (character->GetEquippedWeapon())
			{
				character->GetEquippedWeapon()->Unequip(a_Actor);
				character->m_tempReferenceToWeaponBeingEquipped = this;
				m_tempReferenceToCharacterBeingEquipped = character;
			}
			else
			{
				//activate the meshes
				NET_SetRegisteredMeshVisibilities(true);
				//register this as the equipped weapon
				character->SetEquippedWeapon(this);
				PlaySound("Equip", true); //could be played in the object or on the world, makes no difference
				PlayParticle("Equip"); //could be played in the object or on the world, makes no difference
				character->PlayAnimationMontage("Draw", (int)GetWeaponType());
				//When equipped override the default network settings
				for (AWeaponPartActor* part : m_weaponParts_r)
				{
					part->bAlwaysRelevant = true;
					part->NetPriority = 3;
				}
				bAlwaysRelevant = true;
				NetPriority = 3;
			}

		}
	}
	Reload();
}

void AWeaponPickup::FinalizeEquip()
{
	//THIS FUNCTION WAS A LATER ADDITION TO THE CODE. IT IS TRIGGERED BY AN ANIMATION NOTIFY IN THE REALTED PLAYER ANIMATION. THIS ALLOWS THE ANIMATION TO BE SYNCHED WITH THE SWITCHING OF THE WEAPONS
	if (GetLocalRole() == ROLE_Authority)
	{
		if (m_tempReferenceToCharacterBeingEquipped)
		{
			//activate the meshes
			NET_SetRegisteredMeshVisibilities(true);
			//register this as the equipped weapon
			m_tempReferenceToCharacterBeingEquipped->SetEquippedWeapon(this);
			PlaySound("Equip", true); //could be played in the object or on the world, makes no difference
			PlayParticle("Equip"); //could be played in the object or on the world, makes no difference
			m_tempReferenceToCharacterBeingEquipped->PlayAnimationMontage("Draw", (int)GetWeaponType());
			//When equipped override the default network settings
			for (AWeaponPartActor* part : m_weaponParts_r)
			{
				part->bAlwaysRelevant = true;
				part->NetPriority = 3;
			}
			bAlwaysRelevant = true;
			NetPriority = 3;

			m_tempReferenceToCharacterBeingEquipped->m_tempReferenceToWeaponBeingEquipped = nullptr;
			m_tempReferenceToCharacterBeingEquipped = nullptr;
		}
	}
}

void AWeaponPickup::Unequip(AActor* a_Actor, bool a_discardTrigered)
{
	APlayerCharacter* character = Cast<APlayerCharacter>(a_Actor);
	if (character)
	{
		if (character->GetEquippedWeapon() == this)
		{
			character->PlayAnimationMontage("Hoster", (int)GetWeaponType());
			if (!a_discardTrigered)
			{
				m_tempReferenceToCharacterBeingEquipped = character;
			}
			else
			{
				character->SetEquippedWeapon(nullptr);
				//activate the meshes
				NET_SetRegisteredMeshVisibilities(false);
				PlaySound("Unequip", true); //could be played in the object or on the world, makes no difference
				PlayParticle("Unequip"); //could be played in the object or on the world, makes no difference
				//When notequipped use the default network settings
				for (AWeaponPartActor* part : m_weaponParts_r)
				{
					part->bAlwaysRelevant = false;
					part->NetPriority = 2;
				}
				bAlwaysRelevant = false;
				NetPriority = 2;
			}
		}
	}

}

void AWeaponPickup::FinalizeUnequip()
{
	//THIS FUNCTION WAS A LATER ADDITION TO THE CODE. IT IS TRIGGERED BY AN ANIMATION NOTIFY IN THE REALTED PLAYER ANIMATION. THIS ALLOWS THE ANIMATION TO BE SYNCHED WITH THE SWITCHING OF THE WEAPONS
	if (GetLocalRole() == ROLE_Authority)
	{
		if (m_tempReferenceToCharacterBeingEquipped)
		{
			m_tempReferenceToCharacterBeingEquipped->SetEquippedWeapon(nullptr);
			//activate the meshes
			NET_SetRegisteredMeshVisibilities(false);
			PlaySound("Unequip", true); //could be played in the object or on the world, makes no difference
			PlayParticle("Unequip"); //could be played in the object or on the world, makes no difference
			//When notequipped use the default network settings
			for (AWeaponPartActor* part : m_weaponParts_r)
			{
				part->bAlwaysRelevant = false;
				part->NetPriority = 2;
			}
			bAlwaysRelevant = false;
			NetPriority = 2;

			m_tempReferenceToCharacterBeingEquipped = nullptr;
		}
	}
}

void AWeaponPickup::NET_SetRegisteredMeshVisibilities_Implementation(bool a_state)
{
	//UNFORTUNATLY MESH VISIBILITY STATUS IS NOT REPLICATED BY DEFAULT IN UNREAL
	//activate the meshes
	for (USkeletalMeshComponent* mesh : m_registeredMeshes_r)
	{
		if (mesh)
			mesh->SetVisibility(a_state);
	}
}

void AWeaponPickup::GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AWeaponPickup, m_weaponType_r)
	DOREPLIFETIME(AWeaponPickup, m_weaponRarity_r)
	DOREPLIFETIME(AWeaponPickup, m_fireMode_r)
	DOREPLIFETIME(AWeaponPickup, m_ammoPool)
	DOREPLIFETIME(AWeaponPickup, m_ammoInMagazine)
	DOREPLIFETIME(AWeaponPickup, m_totalAttributes)
	DOREPLIFETIME(AWeaponPickup, m_weaponParts_r)

    //Release 2 //COMMENT ADDED BY ANOTHER TEAMMATE (I WOULD NOT MARK THE PASSAGE OF TIME IN THE CODE, SEEMS COUTER PRODUCTIVE)
    DOREPLIFETIME(AWeaponPickup, m_projectileOnStartEffect)
    DOREPLIFETIME(AWeaponPickup, m_projectileInFlightEffect)
    DOREPLIFETIME(AWeaponPickup, m_projectileOnHitEffect)
	DOREPLIFETIME(AWeaponPickup, m_totalAmmo)
}
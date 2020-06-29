// Fill out your copyright notice in the Description page of Project Settings.

//Header include
#include "PlayerCharacter.h"

//Custom Engine Includes
#include "Kismet/GameplayStatics.h"
#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Camera/CameraComponent.h"
#include "Net/UnrealNetwork.h"
#include "Engine.h"
#include "Engine/World.h"
#include "GameFramework/GameStateBase.h"

//Custom Project Includes
#include "GameObjects/Interactables/Pickups/Weapons/WeaponPickup.h"
#include "GameObjects/Interactables/InteractableActor.h"
#include "Custom_Components/InventoryComponent.h"
#include "Custom_Components/ObjectHealthComponent.h"
#include "GameInstance/CapstoneGameInstance.h"


APlayerCharacter::APlayerCharacter()
{
	GetCapsuleComponent()->SetCollisionProfileName("PlayerCollision1");

	//bind collision
	GetCapsuleComponent()->OnComponentHit.AddDynamic(this, &APlayerCharacter::OnHit);

	// Create a CameraComponent
	m_FPSCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("FPS Camera"));
	m_FPSCameraComponent->SetupAttachment(RootComponent);
	m_FPSCameraComponent->bUsePawnControlRotation = true;

	// Create a mesh component that will be used when being viewed from a '1st person' view (when controlling this pawn)
	m_FPSSkeletalMeshComponent = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("FP SMesh"));
	m_FPSSkeletalMeshComponent->SetOnlyOwnerSee(true);
	m_FPSSkeletalMeshComponent->SetupAttachment(m_FPSCameraComponent);
	m_FPSSkeletalMeshComponent->bCastDynamicShadow = false;
	m_FPSSkeletalMeshComponent->CastShadow = false;

	// Modify the main mesh so the Owner can't see it
	GetMesh()->SetOnlyOwnerSee(false);
	GetMesh()->SetOwnerNoSee(true);

	//Add sound and particles related to the player character
	m_soundCues.Add("Jump");
	m_particleFXs.Add("Jump");


	//Add animation montages related to Player Character
	m_animationMontages_FPM.Add("Discard");
	m_animationMontages_FPM.Add("Draw");
	m_animationMontages_FPM.Add("Hoster");
	m_animationMontages_FPM.Add("Interact");
	m_animationMontages_FPM.Add("Shoot");
	TArray<FName> keys;
	m_animationMontages_FPM.GetKeys(keys);
	for (FName key : keys)
	{
		for (int i = 0; i < (int)WeaponTypeEnum::WTE_Invalid; i++)
		{
			m_animationMontages_FPM[key].m_montages.Add(nullptr);
		}
	}

	m_animationMontages_TPM.Add("Discard");
	m_animationMontages_TPM.Add("Draw");
	m_animationMontages_TPM.Add("Hoster");
	m_animationMontages_TPM.Add("Interact");
	m_animationMontages_TPM.Add("Shoot");
	m_animationMontages_TPM.GetKeys(keys);
	for (FName key : keys)
	{
		for (int i = 0; i < (int)WeaponTypeEnum::WTE_Invalid; i++)
		{
			m_animationMontages_TPM[key].m_montages.Add(nullptr);
		}
	}

    //Add lerped values related to the Player Character class
    m_lerpedValues.Add("FOV");
    m_lerpedValues.Add("Recoil");

	//Setup Replication
	bAlwaysRelevant = true;
	NetPriority = 5;
	m_playerIndex_r = -1;
    m_FOVLerpDurantion = 0.5f;
}

void APlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	// Bind Movement Inputs 
	PlayerInputComponent->BindAxis("MoveForward", this, &APlayerCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &APlayerCharacter::MoveSideways);
	// Mouse Camera Rotation
	PlayerInputComponent->BindAxis("Turn", this, &APlayerCharacter::MouseInputHorizontal);
	PlayerInputComponent->BindAxis("LookUp", this, &APlayerCharacter::MouseInputVertical);
	// Bind jumping input
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &APlayerCharacter::PlayerJump);

	//Inventory and pickup related bindings
	PlayerInputComponent->BindAction("Interact", IE_Pressed, this, &APlayerCharacter::InteractWithFocused);
	PlayerInputComponent->BindAction("ShowInventory", IE_Pressed, this, &APlayerCharacter::ShowInventory);
	PlayerInputComponent->BindAction("ShowInventory", IE_Released, this, &APlayerCharacter::HideInventory);
	PlayerInputComponent->BindAction("UsePickup", IE_Pressed, this, &APlayerCharacter::UseSelected);
	PlayerInputComponent->BindAction("DropPickup", IE_Pressed, this, &APlayerCharacter::DropSelected);

	//Weapon related binds (Use the UsePickup bind to equip a new weapon)
	PlayerInputComponent->BindAction("Fire1", IE_Pressed, this, &APlayerCharacter::StartFiring);
	PlayerInputComponent->BindAction("Fire1", IE_Released, this, &APlayerCharacter::StopFiring);
	PlayerInputComponent->BindAction("Fire2", IE_Pressed, this, &APlayerCharacter::StartAiming);
	PlayerInputComponent->BindAction("Fire2", IE_Released, this, &APlayerCharacter::StopAiming);
	PlayerInputComponent->BindAction("Reload", IE_Released, this, &APlayerCharacter::Reload);
}

void APlayerCharacter::BeginPlay()
{
	Super::BeginPlay();
	SetInputEnabled(false);

	if (GetLocalRole() ==ROLE_Authority)
	{
		//setup frindly fire mechanic.
		UCapstoneGameInstance* gameInstance = Cast<UCapstoneGameInstance>(GetGameInstance());
		m_isFriendlyFire = gameInstance->GetIsFriendlyFire();
		if (m_isFriendlyFire)
		{
			if (m_playerIndex_r == -1)
			{
				TArray<APlayerState*> players = GetWorld()->GetAuthGameMode()->GameState->PlayerArray;
				m_playerIndex_r = players.Num();
				//doing this here is pretty bad, but it works...
				FString profileName = "PlayerCollision";
				profileName.AppendInt(m_playerIndex_r);
				GetCapsuleComponent()->SetCollisionProfileName(*profileName);
				//end
			}
			FTimerHandle t;
			GetWorldTimerManager().SetTimer(t, this, &APlayerCharacter::CLIENT_SetCollisionForFriendlyFire, 3.0f, false);
		}

		//setup fall back to world timer
		FTimerHandle t;
		GetWorldTimerManager().SetTimer(t, this, &APlayerCharacter::CheckForGround, 1.0f, true);

		//setup default weapon
		if (m_defaultWeaponTemplate)
		{
			AWeaponPickup* defaultWeapon = GetWorld()->SpawnActor<AWeaponPickup>(m_defaultWeaponTemplate);
			m_inventoryComponent->SetDefaultWeapon(defaultWeapon);
			defaultWeapon->CompleteWeaponAssembly();
			defaultWeapon->Interact(this);
		}

		//setup enable on start timer
		FTimerHandle t2;
		GetWorldTimerManager().SetTimer(t2, this, &APlayerCharacter::EnableOnLevelStart, 3.0f, false);
	}
	//save the initial FOV to allow aiming
    m_lerpedValues["FOV"].Init(m_FPSCameraComponent->FieldOfView, m_FPSCameraComponent->FieldOfView - 30, 0.05f, 0, 0/*, &(m_FPSCameraComponent->FieldOfView)*/);

	if (GetLocalRole() == ROLE_AutonomousProxy || GetLocalRole() == ROLE_Authority)
	{
		//setup raycast to focused
		FTimerHandle t;
		GetWorldTimerManager().SetTimer(t, this, &APlayerCharacter::RayCastToFocused, 0.1f, true);
	}
}

void APlayerCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (GetLocalRole() == ROLE_AutonomousProxy || GetLocalRole() == ROLE_Authority)
	{
		//Interaction setUp
		//RayCastToFocused();

		//Debug messages for equipped weapon
		//if (GetLocalRole() == ROLE_Authority)
		//{
		//	if (m_equippedWeapon_r)
		//	{
		//		GEngine->AddOnScreenDebugMessage(2, 1.f, FColor::Red, FString::Printf(TEXT("Ammo in magazine: %f"), m_equippedWeapon_r->GetAmmoInMagazine()));
		//		GEngine->AddOnScreenDebugMessage(3, 1.f, FColor::Red, FString::Printf(TEXT("\nWEAPON STATS:\nRarity:%d\nDamage:%f\nAcc:%f\nFireRate:%f\nRecoil:%f\nMagSize:%f\nWeight:%f"), (int)m_equippedWeapon_r->GetWeaponRarity(), m_equippedWeapon_r->GetAttributeTotal(WeaponAttributeEnum::WAE_Damage), m_equippedWeapon_r->GetAttributeTotal(WeaponAttributeEnum::WAE_Accuracy), m_equippedWeapon_r->GetAttributeTotal(WeaponAttributeEnum::WAE_FireRate), m_equippedWeapon_r->GetAttributeTotal(WeaponAttributeEnum::WAE_Recoil), m_equippedWeapon_r->GetAttributeTotal(WeaponAttributeEnum::WAE_MagazineSize), m_equippedWeapon_r->GetAttributeTotal(WeaponAttributeEnum::WAE_Weight)));
		//	}
		//}

        m_FPSCameraComponent->SetFieldOfView(m_lerpedValues["FOV"].Get()); //This might change, ideally the lerped values would update whatever function is linked to them
        LookVertical(-m_lerpedValues["Recoil"].Get()); //This might change, ideally the lerped values would update whatever function is linked to them
        LookHorizontal(m_lerpedValues["Recoil"].Get() * 0.1f); //This might change, ideally the lerped values would update whatever function is linked to them
	}
}

float APlayerCharacter::PlayAnimationMontage(FName a_key, int a_index)
{
	if (m_animationMontages_FPM.Contains(a_key) && m_animationMontages_FPM[a_key].m_montages.IsValidIndex(a_index) && m_animationMontages_FPM[a_key].m_montages[a_index] != nullptr)
	{
		SERVER_PlayAnimationMontage(a_key, a_index);
		return m_animationMontages_FPM[a_key].m_montages[a_index]->GetPlayLength();
	}
	return 0.0f;
}

void APlayerCharacter::NET_PlayAnimationMontage(FName a_key, int a_index)
{
	if (m_animationMontages_TPM.Contains(a_key) && m_animationMontages_TPM[a_key].m_montages.IsValidIndex(a_index) && m_animationMontages_TPM[a_key].m_montages[a_index] != nullptr)
	{
		UAnimInstance* animInstance = GetMesh()->GetAnimInstance();
		if (animInstance)
		{
			animInstance->Montage_Play(m_animationMontages_TPM[a_key].m_montages[a_index], 1.0f);
		}
	}
	if (m_animationMontages_FPM.Contains(a_key) && m_animationMontages_FPM[a_key].m_montages.IsValidIndex(a_index) && m_animationMontages_FPM[a_key].m_montages[a_index] != nullptr)
	{
		UAnimInstance* animInstance = m_FPSSkeletalMeshComponent->GetAnimInstance();
		if (animInstance)
		{
			animInstance->Montage_Play(m_animationMontages_FPM[a_key].m_montages[a_index], 1.0f);
		}
	}
}

void APlayerCharacter::FallBackIntoLevel()
{
	SERVER_FallBackIntoLevel();
}

void APlayerCharacter::SERVER_FallBackIntoLevel_Implementation()
{
	SetInputEnabled(false);
	m_isFallingBackToLevel_r = true;
	ApplyDamage(m_fallBackDamagePercentage, true);
	StopFiring();
	StopAiming();
	CLIENT_FallBackIntoLevel();
}

void APlayerCharacter::CLIENT_FallBackIntoLevel_Implementation()
{
	GetMovementComponent()->StopMovementImmediately();
	SetActorRotation(m_lastValidStepRotation_r);
	SetActorLocation(m_lastValidStepLocation_r + m_fallBackHight * FVector::UpVector, false, nullptr, ETeleportType::ResetPhysics);
}

void APlayerCharacter::ApplyRecoil(float a_recoilAmmount)
{
	CLIENT_ApplyRecoil(a_recoilAmmount);
}

void APlayerCharacter::CLIENT_ApplyRecoil_Implementation(float a_recoilAmmount)
{
    m_lerpedValues["Recoil"].Init(a_recoilAmmount, 0, 0.15f, 0, 3, true);
}

void APlayerCharacter::TrashWeapon()
{
	SERVER_TrashWeapon();
}

void APlayerCharacter::SERVER_TrashWeapon_Implementation()
{
	if (m_equippedWeapon_r)
	{
		//SHOULD PROBABLY SET A CAN SHOOT AND A CAN OPEN INVENTORY HERE TO AVOID GLITCHES
		PlayAnimationMontage("Discard", (int)(m_equippedWeapon_r->GetWeaponType()));
	}
}

void APlayerCharacter::FinalizeServerThrashWeapon()
{
	if (GetLocalRole() == ROLE_Authority)
	{
		if (m_equippedWeapon_r)
		{
			AWeaponPickup* trashWeapon = m_equippedWeapon_r;
			InventoryTabEnum typeOfWeapon = trashWeapon->GetPickupType();
			DropEquipped();
			trashWeapon->DestroySelf();
			m_inventoryComponent->EquipAnyWeapon(typeOfWeapon);
			//SHOULD PROBABLY SET A CAN SHOOT AND A CAN OPEN INVENTORY HERE TO AVOID GLITCHES
		}
	}
}

void APlayerCharacter::NotifyHolster()
{
	if (m_tempReferenceToWeaponBeingEquipped)
	{
		m_equippedWeapon_r->FinalizeUnequip();
		m_tempReferenceToWeaponBeingEquipped->FinalizeEquip();
	}
}

void APlayerCharacter::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	if (m_isFallingBackToLevel_r)
	{
		SetInputEnabled(true);
		m_isFallingBackToLevel_r = false;
	}
}

APickupInteractable* APlayerCharacter::GetDefaultWeapon()
{
	return m_inventoryComponent->GetDefaultWeapon();
}

void APlayerCharacter::Die()
{
	Super::Die();
	SetInputEnabled(false);
    StopFiring();
    StopAiming();
	FTimerHandle t;
	GetWorldTimerManager().SetTimer(t, this, &APlayerCharacter::Respawn, m_respawnDelay);
}

void APlayerCharacter::Respawn()
{
	Super::Respawn();
	SetInputEnabled(true);
	m_healthComponent->SetHealth(m_healthComponent->GetMaxHealth());
	if (m_currentCheckpoint_r)
	{
		CLIENT_Respawn();
	}
	else
	{
		GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Red, TEXT("I DO NOT HAVE A SPAWN POINT"));
	}
}

void APlayerCharacter::CLIENT_Respawn_Implementation()
{
	SetActorLocation(m_currentCheckpoint_r->GetActorLocation(), false, nullptr, ETeleportType::ResetPhysics);
}

void APlayerCharacter::UpdateCameraRotation()
{
	if (GetLocalRole() == ROLE_AutonomousProxy || GetLocalRole() == ROLE_Authority)
	{
		SERVER_UpdateCameraRotation(GetViewRotation());
	}
}

void APlayerCharacter::SERVER_UpdateCameraRotation_Implementation(FRotator a_viewRotation)
{
	if (m_FPSCameraComponent)
	{
		NET_UpdateCameraRotation(a_viewRotation);
	}
}

void APlayerCharacter::NET_UpdateCameraRotation_Implementation(FRotator a_viewRotation)
{
	m_FPSCameraComponent->SetWorldRotation(a_viewRotation);
}

void APlayerCharacter::MoveForward(float a_val)
{
	if (a_val != 0.0f)
	{
		// add movement in that direction
		AddMovementInput(GetActorForwardVector(), a_val * m_baseMoveSpeed* GetWorld()->GetDeltaSeconds());
	}
}

void APlayerCharacter::MoveSideways(float a_val)
{
	if (a_val != 0.0f)
	{
		// add movement in that direction
		AddMovementInput(GetActorRightVector(), a_val * m_baseMoveSpeed* GetWorld()->GetDeltaSeconds());
	}
}

void APlayerCharacter::MouseInputHorizontal(float a_val)
{
	if (GetInventoryComponent()->GetIsDisplayingInventory())
	{
		GetInventoryComponent()->MouseInputHorizontal(a_val);
	}
	else
	{
		LookHorizontal(a_val);
	}
}

void APlayerCharacter::MouseInputVertical(float a_val)
{
	if (GetInventoryComponent()->GetIsDisplayingInventory())
	{
		GetInventoryComponent()->MouseInputVertical(a_val);
	}
	else
	{
		LookVertical(a_val);
	}
}

void APlayerCharacter::LookVertical(float a_val)
{
	float sensitivity = Cast<UCapstoneGameInstance>(UGameplayStatics::GetGameInstance(GetWorld()))->m_sensitivity;

	float turnRate = (m_equippedWeapon_r != nullptr) ? (m_maxWeaponWeight - m_equippedWeapon_r->GetAttributeTotal(WeaponAttributeEnum::WAE_Weight)) / m_maxWeaponWeight : 1;
	turnRate *= m_baseTurnRate;
	turnRate *= sensitivity;

	if (Cast<UCapstoneGameInstance>(UGameplayStatics::GetGameInstance(GetWorld()))->m_invertCamera)
	{
		turnRate *= -1;
	}

	AddControllerPitchInput(a_val * turnRate * GetWorld()->GetDeltaSeconds());
	if (!m_updateCameraRotationTimer.IsValid() && GetController() && GetController()->IsLocalController())
	{
		GetWorldTimerManager().SetTimer(m_updateCameraRotationTimer, this, &APlayerCharacter::UpdateCameraRotation, 0.1f, true);
	}
}

void APlayerCharacter::LookHorizontal(float a_val)
{
	float sensitivity = Cast<UCapstoneGameInstance>(UGameplayStatics::GetGameInstance(GetWorld()))->m_sensitivity;
	float turnRate = (m_equippedWeapon_r !=nullptr) ? (m_maxWeaponWeight - m_equippedWeapon_r->GetAttributeTotal(WeaponAttributeEnum::WAE_Weight))/m_maxWeaponWeight : 1;
	turnRate *= m_baseTurnRate;
	turnRate *= sensitivity;

	if (Cast<UCapstoneGameInstance>(UGameplayStatics::GetGameInstance(GetWorld()))->m_invertCamera)
	{
		turnRate *= -1;
	}

	AddControllerYawInput(a_val * turnRate * GetWorld()->GetDeltaSeconds());
}

void APlayerCharacter::PlayerJump()
{
	if (GetMovementComponent()->IsJumpAllowed())
	{
		PlayParticle("Jump");
		PlaySound("Jump", true);
	}
	Jump();
}

void APlayerCharacter::StartFiring()
{
	SERVER_StartFiring(this);
}

void APlayerCharacter::SERVER_StartFiring_Implementation(AActor* a_actor)
{
	if (m_equippedWeapon_r)
		m_equippedWeapon_r->StartFiring(a_actor, m_isFriendlyFire);
}

void APlayerCharacter::StopFiring()
{
	SERVER_StopFiring();
}

void APlayerCharacter::SERVER_StopFiring_Implementation()
{
	if (m_equippedWeapon_r)
		m_equippedWeapon_r->StopFiring();
}

void APlayerCharacter::StartAiming()
{
	if (m_equippedWeapon_r)
	{
        m_lerpedValues["FOV"].SetTargetAlpha(1.0f);
		SERVER_StartAiming(this);
	}
}

void APlayerCharacter::SERVER_StartAiming_Implementation(AActor* a_actor)
{
    if (m_equippedWeapon_r)
    {
        m_equippedWeapon_r->SetIsAiming(true);
    }
}

void APlayerCharacter::StopAiming()
{
    if (m_equippedWeapon_r)
    {
        m_lerpedValues["FOV"].SetTargetAlpha(0.0f);
        SERVER_StopAiming();
    }
}

void APlayerCharacter::SERVER_StopAiming_Implementation()
{
	if (m_equippedWeapon_r)
	{
		m_equippedWeapon_r->SetIsAiming(false);
	}
}

void APlayerCharacter::Reload()
{
	SERVER_Reload();
}

void APlayerCharacter::SERVER_Reload_Implementation()
{
	if (m_equippedWeapon_r)
		m_equippedWeapon_r->Reload();
}

void APlayerCharacter::RayCastToFocused()
{
	SERVER_RayCastToFocused();
}

void APlayerCharacter::SERVER_RayCastToFocused_Implementation()
{
	if (m_FPSCameraComponent)
	{
		//Save the previous interactable focused for later comparison
		m_focusedInteractible[1] = m_focusedInteractible[0];
		//Raycast
		FHitResult OutHit;
		FVector Start = m_FPSCameraComponent->GetComponentLocation();
		FVector ForwardVector = m_FPSCameraComponent->GetForwardVector();
		FVector End = ((ForwardVector * m_interactDistance) + Start);
		FCollisionQueryParams CollisionParams;
		
		CollisionParams.AddIgnoredActor(this);
		GetWorld()->SweepSingleByChannel(OutHit,Start,End,FQuat(), ECC_PhysicsBody,FCollisionShape::MakeSphere(m_interactRadius), CollisionParams);
		if (OutHit.bBlockingHit)
			DrawDebugLine(GetWorld(), Start, OutHit.ImpactPoint, FColor().Red, false, 0.05, 0, 1);
		//Set the focusedInteractable even if its null
		m_focusedInteractible[0] = Cast<AInteractableActor>(OutHit.GetActor());

		CLIENT_RayCastToFocused();
	}
}

void APlayerCharacter::CLIENT_RayCastToFocused_Implementation()
{
	if (GetLocalRole() != ROLE_Authority) //if you are the autority you already did this on the SERVER version
	{
		//Save the previous interactable focused for later comparison
		m_focusedInteractible[1] = m_focusedInteractible[0];

		//Raycast
		FHitResult OutHit;
		FVector Start = m_FPSCameraComponent->GetComponentLocation();
		FVector ForwardVector = m_FPSCameraComponent->GetForwardVector();
		FVector End = ((ForwardVector * m_interactDistance) + Start);
		FCollisionQueryParams CollisionParams;
		CollisionParams.AddIgnoredActor(this);
		GetWorld()->SweepSingleByChannel(OutHit, Start, End, FQuat(), ECC_PhysicsBody, FCollisionShape::MakeSphere(m_interactRadius), CollisionParams);
		if(OutHit.bBlockingHit)
			DrawDebugLine(GetWorld(), Start, OutHit.ImpactPoint, FColor().Green, false, 0.05, 0, 1);
		//Set the focusedInteractable even if its null
		m_focusedInteractible[0] = Cast<AInteractableActor>(OutHit.GetActor());
	}

	//compare the previous and current focusedInteractible and fucus and unfocus if necessary
	if (m_focusedInteractible[0] != m_focusedInteractible[1])
	{
		if (m_focusedInteractible[0])
		{
			m_focusedInteractible[0]->Focus();
		}
		if (m_focusedInteractible[1])
		{
			m_focusedInteractible[1]->Unfocus();
		}
	}
}

void APlayerCharacter::InteractWithFocused()
{
	SERVER_InteractWithFocused();
}

void APlayerCharacter::SERVER_InteractWithFocused_Implementation()
{
	if (m_focusedInteractible[0])
	{
		if (m_focusedInteractible[0]->Interact(this))
		{
			if (m_equippedWeapon_r)
			{
				PlayAnimationMontage("Interact", (int)(m_equippedWeapon_r->GetWeaponType()));
			}
		}
	}
}

void APlayerCharacter::ShowInventory()
{
	SERVER_ShowInventory();
}

void APlayerCharacter::SERVER_ShowInventory_Implementation()
{
	m_inventoryComponent->DisplayInventory(true);
}

void APlayerCharacter::HideInventory()
{
	SERVER_HideInventory();
}

void APlayerCharacter::SERVER_HideInventory_Implementation()
{
	m_inventoryComponent->DisplayInventory(false);
}

void APlayerCharacter::DropSelected()
{
	SERVER_DropSelected();
}

void APlayerCharacter::SERVER_DropSelected_Implementation()
{
	m_inventoryComponent->DropSelected(this);
}

void APlayerCharacter::DropEquipped()
{
	SERVER_DropEquipped();
}

void APlayerCharacter::SERVER_DropEquipped_Implementation()
{
    if (m_equippedWeapon_r != nullptr)
    {
        m_inventoryComponent->DropPickup(this, m_equippedWeapon_r);
    }
        return;
}

void APlayerCharacter::UseSelected()
{
	SERVER_UseSelected();
}

void APlayerCharacter::SERVER_UseSelected_Implementation()
{
	m_inventoryComponent->UseSelected(this);
}

void APlayerCharacter::UseAt(InventoryTabEnum a_tab, int a_tabIndex)
{
    SERVER_UseAt(a_tab,a_tabIndex);
}

void APlayerCharacter::SERVER_UseAt_Implementation(InventoryTabEnum a_tab, int a_tabIndex)
{
    m_inventoryComponent->UseAt(this, a_tab,a_tabIndex);
}

void APlayerCharacter::CheckForGround()
{
	/*Initializing the variables needed for the raycast */
	FHitResult OutHitDown;
	/*Start of the ray is the location of the Muzzle*/
	FVector Start = GetActorLocation() + GetActorForwardVector() * 100;
	/*The end of the ray uses the forward vector of the The Weapon */
	FVector Direction = FVector::DownVector; //If using projectiles in the future the accuracy would be used in much the same way to set the direction of the movement.
	FVector End = ((Direction * 300.0f) + Start);//could have a range variable for this
	/*Setting the collision parameters to ignore the player when the ray is fired*/
	FCollisionQueryParams CollisionParams;
	CollisionParams.AddIgnoredActor(this);

	if (GetWorld()->LineTraceSingleByChannel(OutHitDown, Start, End, ECC_WorldStatic, CollisionParams)) //Raycast down looking for ground
	{
        if (OutHitDown.Actor != nullptr) //if the hit actor is not fucking brush
        {
            if (!OutHitDown.Actor->ActorHasTag("MovingPlatform") && !OutHitDown.Actor->ActorHasTag("FallBackTrigger")) //if its not a moving platform
            {
				/*Initializing the variables needed for the raycast */
				FHitResult OutHitUp;
				Start = GetActorLocation();
				Direction = FVector::UpVector;
				End = ((Direction * m_fallBackHight) + Start);

				if (!GetWorld()->LineTraceSingleByChannel(OutHitUp, Start, End, ECC_WorldStatic, CollisionParams)) //if there are no objects up to 10 meters above
				{
					m_lastValidStepLocation_r = OutHitDown.ImpactPoint - GetActorForwardVector() * 100; //valid step position to use later
					m_lastValidStepRotation_r = GetActorRotation();
				}
            }
        }
	}
}

void APlayerCharacter::SetCollisionForFriendlyFire()
{
	UCapstoneGameInstance* gameInstance = Cast<UCapstoneGameInstance>(GetGameInstance());
	if (gameInstance->GetInstanceIndex() == -1) //if the instance still doesn't have an index
	{
		if (m_playerIndex_r == -1) //if the player index was not replicated yet
		{
			FTimerHandle t;
			GetWorldTimerManager().SetTimer(t, this, &APlayerCharacter::SetCollisionForFriendlyFire, 1.0f, false);
			return;
		}
		else
		{
			gameInstance->SetInstanceIndex(m_playerIndex_r);
			FTimerHandle t;
			GetWorldTimerManager().SetTimer(t, this, &APlayerCharacter::SetCollisionForFriendlyFire, 1.0f, false);
			return;
		}
	}
	else
	{
		SERVER_SetCollisionForFriendlyFire(gameInstance->GetInstanceIndex());
	}
}

void APlayerCharacter::CLIENT_SetCollisionForFriendlyFire_Implementation()
{
	FTimerHandle t;
	GetWorldTimerManager().SetTimer(t, this, &APlayerCharacter::SetCollisionForFriendlyFire, 1.0f, false);
}

void APlayerCharacter::SERVER_SetCollisionForFriendlyFire_Implementation(int a_playerIndex)
{
	NET_SetCollisionForFriendlyFire(a_playerIndex);
}

void APlayerCharacter::NET_SetCollisionForFriendlyFire_Implementation(int a_playerIndex)
{
	m_playerIndex_r = a_playerIndex;
	FString profileName = "PlayerCollision";
	profileName.AppendInt(m_playerIndex_r);
	GetCapsuleComponent()->SetCollisionProfileName(*profileName);
}

void APlayerCharacter::EnableOnLevelStart()
{
	TArray<APlayerState*> players = GetWorld()->GetAuthGameMode()->GameState->PlayerArray;
	for (APlayerState* playerState : players)
	{
		AMyCharacter* character = Cast<AMyCharacter>(playerState->GetPawn());
		if (!character->GetHasTickedOnce())
		{
			FTimerHandle t;
			GetWorldTimerManager().SetTimer(t, this, &APlayerCharacter::EnableOnLevelStart, 3.0f, false);

			return;
		}
	}
	SetInputEnabled(true);
	InitiateFadeFromBlack(this);
}

void APlayerCharacter::GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(APlayerCharacter, m_currentCheckpoint_r)
	DOREPLIFETIME(APlayerCharacter, m_equippedWeapon_r)
	DOREPLIFETIME(APlayerCharacter, m_lastValidStepLocation_r)
	DOREPLIFETIME(APlayerCharacter, m_lastValidStepRotation_r)
	DOREPLIFETIME(APlayerCharacter, m_playerIndex_r)
	DOREPLIFETIME(APlayerCharacter, m_isFallingBackToLevel_r)
}
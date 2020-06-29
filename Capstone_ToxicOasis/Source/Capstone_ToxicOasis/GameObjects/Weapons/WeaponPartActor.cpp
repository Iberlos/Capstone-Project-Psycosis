// Fill out your copyright notice in the Description page of Project Settings.


#include "WeaponPartActor.h"
#include "../../EnumMaster/EnumMaster.h"
#include "UnrealNetwork.h"
#include "Components/SkeletalMeshComponent.h"

AWeaponPartActor::AWeaponPartActor()
{
	GetSkeletalMeshComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision); //Had to add this for a test
	//GetSkeletalMeshComponent()->SetIsReplicated(true);
    //SetupWeaponAttributeArray();


	//Setup Networking info
	bAlwaysRelevant = false;
	NetPriority = 2;
	SetReplicates(true);
    //Testing	
    SetActorTickEnabled(false);
    NetUpdateFrequency = 0;
    //SetReplicateMovement(true);
}

void AWeaponPartActor::SetupWeaponAttributeArray()
{
    m_weaponAttributeArray.Init(NULL, 6);
    m_weaponAttributeArray[(int)WeaponAttributeEnum::WAE_Damage] = m_partDamage;
    m_weaponAttributeArray[(int)WeaponAttributeEnum::WAE_Accuracy] = m_partAccuracy;
    m_weaponAttributeArray[(int)WeaponAttributeEnum::WAE_FireRate] = m_partFireRate;
    m_weaponAttributeArray[(int)WeaponAttributeEnum::WAE_Recoil] = m_partRecoil;
    m_weaponAttributeArray[(int)WeaponAttributeEnum::WAE_MagazineSize] = m_partMagSize;
    m_weaponAttributeArray[(int)WeaponAttributeEnum::WAE_Weight] = m_partWeight;
}

void AWeaponPartActor::GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    DOREPLIFETIME(AWeaponPartActor, m_partDamage);
    DOREPLIFETIME(AWeaponPartActor, m_partAccuracy);
    DOREPLIFETIME(AWeaponPartActor, m_partFireRate);
    DOREPLIFETIME(AWeaponPartActor, m_partRecoil);
    DOREPLIFETIME(AWeaponPartActor, m_partMagSize);
    DOREPLIFETIME(AWeaponPartActor, m_partWeight);
    DOREPLIFETIME(AWeaponPartActor, m_weaponAttributeArray);

    //Enums
    //DOREPLIFETIME(AWeaponPartActor, m_startEffect);
}
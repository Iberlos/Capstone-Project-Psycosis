// Fill out your copyright notice in the Description page of Project Settings.


#include "MainGameMode.h"

#include "Kismet/GameplayStatics.h"
#include "Engine.h"
#include "MoviePlayer.h"
#include "GameObjects/Players/PlayerCharacter.h"
#include "GameObjects/Weapons/WeaponGeneratorActor.h"
#include "GameObjects/Interactables/Pickups/Weapons/WeaponPickup.h"	
#include "GameObjects/Projectiles/PrototypeProjectileEffect/PrototypeProjectile.h"


AMainGameMode::AMainGameMode()
{
    m_playerList.Init(nullptr, 0);
}

void AMainGameMode::PostLogin(APlayerController* NewPlayer)
{
    Super::PostLogin(NewPlayer);
    AController* controller = NewPlayer;
    if (controller)
    {
        APlayerCharacter* player = Cast<APlayerCharacter>(controller->GetPawn());
        if (player != nullptr)
        {
            AddPlayerToList(player);
        }
    }
}

void AMainGameMode::StartPlay()
{
    Super::StartPlay();

    //m_worldWeaponGenerator = GetWorld()->SpawnActor<AWeaponGeneratorActor>(m_WeaponGenerator, FVector(0, 0, 0), FRotator(0, 0, 0));	

//GetWorld()->GetTimerManager().SetTimer(CheckLoadCompleteTimer, this, &AMainGameMode::VerifyLoadingComplete, 0.5f, true, 0.f);	
//VerifyLoadingComplete();	
//m_worldWeaponGenerator->SERVER_PreGenerateWeaponPool();	
    FindWeaponGeneratorInWorld();
    //GetMoviePlayer()->StopMovie();
}

AWeaponGeneratorActor* AMainGameMode::GetWorldWeaponGenerator()
{
    if (m_worldWeaponGenerator)
    {
        return m_worldWeaponGenerator;
    }
    else
    {
        return nullptr;
    }

}

void AMainGameMode::AddNewBulletToPool(APrototypeProjectile* newBullet)
{
    //Cast<UPrimitiveComponent>(newBullet->GetRootComponent())->SetSimulatePhysics(false);	
    newBullet->TeleportTo(FVector(0, -10000, 0), FRotator(0, 0, 0));
    newBullet->ClearEffects();
    newBullet->SERVER_DisableBullet();
    CurrentBulletPool.Add(newBullet);
}

APrototypeProjectile* AMainGameMode::GetBulletFromPool()
{
    if (CurrentBulletPool.Num() > 0)
    {
        APrototypeProjectile* tempBullet = CurrentBulletPool[CurrentBulletPool.Num() - 1];
        CurrentBulletPool.Remove(tempBullet);
        //Cast<UPrimitiveComponent>(tempBullet->GetRootComponent())->SetSimulatePhysics(false);	
        if (tempBullet)
        {
            tempBullet->SERVER_EnableBullet();
        }
        return tempBullet;
    }
    else
    {
        return nullptr;
    }
}

bool AMainGameMode::IsBulletPoolValid()
{
    if (CurrentBulletPool.Num() > 0)
    {
        return true;
    }
    else
    {
        return false;
    }
}

void AMainGameMode::AddNewWeaponToPool(AWeaponPickup* newWeapon)
{
    //newWeapon->TeleportTo(FVector(-10000, -10000, 0), FRotator(0, 0, 0));	
    //CurrentWeaponPool.Add(newWeapon);	
    //newWeapon->SERVER_DisableWeapon();	
    //New System to include weapon rarity	
    newWeapon->TeleportTo(FVector(-10000, -10000, 0), FRotator(0, 0, 0));
    switch (newWeapon->GetWeaponRarity())
    {
    case WeaponRarityEnum::WRE_Common:
        CommonWeaponPool.Add(newWeapon);
        break;
    case WeaponRarityEnum::WRE_Uncommon:
        UncommonWeaponPool.Add(newWeapon);
        break;
    case WeaponRarityEnum::WRE_Rare:
        RareWeaponPool.Add(newWeapon);
        break;
    case WeaponRarityEnum::WRE_Legendary:
        LegendaryWeaponPool.Add(newWeapon);
        break;
    }
    newWeapon->SERVER_DisableWeapon();
}

//AWeaponPickup* AMainGameMode::GetWeaponFromPool()
//{
//    if (CurrentWeaponPool.Num() > 0)
//    {
//        int randomValue = FMath::FRandRange(0, CurrentWeaponPool.Num() - 1);
//        AWeaponPickup* weaponToReturn = CurrentWeaponPool[randomValue];
//        //CurrentWeaponPool.RemoveAt(randomValue);	
//        CurrentWeaponPool.Remove(weaponToReturn);
//        if (weaponToReturn)
//        {
//            weaponToReturn->SERVER_EnableWeapon();
//        }
//        return weaponToReturn;
//    }
//    else
//    {
//        return nullptr;
//    }
//}

AWeaponPickup* AMainGameMode::GetWeaponFromPool(WeaponRarityEnum weaponRarity)
{
    switch (weaponRarity)
    {
    case WeaponRarityEnum::WRE_Common:
        if (CommonWeaponPool.Num() > 0)
        {
            int randomValue = FMath::FRandRange(0, CommonWeaponPool.Num() - 1);
            AWeaponPickup* weaponToReturn = CommonWeaponPool[randomValue];
            //CurrentWeaponPool.RemoveAt(randomValue);	
            CommonWeaponPool.Remove(weaponToReturn);
            if (weaponToReturn)
            {
                weaponToReturn->SERVER_EnableWeapon();
            }
            return weaponToReturn;
        }
        break;
    case WeaponRarityEnum::WRE_Uncommon:
        if (UncommonWeaponPool.Num() > 0)
        {
            int randomValue = FMath::FRandRange(0, UncommonWeaponPool.Num() - 1);
            AWeaponPickup* weaponToReturn = UncommonWeaponPool[randomValue];
            UncommonWeaponPool.Remove(weaponToReturn);
            if (weaponToReturn)
            {
                weaponToReturn->SERVER_EnableWeapon();
            }
            return weaponToReturn;
        }
        break;
    case WeaponRarityEnum::WRE_Rare:
        if (RareWeaponPool.Num() > 0)
        {
            int randomValue = FMath::FRandRange(0, RareWeaponPool.Num() - 1);
            AWeaponPickup* weaponToReturn = RareWeaponPool[randomValue];
            RareWeaponPool.Remove(weaponToReturn);
            if (weaponToReturn)
            {
                weaponToReturn->SERVER_EnableWeapon();
            }
            return weaponToReturn;
        }
        break;
    case WeaponRarityEnum::WRE_Legendary:
        if (LegendaryWeaponPool.Num() > 0)
        {
            int randomValue = FMath::FRandRange(0, LegendaryWeaponPool.Num() - 1);
            AWeaponPickup* weaponToReturn = LegendaryWeaponPool[randomValue];
            LegendaryWeaponPool.Remove(weaponToReturn);
            if (weaponToReturn)
            {
                weaponToReturn->SERVER_EnableWeapon();
            }
            return weaponToReturn;
        }
        break;
        //default:	
        //    return nullptr;	
        //    break;	
    }
    return nullptr;
}

bool AMainGameMode::IsWeaponPoolValid()
{
    if (CurrentWeaponPool.Num() > 0)
    {
        return true;
    }
    else
    {
        return false;
    }
}

void AMainGameMode::SetGameIsReadyToStart()
{
    SetMatchState(MatchState::WaitingToStart);
}

void AMainGameMode::CreateWeaponPool()
{
    if (m_worldWeaponGenerator != nullptr && m_worldWeaponGenerator->GetLocalRole() == ROLE_Authority)
    {
        int numWeapons = 50;
        for (int i = 0; i < numWeapons; i++)
        {
            FActorSpawnParameters pickupSpawnParams;
            //APickupInteractable* pickup = GetWorld()->SpawnActor<APickupInteractable>(FVector(0, 0, 0), FRotator(0, 0, 0));	
            AWeaponPickup* weapon = GetWorld()->SpawnActor<AWeaponPickup>(FVector(0, 0, 0), FRotator(0, 0, 0));
            //AWeaponPickup* weapon = Cast<AWeaponPickup>(pickup);	
            m_worldWeaponGenerator->GenerateWeaponFromTemplate(weapon);
            AddNewWeaponToPool(weapon);
        }
    }
}

void AMainGameMode::FindWeaponGeneratorInWorld()
{
    TArray<AActor*> FoundObjects;
    UGameplayStatics::GetAllActorsOfClass(GetWorld(), AWeaponGeneratorActor::StaticClass(), FoundObjects);
    if (FoundObjects.Num() > 0)
    {
        m_worldWeaponGenerator = Cast<AWeaponGeneratorActor>(FoundObjects[0]);
    }
    else
    {
        GEngine->AddOnScreenDebugMessage(-1, 50.0f, FColor::Red, TEXT("THERE IS NO WEAPON GENERATOR SOURCE IN THE LEVEL"));
    }
}

void AMainGameMode::VerifyLoadingComplete()
{
    if (GetLocalRole() == ROLE_Authority)
    {
        //if (IsWeaponPoolValid() == true && IsBulletPoolValid() == true)	
        //{	
        //    NETMC_EndLoadingScreen();	
        //}	
        //if (CurrentWeaponPool.Num() == 50 && CurrentBulletPool.Num() == 500)	
        if (CurrentWeaponPool.Num() > 40 && CurrentBulletPool.Num() > 400)
        {
            NETMC_EndLoadingScreen();
        }
        GetWorldTimerManager().ClearTimer(CheckLoadCompleteTimer);
    }
}

void AMainGameMode::NETMC_EndLoadingScreen_Implementation()
{
    GetMoviePlayer()->StopMovie();
}

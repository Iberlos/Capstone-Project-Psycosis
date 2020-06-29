// Fill out your copyright notice in the Description page of Project Settings.


#include "WeaponGeneratorObject.h"

//Other Classes
#include "WeaponPartActor.h"
#include "../Interactables/Pickups/Weapons/WeaponPickup.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components//BoxComponent.h"
#include "GameObjects/Projectiles/ProjectileActor.h"

//Functionality Includes
#include "Runtime/Engine/Classes/Engine/Engine.h"
#include "Runtime/AssetRegistry/Public/AssetRegistryModule.h"
#include "Runtime/AssetRegistry/Public/ARFilter.h"
#include "Kismet/KismetMathLibrary.h"
#include "UObject/UObjectGlobals.h"
#include "UObject/ConstructorHelpers.h"
#include "UnrealNetwork.h"

UWeaponGeneratorObject::UWeaponGeneratorObject()
{
    SERVER_PopulateWeaponPartArrays();
}

void UWeaponGeneratorObject::GenerateWeaponFromTemplate(class AWeaponPickup* WeaponTemplate)
{   
    int RandomWeaponChoice = FMath::RandRange(1, 3);
    //int RandomWeaponChoice = 1;

    //Rifle Weapon Generation/Attachment Logic
#pragma region
    if (RandomWeaponChoice == 1)
    {
        int randReceiverNum = FMath::RandRange(0, m_rifleReceiverWeaponParts.Num() - 1);
        int randBarrelNum = FMath::RandRange(0, m_rifleBarrelWeaponParts.Num() - 1);
        int randMagNum = FMath::RandRange(0, m_rifleMagazineWeaponParts.Num() - 1);
        int randOpticNum = FMath::RandRange(0, m_rifleOpticWeaponParts.Num() - 1);
        int randStockNum = FMath::RandRange(0, m_rifleStockWeaponParts.Num() - 1);


        //Original Generation Source Code
#pragma region
        int RandY = FMath::RandRange(-400, 400);
        //if (m_rifleReceiverWeaponParts.IsValidIndex(randReceiverNum))
        //if (m_rifleReceiverWeaponParts[randReceiverNum]->IsValidLowLevel())
        //if (IsValid(m_rifleReceiverWeaponParts[randReceiverNum]))
        //{
            AWeaponPartActor* TempReceiver = GetWorld()->SpawnActor<AWeaponPartActor>
                //(m_rifleReceiverWeaponParts[FMath::RandRange(0, m_rifleReceiverWeaponParts.Num() - 1)], FVector(0.f, RandY, 190.f), FRotator(0, 0, 0));
                (m_rifleReceiverWeaponParts[randReceiverNum], FVector(0.f, RandY, 190.f), FRotator(0, 0, 0));
            TempReceiver->AttachToActor(WeaponTemplate, FAttachmentTransformRules::SnapToTargetNotIncludingScale);
            
            AWeaponPartActor* TempBarrel = GetWorld()->SpawnActor<AWeaponPartActor>
                //(m_rifleBarrelWeaponParts[FMath::RandRange(0, m_rifleBarrelWeaponParts.Num() - 1)], FVector(0.f, RandY, 190.f), FRotator(0, 0, 0));
                (m_rifleBarrelWeaponParts[randBarrelNum], FVector(0.f, RandY, 190.f), FRotator(0, 0, 0));
            TempBarrel->AttachToActor(TempReceiver, FAttachmentTransformRules::SnapToTargetNotIncludingScale, FName("Barrel_Socket"));

            AWeaponPartActor* TempMag = GetWorld()->SpawnActor<AWeaponPartActor>
                //(m_rifleMagazineWeaponParts[FMath::RandRange(0, m_rifleMagazineWeaponParts.Num() - 1)], FVector(0.f, RandY, 190.f), FRotator(0, 0, 0));
                (m_rifleMagazineWeaponParts[randMagNum], FVector(0.f, RandY, 190.f), FRotator(0, 0, 0));
            TempMag->AttachToActor(TempReceiver, FAttachmentTransformRules::SnapToTargetNotIncludingScale, FName("Magazine_Socket"));

            AWeaponPartActor* TempOptic = GetWorld()->SpawnActor<AWeaponPartActor>
                //(m_rifleOpticWeaponParts[FMath::RandRange(0, m_rifleOpticWeaponParts.Num() - 1)], FVector(0.f, RandY, 190.f), FRotator(0, 0, 0));
                (m_rifleOpticWeaponParts[randOpticNum], FVector(0.f, RandY, 190.f), FRotator(0, 0, 0));
            TempOptic->AttachToActor(TempBarrel, FAttachmentTransformRules::SnapToTargetNotIncludingScale, FName("Optic_Socket"));

            AWeaponPartActor* TempStock = GetWorld()->SpawnActor<AWeaponPartActor>
                //(m_rifleStockWeaponParts[FMath::RandRange(0, m_rifleStockWeaponParts.Num() - 1)], FVector(0.f, RandY, 190.f), FRotator(0, 0, 0));
                (m_rifleStockWeaponParts[randStockNum], FVector(0.f, RandY, 190.f), FRotator(0, 0, 0));
            TempStock->AttachToActor(TempReceiver, FAttachmentTransformRules::SnapToTargetNotIncludingScale, FName("Stock_Socket"));

            WeaponTemplate->AddNewWeaponPartToWeaponPartArray(TempBarrel);
            WeaponTemplate->AddNewWeaponPartToWeaponPartArray(TempReceiver);
            WeaponTemplate->AddNewWeaponPartToWeaponPartArray(TempMag);
            WeaponTemplate->AddNewWeaponPartToWeaponPartArray(TempStock);
            WeaponTemplate->AddNewWeaponPartToWeaponPartArray(TempOptic);

            WeaponTemplate->RegisterMesh(Cast<USkeletalMeshComponent>(TempBarrel->GetRootComponent()));
            WeaponTemplate->RegisterMesh(Cast<USkeletalMeshComponent>(TempReceiver->GetRootComponent()));
            WeaponTemplate->RegisterMesh(Cast<USkeletalMeshComponent>(TempStock->GetRootComponent()));
            WeaponTemplate->RegisterMesh(Cast<USkeletalMeshComponent>(TempMag->GetRootComponent()));
            WeaponTemplate->RegisterMesh(Cast<USkeletalMeshComponent>(TempOptic->GetRootComponent()));

            //WeaponTemplate->CompleteWeaponAssembly();

            WeaponTemplate->SetPickupType(InventoryTabEnum::ITE_Rifles);
            WeaponTemplate->SetWeaponType(WeaponTypeEnum::WTE_Rifle);

            //Randomization for the weapon rarity multiplier
            int rarityTemp = FMath::RandRange(1, 4);
            switch (rarityTemp)
            {
            case 1:
                WeaponTemplate->SetWeaponRarity(WeaponRarityEnum::WRE_Common);
                break;
            case 2:
                WeaponTemplate->SetWeaponRarity(WeaponRarityEnum::WRE_Uncommon);
                break;
            case 3:
                WeaponTemplate->SetWeaponRarity(WeaponRarityEnum::WRE_Rare);
                break;
            case 4:
                WeaponTemplate->SetWeaponRarity(WeaponRarityEnum::WRE_Legendary);
                break;
            default:
                WeaponTemplate->SetWeaponRarity(WeaponRarityEnum::WRE_Common);
                break;
            }

            int fireModeTemp = FMath::RandRange(1, 3);
            switch (fireModeTemp)
            {
            case 1:
                WeaponTemplate->SetWeaponFireMode(WeaponFireModeEnum::WFME_Single);
                break;
            case 2:
                WeaponTemplate->SetWeaponFireMode(WeaponFireModeEnum::WFME_Burst);
                break;
            case 3:
                WeaponTemplate->SetWeaponFireMode(WeaponFireModeEnum::WFME_Auto);
                break;
            default:
                WeaponTemplate->SetWeaponFireMode(WeaponFireModeEnum::WFME_Single);
                break;
            }

            WeaponTemplate->CompleteWeaponAssembly();
        //}
        //else
        //{
        //    WeaponTemplate->Destroy();
        //}
        


        //.isValidLowLevel();
#pragma endregion

        


    }
#pragma endregion

    //Pistol Weapon Generation/Attachment Logic
#pragma region
    if (RandomWeaponChoice == 2)
    {
        int RandY = FMath::RandRange(-400, 400);
        AWeaponPartActor* TempFrame = GetWorld()->SpawnActor<AWeaponPartActor>
            (m_pistolFrameParts[FMath::RandRange(0, m_pistolFrameParts.Num() - 1)], FVector(0.f, RandY, 190.f), FRotator(0, 0, 0));
        TempFrame->AttachToActor(WeaponTemplate, FAttachmentTransformRules::SnapToTargetNotIncludingScale);

        AWeaponPartActor* TempBarrel = GetWorld()->SpawnActor<AWeaponPartActor>
            (m_pistolBarrelParts[FMath::RandRange(0, m_pistolBarrelParts.Num() - 1)], FVector(0.f, RandY, 190.f), FRotator(0, 0, 0));
        TempBarrel->AttachToActor(TempFrame, FAttachmentTransformRules::SnapToTargetNotIncludingScale, FName("Barrel_Socket"));

        AWeaponPartActor* TempMag = GetWorld()->SpawnActor<AWeaponPartActor>
            (m_pistolMagazineParts[FMath::RandRange(0, m_pistolMagazineParts.Num() - 1)], FVector(0.f, RandY, 190.f), FRotator(0, 0, 0));
        TempMag->AttachToActor(TempFrame, FAttachmentTransformRules::SnapToTargetNotIncludingScale, FName("Magazine_Socket"));

        AWeaponPartActor* TempSlide = GetWorld()->SpawnActor<AWeaponPartActor>
            (m_pistolSlideParts[FMath::RandRange(0, m_pistolSlideParts.Num() - 1)], FVector(0.f, RandY, 190.f), FRotator(0, 0, 0));
        TempSlide->AttachToActor(TempFrame, FAttachmentTransformRules::SnapToTargetNotIncludingScale, FName("Slide_Socket"));

        AWeaponPartActor* TempTrigGuard = GetWorld()->SpawnActor<AWeaponPartActor>
            (m_pistolTriggerGuardParts[FMath::RandRange(0, m_pistolTriggerGuardParts.Num() - 1)], FVector(0.f, RandY, 190.f), FRotator(0, 0, 0));
        TempTrigGuard->AttachToActor(TempFrame, FAttachmentTransformRules::SnapToTargetNotIncludingScale, FName("TriggerGuard_Socket"));

        WeaponTemplate->AddNewWeaponPartToWeaponPartArray(TempFrame);
        WeaponTemplate->AddNewWeaponPartToWeaponPartArray(TempBarrel);
        WeaponTemplate->AddNewWeaponPartToWeaponPartArray(TempMag);
        WeaponTemplate->AddNewWeaponPartToWeaponPartArray(TempSlide);
        WeaponTemplate->AddNewWeaponPartToWeaponPartArray(TempTrigGuard);

        WeaponTemplate->RegisterMesh(Cast<USkeletalMeshComponent>(TempFrame->GetRootComponent()));
        WeaponTemplate->RegisterMesh(Cast<USkeletalMeshComponent>(TempBarrel->GetRootComponent()));
        WeaponTemplate->RegisterMesh(Cast<USkeletalMeshComponent>(TempMag->GetRootComponent()));
        WeaponTemplate->RegisterMesh(Cast<USkeletalMeshComponent>(TempSlide->GetRootComponent()));
        WeaponTemplate->RegisterMesh(Cast<USkeletalMeshComponent>(TempTrigGuard->GetRootComponent()));

        //WeaponTemplate->CompleteWeaponAssembly();
        WeaponTemplate->SetPickupType(InventoryTabEnum::ITE_Pistols);
        WeaponTemplate->SetWeaponType(WeaponTypeEnum::WTE_Pistol);
        WeaponTemplate->GetColliderComponent()->SetBoxExtent(FVector(16.f, 7.f, 14.f));

        //Randomization for the weapon rarity multiplier
        int rarityTemp = FMath::RandRange(1, 4);
        switch (rarityTemp)
        {
        case 1:
            WeaponTemplate->SetWeaponRarity(WeaponRarityEnum::WRE_Common);
            break;
        case 2:
            WeaponTemplate->SetWeaponRarity(WeaponRarityEnum::WRE_Uncommon);
            break;
        case 3:
            WeaponTemplate->SetWeaponRarity(WeaponRarityEnum::WRE_Rare);
            break;
        case 4:
            WeaponTemplate->SetWeaponRarity(WeaponRarityEnum::WRE_Legendary);
            break;
        default:
            WeaponTemplate->SetWeaponRarity(WeaponRarityEnum::WRE_Common);
            break;
        }

        int fireModeTemp = FMath::RandRange(1, 3);
        switch (fireModeTemp)
        {
        case 1:
            WeaponTemplate->SetWeaponFireMode(WeaponFireModeEnum::WFME_Single);
            break;
        case 2:
            WeaponTemplate->SetWeaponFireMode(WeaponFireModeEnum::WFME_Burst);
            break;
        case 3:
            WeaponTemplate->SetWeaponFireMode(WeaponFireModeEnum::WFME_Auto);
            break;
        default:
            WeaponTemplate->SetWeaponFireMode(WeaponFireModeEnum::WFME_Single);
            break;
        }

        WeaponTemplate->CompleteWeaponAssembly();
        
    }
#pragma endregion

    //Shotgun Weapon Generation/Attachment Logic
#pragma region
    if (RandomWeaponChoice == 3)
    {
        int RandY = FMath::RandRange(-400, 400);
        AWeaponPartActor* TempReceiver = GetWorld()->SpawnActor<AWeaponPartActor>
            (m_shotgunReceiverParts[FMath::RandRange(0, m_shotgunReceiverParts.Num() - 1)], FVector(0.f, RandY, 190.f), FRotator(0, 0, 0));
        TempReceiver->AttachToActor(WeaponTemplate, FAttachmentTransformRules::SnapToTargetNotIncludingScale);

        AWeaponPartActor* TempBarrel = GetWorld()->SpawnActor<AWeaponPartActor>
            (m_shotgunBarrelParts[FMath::RandRange(0, m_shotgunBarrelParts.Num() - 1)], FVector(0.f, RandY, 190.f), FRotator(0, 0, 0));
        TempBarrel->AttachToActor(TempReceiver, FAttachmentTransformRules::SnapToTargetNotIncludingScale, FName("Barrel_Socket"));

        AWeaponPartActor* TempTube = GetWorld()->SpawnActor<AWeaponPartActor>
            (m_shotgunTubeParts[FMath::RandRange(0, m_shotgunTubeParts.Num() - 1)], FVector(0.f, RandY, 190.f), FRotator(0, 0, 0));
        TempTube->AttachToActor(TempReceiver, FAttachmentTransformRules::SnapToTargetNotIncludingScale, FName("Tube_Socket"));

        AWeaponPartActor* TempPump = GetWorld()->SpawnActor<AWeaponPartActor>
            (m_shotgunPumpParts[FMath::RandRange(0, m_shotgunPumpParts.Num() - 1)], FVector(0.f, RandY, 190.f), FRotator(0, 0, 0));
        TempPump->AttachToActor(TempReceiver, FAttachmentTransformRules::SnapToTargetNotIncludingScale, FName("Pump_Socket"));

        AWeaponPartActor* TempStock = GetWorld()->SpawnActor<AWeaponPartActor>
            (m_shotgunStockParts[FMath::RandRange(0, m_shotgunStockParts.Num() - 1)], FVector(0.f, RandY, 190.f), FRotator(0, 0, 0));
        TempStock->AttachToActor(TempReceiver, FAttachmentTransformRules::SnapToTargetNotIncludingScale, FName("Stock_Socket"));

        WeaponTemplate->AddNewWeaponPartToWeaponPartArray(TempReceiver);
        WeaponTemplate->AddNewWeaponPartToWeaponPartArray(TempBarrel);
        WeaponTemplate->AddNewWeaponPartToWeaponPartArray(TempTube);
        WeaponTemplate->AddNewWeaponPartToWeaponPartArray(TempPump);
        WeaponTemplate->AddNewWeaponPartToWeaponPartArray(TempStock);

        WeaponTemplate->RegisterMesh(Cast<USkeletalMeshComponent>(TempReceiver->GetRootComponent()));
        WeaponTemplate->RegisterMesh(Cast<USkeletalMeshComponent>(TempBarrel->GetRootComponent()));
        WeaponTemplate->RegisterMesh(Cast<USkeletalMeshComponent>(TempTube->GetRootComponent()));
        WeaponTemplate->RegisterMesh(Cast<USkeletalMeshComponent>(TempPump->GetRootComponent()));
        WeaponTemplate->RegisterMesh(Cast<USkeletalMeshComponent>(TempStock->GetRootComponent()));

        WeaponTemplate->SetPickupType(InventoryTabEnum::ITE_Shotguns);
        WeaponTemplate->SetWeaponType(WeaponTypeEnum::WTE_Shotgun);

        //Randomization for the weapon rarity multiplier
        int rarityTemp = FMath::RandRange(1, 4);
        switch (rarityTemp)
        {
        case 1:
            WeaponTemplate->SetWeaponRarity(WeaponRarityEnum::WRE_Common);
            break;
        case 2:
            WeaponTemplate->SetWeaponRarity(WeaponRarityEnum::WRE_Uncommon);
            break;
        case 3:
            WeaponTemplate->SetWeaponRarity(WeaponRarityEnum::WRE_Rare);
            break;
        case 4:
            WeaponTemplate->SetWeaponRarity(WeaponRarityEnum::WRE_Legendary);
            break;
        default:
            WeaponTemplate->SetWeaponRarity(WeaponRarityEnum::WRE_Common);
            break;
        }
        WeaponTemplate->SetWeaponFireMode(WeaponFireModeEnum::WFME_Single);
       /* int fireModeTemp = FMath::RandRange(1, 3);
        switch (fireModeTemp)
        {
        case 1:
            WeaponTemplate->SetWeaponFireMode(WeaponFireModeEnum::WFME_Single);
            break;
        case 2:
            WeaponTemplate->SetWeaponFireMode(WeaponFireModeEnum::WFME_Burst);
            break;
        case 3:
            WeaponTemplate->SetWeaponFireMode(WeaponFireModeEnum::WFME_Auto);
            break;
        default:
            WeaponTemplate->SetWeaponFireMode(WeaponFireModeEnum::WFME_Single);
            break;
        }*/

        WeaponTemplate->CompleteWeaponAssembly();
    }
#pragma endregion

    WeaponTemplate->SetWeaponProjectile(m_weaponProjectile);
}

//Fills the weapon part arrays for all weapon types
void UWeaponGeneratorObject::SERVER_PopulateWeaponPartArrays_Implementation()
{
    //Setup registry module
    FAssetRegistryModule& AssetRegistryModule = FModuleManager::LoadModuleChecked<FAssetRegistryModule>("AssetRegistry");
    
    //Holds the path to all the weapon type folders
    FString TypesOfWeaponPath = "/Game/Art/3D_Models/Non_Organic/Guns/";

    //Sets the part of the path to the specified weapon
    FString WeaponTypePath = "Rifle/";//Default Weapon Type

    //Array that holds the part of the path to the weapon parts
    TArray<FString> RifleWeaponTypePartPath = {"Barrel", "Magazine", "Optic", "Receiver", "Stock"};
    TArray<FString> PistolWeaponTypePartPath = {"Barrel", "Frame", "Magazine", "Slide", "TriggerGuard"};
    TArray<FString> ShotgunWeaponTypePartPath = {"Barrel", "Magazine", "Pump", "Receiver", "Stock"};

    //Asset data array to hold the results of what gets found
    TArray<FAssetData> AssetData;

    //Filter to sort out criteria for what type of things will be searched for
    FARFilter dataFilter;

    //Telling the filter to only look for objects with the class name of Blueprint
    dataFilter.ClassNames.Add("Blueprint");

    //Filling Rifle part arrays
#pragma region
    WeaponTypePath = "Rifle/";
    for (int i = 0; i < RifleWeaponTypePartPath.Num(); i++)
    {
        FString PartPath = TypesOfWeaponPath + WeaponTypePath + RifleWeaponTypePartPath[i];
        //FString PartPath = "/Game/Art/3D_Models/Non_Organic/Guns/Rifle/Barrel";
        dataFilter.PackagePaths.Add(FName(*PartPath));
        AssetRegistryModule.Get().GetAssets(dataFilter, AssetData);
        if (AssetData.Num() > 0)
        {
            if (RifleWeaponTypePartPath[i] == "Barrel")
            {
                for (int j = 0; j < AssetData.Num(); j++)
                {
                    //Initial attempt to load object from FObjectFinder but StaticLoadObject seems to work, not sure of difference but will look into later

                    //m_rifleBarrelWeaponParts.Add(Cast<UClass>(StaticLoadObject(UObject::StaticClass(), nullptr, *("Blueprint'" + AssetData[j].ObjectPath.ToString() + "_C'")    )));
                    m_rifleBarrelWeaponParts.Add(StaticLoadClass(AWeaponPartActor::StaticClass(), nullptr, *("Blueprint'" + AssetData[j].ObjectPath.ToString() + "_C'")));
                }
            }
            if (RifleWeaponTypePartPath[i] == "Magazine")
            {
                for (int j = 0; j < AssetData.Num(); j++)
                {
                    //m_rifleReceiverWeaponParts.Add(Cast<UClass>(StaticLoadObject(UObject::StaticClass(), nullptr, *("Blueprint'" + AssetData[j].ObjectPath.ToString() + "_C'")  )));
                    m_rifleMagazineWeaponParts.Add(StaticLoadClass(AWeaponPartActor::StaticClass(), nullptr, *("Blueprint'" + AssetData[j].ObjectPath.ToString() + "_C'")));
                }
            }
            if (RifleWeaponTypePartPath[i] == "Receiver")
            {
                for (int j = 0; j < AssetData.Num(); j++)
                {
                    //m_rifleReceiverWeaponParts.Add(Cast<UClass>(StaticLoadObject(UObject::StaticClass(), nullptr, *("Blueprint'" + AssetData[j].ObjectPath.ToString() + "_C'")  )));
                    m_rifleReceiverWeaponParts.Add(StaticLoadClass(AWeaponPartActor::StaticClass(), nullptr, *("Blueprint'" + AssetData[j].ObjectPath.ToString() + "_C'")  ));
                    //m_rifleReceiver = StaticLoadClass(AWeaponPartActor::StaticClass(), nullptr, *("Blueprint'" + AssetData[j].ObjectPath.ToString() + "_C'"));
                }
            }
            if (RifleWeaponTypePartPath[i] == "Optic")
            {
                for (int j = 0; j < AssetData.Num(); j++)
                {
                    //m_rifleReceiverWeaponParts.Add(Cast<UClass>(StaticLoadObject(UObject::StaticClass(), nullptr, *("Blueprint'" + AssetData[j].ObjectPath.ToString() + "_C'")  )));
                    m_rifleOpticWeaponParts.Add(StaticLoadClass(AWeaponPartActor::StaticClass(), nullptr, *("Blueprint'" + AssetData[j].ObjectPath.ToString() + "_C'")));
                }
            }
            if (RifleWeaponTypePartPath[i] == "Stock")
            {
                for (int j = 0; j < AssetData.Num(); j++)
                {

                    //m_rifleStockWeaponParts.Add(Cast<UClass>(StaticLoadObject(UObject::StaticClass(), nullptr, *("Blueprint'" + AssetData[j].ObjectPath.ToString() + "_C'") )));

                    m_rifleStockWeaponParts.Add(StaticLoadClass(AWeaponPartActor::StaticClass(), nullptr, *("Blueprint'" + AssetData[j].ObjectPath.ToString() + "_C'")));
                }
            }
        }
        AssetData.Empty();
        dataFilter.PackagePaths.Empty();
    }
#pragma  endregion

    //Filling Shotgun part arrays
#pragma region
    WeaponTypePath = "Shotgun/";
    for (int i = 0; i < ShotgunWeaponTypePartPath.Num(); i++)
    {
        FString PartPath = TypesOfWeaponPath + WeaponTypePath + ShotgunWeaponTypePartPath[i];
        dataFilter.PackagePaths.Add(FName(*PartPath));
        AssetRegistryModule.Get().GetAssets(dataFilter, AssetData);
        if (AssetData.Num() > 0)
        {
            if (ShotgunWeaponTypePartPath[i] == "Barrel")
            {
                for (int j = 0; j < AssetData.Num(); j++)
                {
                    m_shotgunBarrelParts.Add(StaticLoadClass(AWeaponPartActor::StaticClass(), nullptr, *("Blueprint'" + AssetData[j].ObjectPath.ToString() + "_C'")));
                }
            }
            if (ShotgunWeaponTypePartPath[i] == "Magazine")
            {
                for (int j = 0; j < AssetData.Num(); j++)
                {
                    m_shotgunTubeParts.Add(StaticLoadClass(AWeaponPartActor::StaticClass(), nullptr, *("Blueprint'" + AssetData[j].ObjectPath.ToString() + "_C'")));
                }
            }
            if (ShotgunWeaponTypePartPath[i] == "Pump")
            {
                for (int j = 0; j < AssetData.Num(); j++)
                {
                    m_shotgunPumpParts.Add(StaticLoadClass(AWeaponPartActor::StaticClass(), nullptr, *("Blueprint'" + AssetData[j].ObjectPath.ToString() + "_C'")));
                }
            }
            if (ShotgunWeaponTypePartPath[i] == "Receiver")
            {
                for (int j = 0; j < AssetData.Num(); j++)
                {
                    m_shotgunReceiverParts.Add(StaticLoadClass(AWeaponPartActor::StaticClass(), nullptr, *("Blueprint'" + AssetData[j].ObjectPath.ToString() + "_C'")));
                }
            }
            if (ShotgunWeaponTypePartPath[i] == "Stock")
            {
                for (int j = 0; j < AssetData.Num(); j++)
                {
                    m_shotgunStockParts.Add(StaticLoadClass(AWeaponPartActor::StaticClass(), nullptr, *("Blueprint'" + AssetData[j].ObjectPath.ToString() + "_C'")));
                }
            }
        }
        AssetData.Empty();
        dataFilter.PackagePaths.Empty();
    }
#pragma  endregion

    //Filling Pistol part arrays
#pragma region
    WeaponTypePath = "Pistol/";
    for (int i = 0; i < PistolWeaponTypePartPath.Num(); i++)
    {
        FString PartPath = TypesOfWeaponPath + WeaponTypePath + PistolWeaponTypePartPath[i];
        //FString PartPath = "/Game/Art/3D_Models/Non_Organic/Guns/Rifle/Barrel";
        dataFilter.PackagePaths.Add(FName(*PartPath));
        AssetRegistryModule.Get().GetAssets(dataFilter, AssetData);
        if (AssetData.Num() > 0)
        {
            if (PistolWeaponTypePartPath[i] == "Barrel")
            {
                for (int j = 0; j < AssetData.Num(); j++)
                {
                    //Initial attempt to load object from FObjectFinder but StaticLoadObject seems to work, not sure of difference but will look into later
                    m_pistolBarrelParts.Add(StaticLoadClass(AWeaponPartActor::StaticClass(), nullptr, *("Blueprint'" + AssetData[j].ObjectPath.ToString() + "_C'")));
                }
            }
            if (PistolWeaponTypePartPath[i] == "Frame")
            {
                for (int j = 0; j < AssetData.Num(); j++)
                {
                    m_pistolFrameParts.Add(StaticLoadClass(AWeaponPartActor::StaticClass(), nullptr, *("Blueprint'" + AssetData[j].ObjectPath.ToString() + "_C'")));
                }
            }
            if (PistolWeaponTypePartPath[i] == "Magazine")
            {
                for (int j = 0; j < AssetData.Num(); j++)
                {
                    m_pistolMagazineParts.Add(StaticLoadClass(AWeaponPartActor::StaticClass(), nullptr, *("Blueprint'" + AssetData[j].ObjectPath.ToString() + "_C'")));
                }
            }
            if (PistolWeaponTypePartPath[i] == "Slide")
            {
                for (int j = 0; j < AssetData.Num(); j++)
                {
                    m_pistolSlideParts.Add(StaticLoadClass(AWeaponPartActor::StaticClass(), nullptr, *("Blueprint'" + AssetData[j].ObjectPath.ToString() + "_C'")));
                }
            }
            if (PistolWeaponTypePartPath[i] == "TriggerGuard")
            {
                for (int j = 0; j < AssetData.Num(); j++)
                {
                    m_pistolTriggerGuardParts.Add(StaticLoadClass(AWeaponPartActor::StaticClass(), nullptr, *("Blueprint'" + AssetData[j].ObjectPath.ToString() + "_C'")));
                }
            }
        }
        AssetData.Empty();
        dataFilter.PackagePaths.Empty();
    }

#pragma  endregion


    //Getting Projectiles
    //Manual path due to only having one in game
    //dataFilter.ClassNames.Add("Blueprint");
    //Blueprint'/Game/Blueprints/GameObjects/Projectiles/BP_TestProjectileActor.BP_TestProjectileActor'
    FString ProjectilePath = "/Game/Blueprints/GameObjects/Projectiles";
    dataFilter.PackagePaths.Add(FName(*ProjectilePath));
    AssetRegistryModule.Get().GetAssets(dataFilter, AssetData);
    if (AssetData.Num() > 0)
    {
        m_weaponProjectile = StaticLoadClass(AProjectileActor::StaticClass(), nullptr, *("Blueprint'" + AssetData[0].ObjectPath.ToString() + "_C'"));
    }
    
    AssetData.Empty();
    dataFilter.PackagePaths.Empty();
}



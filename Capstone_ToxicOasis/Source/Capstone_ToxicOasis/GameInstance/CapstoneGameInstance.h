// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "../GameObjects/Weapons/WeaponGeneratorObject.h"
#include "CapstoneGameInstance.generated.h"

/*************************************
Class: CapstoneGameInstance
Author: Abdul Ali
Created: october 2019
Last Modified: 3/Feb/2020
Description: Class in charge of keeping data related to individual game instances, It is bieng used to store data that is relevant to all scenes such as Frindly fire toggle, Networked toggle, Instance Index and more.
References:
Change Log
**********
Date: unknown
Author: Abdul Ali
Verified By: Lucas Felix
Changes: Added data to store Multiplayer data for UI and gameplay purposes between scenes.
Date: 3/Feb/2020
Author: Luis Filipe Moraes
Verified By: Lucas Felix & Abdul Ali
Changes: CLass was modified to hold an InstanceIndex for the purpose of identifying players and their setting between levels.
*************************************/
UCLASS()
class CAPSTONE_TOXICOASIS_API UCapstoneGameInstance : public UGameInstance
{
	GENERATED_BODY()
	
public:
    //Essentials
    UCapstoneGameInstance();
    //Other Methods
        //Accessor for C++ & BP
    UFUNCTION(BlueprintPure, Category = "Persistence", meta = (DisplayName = "Get Weapon Generator", Keywords = "WeaponGenerator"))
        UWeaponGeneratorObject* WeaponGenerator();
    UFUNCTION(BlueprintCallable)
        void SetMutltiplayerActive(bool a_mutltiplayerActive) { m_mutltiplayerActive = a_mutltiplayerActive; }
    UFUNCTION(BlueprintCallable)
        bool GetMutltiplayerActive() { return m_mutltiplayerActive; }
	UFUNCTION()
		virtual void BeginLoadingScreen(const FString& MapName);
	UFUNCTION()
		virtual void EndLoadingScreen(UWorld* InLoadedWorld);
        

#pragma region
    //...
    //int NumOfGeneratorInstances = 0;
    UWeaponGeneratorObject* WeaponGeneratorInstance;
	UFUNCTION(BlueprintCallable)
		void SetIsFriendlyFire(bool a_isFriednlyFire) { m_isFriendlyFire = a_isFriednlyFire; }
	UFUNCTION(BlueprintCallable)
		bool GetIsFriendlyFire() { return m_isFriendlyFire; }
	UFUNCTION(BlueprintCallable)
		int GetInstanceIndex() { return m_instanceIndex; }
	UFUNCTION(BlueprintCallable)
		void SetInstanceIndex(int a_instanceIndex) { m_instanceIndex = a_instanceIndex>0 && a_instanceIndex<5? a_instanceIndex: -1; }
#pragma region
private:

public:
#pragma endregion
	//...

	//OnEvents
//Components
//Variables
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Multiplayer")
		bool m_mutltiplayerActive;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Multiplayer")
		bool m_isFriendlyFire;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Multiplayer")
		int m_instanceIndex;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings")
        int m_sensitivity = 1;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings")
        bool m_invertCamera = false;
    //UPROPERTY(EditAnywhere, BlueprintAssignable, Category = "Brightness")
    //   class APostProcessVolume* m_postProcess;
protected:
    //Essentials
    //Other Methods
        //Used to cleanup at shutdown if necessary
    //virtual void Shutdown() override;
    virtual void Init() override;
    virtual void OnStart() override;
#pragma region
private:
    //...
protected:
#pragma endregion
    //OnEvents
    //Components
    //Variables
};
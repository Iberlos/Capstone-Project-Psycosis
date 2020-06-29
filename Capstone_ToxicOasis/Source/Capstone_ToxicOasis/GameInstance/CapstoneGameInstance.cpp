// Fill out your copyright notice in the Description page of Project Settings.


#include "CapstoneGameInstance.h"
#include "UnrealNetwork.h"
#include "MoviePlayer.h"
#include "LoadingScreenSlateWidget.h"
#include "Components/PostProcessComponent.h"

UCapstoneGameInstance::UCapstoneGameInstance()
{
    //WeaponGenerator();
    //WeaponGeneratorInstance = NewObject<UWeaponGeneratorObject>(this, FName("WeaponGenerator"));
    m_mutltiplayerActive = false;
	m_isFriendlyFire = false;
	m_instanceIndex = -1;
   // m_postProcessComp = CreateDefaultSubobject<UPostProcessComponent>("Brightness");
}

UWeaponGeneratorObject* UCapstoneGameInstance::WeaponGenerator()
{
    //return IsValid(WeaponGeneratorInstance) ? WeaponGeneratorInstance : WeaponGeneratorInstance = NewObject<UWeaponGeneratorObject>(this, FName("WeaponGenerator"));

    if (WeaponGeneratorInstance)
    {
        return WeaponGeneratorInstance;
    }
    else
    {
         return WeaponGeneratorInstance = NewObject<UWeaponGeneratorObject>(this, FName("WeaponGenerator"));

    }
}


void UCapstoneGameInstance::Init()
{
    Super::Init();
    //WeaponGenerator();

	FCoreUObjectDelegates::PreLoadMap.AddUObject(this, &UCapstoneGameInstance::BeginLoadingScreen);
	FCoreUObjectDelegates::PostLoadMapWithWorld.AddUObject(this, &UCapstoneGameInstance::EndLoadingScreen);
}

void UCapstoneGameInstance::OnStart()
{
    Super::OnStart();
    //WeaponGenerator();
}

//Cleanup function if necessary for Instanced objects
//void UCapstoneGameInstance::Shutdown()
//{
//    Super::Shutdown();
//
//    if (IsValid(WeaponGeneratorInstance))
//    {
//
//    }
//}

void UCapstoneGameInstance::BeginLoadingScreen(const FString& InMapName)
{
	if (!IsRunningDedicatedServer())
	{
		FLoadingScreenAttributes LoadingScreen;
		LoadingScreen.bAutoCompleteWhenLoadingCompletes = false;
		LoadingScreen.bMoviesAreSkippable = false;
		LoadingScreen.MinimumLoadingScreenDisplayTime = 5;
		LoadingScreen.MoviePaths.Add(TEXT("Loading"));
		LoadingScreen.WidgetLoadingScreen = SNew(LoadingScreenSlateWidget);

		GetMoviePlayer()->SetupLoadingScreen(LoadingScreen);
	}
}

void UCapstoneGameInstance::EndLoadingScreen(UWorld* InLoadedWorld)
{

}

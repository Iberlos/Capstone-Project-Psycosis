// Fill out your copyright notice in the Description page of Project Settings.


#include "BedInteractable.h"
#include "Engine/Engine.h"
#include "Net/UnrealNetwork.h"
#include "GameFramework/GameModeBase.h"
#include "Components/BoxComponent.h"
#include "Kismet/GameplayStatics.h"
#include "GameInstance/CapstoneGameInstance.h"

ABedInteractable::ABedInteractable()
{
    m_isActiveInteractable_r = true;
    m_colliderComponent->SetSimulatePhysics(false);
	m_levelName = "Level_FloatingIslands_Production";
}
bool ABedInteractable::Interact(AActor* a_actor)
{
    if (Super::Interact(a_actor))//if you can't be highlighted don't call your functions too
    {
        m_isActiveInteractable_r = false;
        UCapstoneGameInstance* GI = Cast<UCapstoneGameInstance>(GetWorld()->GetGameInstance());

        if (GI->GetMutltiplayerActive())
        {
            if (GetLocalRole() == ROLE_Authority)
            {
                ServerTravel();
            }
        }
        else if (!GI->GetMutltiplayerActive())
        {
            ChangeLevel();
        }
        return true;
    }
    else
    {
        return false;
    }
}

void ABedInteractable::ServerTravel()
{
    UWorld* World = GetWorld();
    World->GetAuthGameMode()->bUseSeamlessTravel = true;
    World->ServerTravel("/Game/Levels/" + m_levelName.ToString() + "?listen");
}

void ABedInteractable::ChangeLevel()
{
    UGameplayStatics::OpenLevel(GetWorld(), m_levelName);
}

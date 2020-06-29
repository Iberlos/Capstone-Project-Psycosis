// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameObjects/Interactables/InteractableActor.h"
#include "BedInteractable.generated.h"

/**
 * 
 */
UCLASS()
class CAPSTONE_TOXICOASIS_API ABedInteractable : public AInteractableActor
{
	GENERATED_BODY()
public:
    ABedInteractable();

    virtual bool Interact(AActor* a_actor) override;

    void ServerTravel();
    void ChangeLevel();

private:
	UPROPERTY(EditAnywhere)
	FName m_levelName;
};

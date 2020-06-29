// Fill out your copyright notice in the Description page of Project Settings.

//Default Unreal Includes
#pragma once

#include "CoreMinimal.h"
#include "GameObjects/Interactables/InteractableActor.h"
//Custom Unreal includes

//Custom project includes

//Generated include
#include "PortalInteractable.generated.h"

/**
 * This class is meant to revceive the events from the artifacts and set itself active when they are all picked up. The players will have to interact with it for it to turn on so they can proceed to another level.
 */
UCLASS()
class CAPSTONE_TOXICOASIS_API APortalInteractable : public AInteractableActor
{
	GENERATED_BODY()
public:
	//Essentials
	APortalInteractable();
	void Tick(float DeltaTime) override;
	//Other Methods
	virtual bool Interact(AActor* a_actor) override;
	void GetAllArtifactsAndRegister();
	void ServerTravel();
	void ChangeLevel();
#pragma region
private:
	//...
public:
#pragma endregion
	//OnEvents
	UFUNCTION()
	void OnArtifactReportInteracted();
	UFUNCTION()
		virtual void OnOverlap
		(
			UPrimitiveComponent* OverlappedComp,
			AActor* OtherActor,
			UPrimitiveComponent* OtherComp,
			int32 OtherBodyIndex,
			bool bFromSweep,
			const FHitResult& SweepResult
		);
	//Components
	//Variables
	//Getters and Setters

protected:
	//Essentials
	void BeginPlay() override;
	//Other Methods
#pragma region
private:
	//...
protected:
#pragma endregion
	//OnEvents
	//Components
	UPROPERTY(EditDefaultsOnly, Category = "Dev. Data|Portal Data|Components")
	UBoxComponent* m_portalTriggerBox;
	//Variables
	UPROPERTY(Replicated)
	int m_artifactsInLevel_r;
	UPROPERTY(Replicated)
	int m_artifactsPickedUp_r;
	UPROPERTY(EditAnywhere, Category = "Dev. Data|Portal Data|Variables")
		FName m_levelName;

};

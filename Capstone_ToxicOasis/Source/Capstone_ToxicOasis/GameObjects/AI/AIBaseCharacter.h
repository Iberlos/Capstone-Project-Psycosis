// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameObjects/Players/MyCharacter.h"
#include "EnumMaster/EnumMaster.h"
#include "AIBaseCharacter.generated.h"

/*************************************
Class: AIBird
Author: Erin
Created: october 2019
Last Modified: 3/mar/2020
Description: This clas defines the bird enemy AI
References:
Change Log
**********
Date: unknown
Author: Erin and abdul
Verified By: Lucas Felix & Abdul Ali
Changes: Added all the functionlaity related to the base fucntionality of the class.
--------------------------------------------------------
Date: 2/mar/2020
Author: Luis Filipe Moraes
Verified By: Lucas Felix & Abdul Ali
Changes: Modified the Die function override and added a FinalizeDie function to allow for waiting the animation using notifies.
--------------------------------------------------------
Date: 6/mar/2020
Author: Luis Filipe Moraes
Verified By: Lucas Felix & Abdul Ali
Changes: Modified the Primary attack function in the base class to play the aniations and sounds and particles. Modified the OnSensePawn function to play the Spot sounds, particles and animations
--------------------------------------------------------
*************************************/
UCLASS()
class CAPSTONE_TOXICOASIS_API AAIBaseCharacter : public AMyCharacter
{
	GENERATED_BODY()

public:
	AAIBaseCharacter();

	virtual void Tick(float DeltaTime) override;

	UFUNCTION()
		void OnSeePawn(APawn* Pawn);

    // Check if this AI Character is alive
	UFUNCTION(BlueprintCallable)
    bool IsAlive();

	void Reset();
#pragma region
private:
	UFUNCTION(Server, Reliable, WithValidation)
		void SERVER_Reset();
	bool SERVER_Reset_Validate() { return true; }

    void Die() override;
	void FinalizeDie() override;

public:
#pragma endregion

    // Setters
    void SetAIState(EAIStates a_aiState); 
    void SetPrimaryAttackRange(float a_pAttackRange);
    void SetSecondaryAttackRange(float a_sAttackRange);
    void SetOwningSpawner(class AAISpawner* a_OwningSpawner) { m_owningAISpawner = a_OwningSpawner; }
    void SetDistToPlayer();
	void SetAttackingPlayer(class APlayerCharacter* a_attackingPlayer) { m_attackingPlayer_r = a_attackingPlayer; }

    // Getters
    class UBehaviorTree* GetBehaviorTree() { return m_behaviorTree; }
    EAIStates GetAIState() { return m_aiState; }
    UFUNCTION(BlueprintCallable)
    AAISpawner* GetOwningSpawner() { return m_owningAISpawner; }
    bool GetTargetedPlayerIsWithinPrimaryAttackRange();
    // Tasks
	virtual void Initialize();
    virtual void MoveToPlayer() { ; }
	virtual void PrimaryAttack();
    virtual void SecondaryAttack() { ; }
    void SetAttackRange();

	bool GetIsActive() { return m_isActive; }
    bool GetIsFinishedSpawning() { return m_finishedSpawning; }
	void SetIsActive(bool isActive) { m_isActive = isActive; }
    void SetIsFinishedSpawning(bool finishedSpawning) { m_finishedSpawning = finishedSpawning; }
	void StartBehaviorTree();
	void StopBehaviorTree();

	void SetIsInPool(bool a_isInPool) { m_isInPool = a_isInPool; }

protected:
	virtual void BeginPlay() override;

    void PlayChasePlayerFeedback() { PlaySound("Chase", true); PlayParticle("Chase", true); PlayAnimationMontage("Chase");}

	// Attack
	UPROPERTY(EditDefaultsOnly, Category = "AI")
		float m_pAttackRange;
	UPROPERTY(EditDefaultsOnly, Category = "AI")
		float m_sAttackRange;
	UPROPERTY(EditDefaultsOnly, Category = "AI")
		float m_pAttackStrength;
	UPROPERTY(EditDefaultsOnly, Category = "AI")
		float m_sAttackStrength;
	// Pawn Sensing
	UPROPERTY(EditDefaultsOnly, Category = "AI")
		float m_senseTimeLimit;
	UPROPERTY(EditDefaultsOnly, Category = "AI" /*Replicated*/)
		class UPawnSensingComponent* m_pawnSensingComp;
	// Health
	UPROPERTY(EditDefaultsOnly, Category = "AI")
		float m_health;
	// Behaviour
	UPROPERTY(EditAnywhere, Category = "AI")
		EAIStates m_aiState;
	UPROPERTY(EditDefaultsOnly, Category = "AI")
		class UBehaviorTree* m_behaviorTree;
	UPROPERTY(EditDefaultsOnly, Category = "AI" /*Replicated*/)
		class APlayerCharacter* m_currentTarget_r;
	UPROPERTY(EditDefaultsOnly, Category = "AI" /*Replicated*/)
		class APlayerCharacter* m_attackingPlayer_r;

	//Drop
	UPROPERTY(EditDefaultsOnly, Category = "AI|Drop")
		class UItemGeneratorComponent* m_itemGeneratorComponent;

	AAISpawner* m_owningAISpawner;
	float m_LastTimePlayerSeen;
	float m_distanceToPlayer;
	bool m_targetIsSensed;
	bool m_isActive;
	bool m_isInPool;
    bool m_finishedSpawning;
	bool m_TargetedPlayerIsWithinPrimaryAttackRange;
	class AAICharacterController* m_AIController;
	FTimerHandle m_resetTimer;
};

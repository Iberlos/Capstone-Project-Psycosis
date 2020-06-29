// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "EnumMaster/EnumMaster.h"
#include "AICharacterController.generated.h"

/**
 *
 */
UCLASS()
class CAPSTONE_TOXICOASIS_API AAICharacterController : public AAIController
{
	GENERATED_BODY()
public:
	AAICharacterController(const class FObjectInitializer& ObjectInitializer);

	virtual void OnPossess(class APawn* InPawn) override;

	virtual void OnUnPossess() override;

	void SetTarget(APawn* NewTarget);
	void SetPrimaryAttackRange(float a_pAttackRange);
	void SetSecondaryAttackRange(float a_sAttackRange);
	void SetDistToPlayer(float a_distToPlayer);
	void SetTargetedPlayerIsWithinPrimaryAttackRange(bool a_TargetedPlayerIsWithinPrimaryAttackRange);
	class APlayerCharacter* GetTarget();

	void SetAIState(EAIStates a_aiState);

	void CallFuncOnAICharacter(EAITaskFunctions AIFunc);

	/*Start Tree when spawner moves the enemy and Stop when it's sent back to the pool*/
	void StartBehaviorTree();
	void StopBehaviorTree();

protected:
	UPROPERTY(EditDefaultsOnly, Category = "AI")
		FName m_aiStateKeyName;
	UPROPERTY(EditDefaultsOnly, Category = "AI")
		FName m_targetPlayerKeyName;
	UPROPERTY(EditDefaultsOnly, Category = "AI")
		FName m_pAttackRangeKeyName;
	UPROPERTY(EditDefaultsOnly, Category = "AI")
		FName m_sAttackRangeKeyName;
	UPROPERTY(EditDefaultsOnly, Category = "AI")
		FName m_distToPlayerKeyName;
	UPROPERTY(EditDefaultsOnly, Category = "AI")
		FName m_TargetedPlayerIsWithinPrimaryAttackRangeKeyName;

	class AAIBaseCharacter* m_aiCharacter;
	class UBehaviorTreeComponent* m_behaviorComp;
	class UBlackboardComponent* m_blackboardComp;
};

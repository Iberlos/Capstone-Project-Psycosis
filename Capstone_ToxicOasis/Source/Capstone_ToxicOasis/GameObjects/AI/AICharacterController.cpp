// Fill out your copyright notice in the Description page of Project Settings.


#include "AICharacterController.h"
#include "GameObjects/Players/PlayerCharacter.h"
#include "AIBaseCharacter.h"

#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "BehaviorTree/BlackboardComponent.h"

#include "Navigation/CrowdFollowingComponent.h"

AAICharacterController::AAICharacterController(const class FObjectInitializer& ObjectInitializer)
	:Super(ObjectInitializer.SetDefaultSubobjectClass<UCrowdFollowingComponent>(TEXT("PathFollowingComponent")))
{
	m_behaviorComp = CreateDefaultSubobject<UBehaviorTreeComponent>("BehaviorComp");
	m_blackboardComp = CreateDefaultSubobject<UBlackboardComponent>("BlackboardComp");

	m_aiStateKeyName = "AIState";
	m_targetPlayerKeyName = "TargetedPlayer";
	m_pAttackRangeKeyName = "PrimaryAttack";
	m_sAttackRangeKeyName = "SecondaryAttack";
	m_distToPlayerKeyName = "DistanceToPlayer";
	m_TargetedPlayerIsWithinPrimaryAttackRangeKeyName = "TargetedPlayerIsWithinPrimaryAttackRange";
}
void AAICharacterController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	m_aiCharacter = Cast<AAIBaseCharacter>(InPawn);
	if (m_aiCharacter)
	{
		if (m_aiCharacter->GetBehaviorTree()->BlackboardAsset)
		{
			m_blackboardComp->InitializeBlackboard(*m_aiCharacter->GetBehaviorTree()->BlackboardAsset);
			SetAIState(m_aiCharacter->GetAIState());
		}

		/*  if(m_aiCharacter->GetIsActive())
			  m_behaviorComp->StartTree(*m_aiCharacter->GetBehaviorTree());*/
	}
}

void AAICharacterController::OnUnPossess()
{
	Super::OnUnPossess();

	m_behaviorComp->StopTree();
}

void AAICharacterController::SetTarget(APawn* NewTarget)
{
	if (m_blackboardComp)
		m_blackboardComp->SetValueAsObject(m_targetPlayerKeyName, NewTarget);
}

void AAICharacterController::SetPrimaryAttackRange(float a_pAttackRange)
{
	if (m_blackboardComp)
		m_blackboardComp->SetValueAsFloat(m_pAttackRangeKeyName, a_pAttackRange);
}

void AAICharacterController::SetSecondaryAttackRange(float a_sAttackRange)
{
	if (m_blackboardComp)
		m_blackboardComp->SetValueAsFloat(m_sAttackRangeKeyName, a_sAttackRange);
}

void AAICharacterController::SetDistToPlayer(float a_distToPlayer)
{
	if (m_blackboardComp)
		m_blackboardComp->SetValueAsFloat(m_distToPlayerKeyName, a_distToPlayer);
}

void AAICharacterController::SetTargetedPlayerIsWithinPrimaryAttackRange(bool a_TargetedPlayerIsWithinPrimaryAttackRangeKeyName)
{
	if (m_blackboardComp)
		m_blackboardComp->SetValueAsBool(m_TargetedPlayerIsWithinPrimaryAttackRangeKeyName, a_TargetedPlayerIsWithinPrimaryAttackRangeKeyName);
}

APlayerCharacter* AAICharacterController::GetTarget()
{
	if (m_blackboardComp)
		return Cast<APlayerCharacter>(m_blackboardComp->GetValueAsObject(m_targetPlayerKeyName));
	return nullptr;
}

void AAICharacterController::SetAIState(EAIStates a_aiState)
{
	if (m_blackboardComp)
		m_blackboardComp->SetValueAsEnum(m_aiStateKeyName, (uint8)a_aiState);
}

void AAICharacterController::CallFuncOnAICharacter(EAITaskFunctions AIFunc)
{
	if (m_aiCharacter)
	{
		switch (AIFunc)
		{
		case EAITaskFunctions::AIFUNC_PrimaryATK:
			m_aiCharacter->PrimaryAttack();
			break;
		case EAITaskFunctions::AIFUNC_SecondaryATK:
			m_aiCharacter->SecondaryAttack();
			break;
		case EAITaskFunctions::AIFUNC_MoveToPlayer:
			m_aiCharacter->MoveToPlayer();
			break;
		default:
			break;
		}
	}
}

void AAICharacterController::StartBehaviorTree()
{
	PrimaryActorTick.bCanEverTick = true;
	if (m_aiCharacter->GetIsActive())
		m_behaviorComp->StartTree(*m_aiCharacter->GetBehaviorTree());
}

void AAICharacterController::StopBehaviorTree()
{
	PrimaryActorTick.bCanEverTick = false;
	if (m_aiCharacter->GetIsActive())
		m_behaviorComp->StopTree();
}

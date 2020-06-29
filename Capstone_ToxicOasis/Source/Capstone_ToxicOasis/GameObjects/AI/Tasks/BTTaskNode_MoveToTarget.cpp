// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTaskNode_MoveToTarget.h"
#include "GameObjects/AI/AICharacterController.h"
#include "EnumMaster/EnumMaster.h"

#include "BehaviorTree/BehaviorTreeComponent.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/Blackboard/BlackboardKeyAllTypes.h"
#include "NavigationSystem.h"

EBTNodeResult::Type UBTTaskNode_MoveToTarget::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
    AAICharacterController* controller = Cast<AAICharacterController>(OwnerComp.GetAIOwner());
    if (!controller)
        return EBTNodeResult::Failed;
    controller->CallFuncOnAICharacter(EAITaskFunctions::AIFUNC_MoveToPlayer);
    return EBTNodeResult::Succeeded;
}

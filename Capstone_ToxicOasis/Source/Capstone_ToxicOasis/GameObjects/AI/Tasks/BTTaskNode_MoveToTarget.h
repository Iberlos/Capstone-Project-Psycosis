// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTaskNode_MoveToTarget.generated.h"

/**
 * 
 */
UCLASS()
class CAPSTONE_TOXICOASIS_API UBTTaskNode_MoveToTarget : public UBTTaskNode
{
	GENERATED_BODY()
	
        virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
};
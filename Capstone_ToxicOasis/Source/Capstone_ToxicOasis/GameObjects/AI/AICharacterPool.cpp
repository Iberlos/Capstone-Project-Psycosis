// Fill out your copyright notice in the Description page of Project Settings.

#include "AICharacterPool.h"
#include "AISpawner.h"
#include "AIBaseCharacter.h"
#include "AIBee.h"
#include "AIHookHand.h"
#include "AIBird.h"
#include "AIChonk.h"
#include "GameMode/MainGameMode.h"

#include "Engine/World.h"
#include "EngineUtils.h"
#include "TimerManager.h"
#include "Kismet/GameplayStatics.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/PawnMovementComponent.h"
#include "GameFramework/CharacterMovementComponent.h"

// Sets default values
AAICharacterPool::AAICharacterPool()
{
    // Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
    PrimaryActorTick.bCanEverTick = true;
    m_enemyPool.Init(nullptr, 0);
    m_poolHasInitialized = false;
    m_hookHandCount = 0;
    m_beeCount = 0;
    m_hookTimer = true;
    m_beeTimer = true;

    m_enemyMultiplier = 1;
}

// Called when the game starts or when spawned
void AAICharacterPool::BeginPlay()
{
    Super::BeginPlay();
    /*Fill the pool a second after the game has started*/
  //  FTimerHandle spawnTimer;
   // GetWorldTimerManager().SetTimer(spawnTimer, this, &AAICharacterPool::InitializePool, 1.0f, false);
     // Multiplier applied to the enemy count
    //int enemyMultiplier = 1;
    // If our current game mode is AMainGameMode
    if (AMainGameMode* gameMode = Cast<AMainGameMode>(UGameplayStatics::GetGameMode(this)))
    {
        // Multiply enemy count by amount of players
        m_enemyMultiplier += gameMode->GetPlayerList().Num();
    }
    if (GetLocalRole() == ROLE_Authority)
    {
        InitializePool();
    }
}

// Called every frame
void AAICharacterPool::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

}

void AAICharacterPool::AddBackToPool(AAIBaseCharacter* a_charToAdd)
{
    /*Add the enemy back to the pool and move it back to the pool's location*/
    m_enemyPool.Add(a_charToAdd);
    a_charToAdd->SetActorLocation(GetActorLocation());
}

void AAICharacterPool::RemoveFromPool(AAIBaseCharacter* a_charToRemove)
{
    m_enemyPool.Remove(a_charToRemove);
}

void AAICharacterPool::InitializePool()
{
    //SERVER_InitializePool();
    if (m_poolHasInitialized == false)
    {
        for (FEnemyTypes enemyType : m_enemyTypes)
        {
            /*Based on the enemtype and spawn accordingly*/
            switch (enemyType.m_aiType)
            {
            case(EAITypes::AI_Hook):
                GetWorldTimerManager().SetTimer(hookSpawnTimer, this, &AAICharacterPool::SpawnHookHand, 1.5f, true);
                break;

            case(EAITypes::AI_Bee):
                GetWorldTimerManager().SetTimer(beeSpawnTimer, this, &AAICharacterPool::SpawnBee, 1.5f, true);
                break;
            }
        }
        m_poolHasInitialized = true;
    }
}

void AAICharacterPool::SERVER_InitializePool_Implementation()
{
    //  FTimerHandle hookSpawnTimer;
    //if (m_poolHasInitialized == false)
    //{
    //    //// Multiplier applied to the enemy count
    //    //int enemyMultiplier = 1;
    //    //// If our current game mode is AMainGameMode
    //    //if (AMainGameMode * gameMode = Cast<AMainGameMode>(UGameplayStatics::GetGameMode(this)))
    //    //{
    //    //    // Multiply enemy count by amount of players
    //    //    enemyMultiplier += gameMode->GetPlayerList().Num();
    //    //}

    //    /*Loop through the tsubclass array and spawn an enemy of each type depending on the number specified in the blueprints*/
    //    for (FEnemyTypes enemyType : m_enemyTypes)
    //    {
    //        /*Based on the enemtype and spawn accordingly*/
    //        switch (enemyType.m_aiType)
    //        {
    //        case(EAITypes::AI_Hook):
    //            GetWorldTimerManager().SetTimer(hookSpawnTimer, this, &AAICharacterPool::SpawnHookHand, 1.5f, true);
    //            break;

    //        case(EAITypes::AI_Bee):
    //            GetWorldTimerManager().SetTimer(beeSpawnTimer, this, &AAICharacterPool::SpawnBee, 1.5f, true);
    //            break;
    //        }
    //    }
    //    m_poolHasInitialized = true;
    //}
}

void AAICharacterPool::SpawnHookHand()
{
    //SERVER_SpawnHookHand();
    auto spawnedAI = GetWorld()->SpawnActorDeferred<AAIHookHand>(m_enemyTypes[0].m_enemyType, GetTransform(), this, nullptr, ESpawnActorCollisionHandlingMethod::AlwaysSpawn);
    spawnedAI->GetCapsuleComponent()->SetCollisionProfileName("OverlapAll");
    spawnedAI->SetActorTickEnabled(false);
    spawnedAI->SetIsInPool(true);
    Cast<UCharacterMovementComponent>(spawnedAI->GetMovementComponent())->GravityScale = 0.0f;
    m_enemyPool.Add(spawnedAI);
    m_hookHandCount++;

    if (m_hookHandCount > m_enemyTypes[0].m_enemyNum* m_enemyMultiplier)
    {
        GetWorldTimerManager().ClearTimer(hookSpawnTimer);
    }
}

void AAICharacterPool::SERVER_SpawnHookHand_Implementation()
{
   /* auto spawnedAI = GetWorld()->SpawnActorDeferred<AAIHookHand>(m_enemyTypes[0].m_enemyType, GetTransform(), this, nullptr, ESpawnActorCollisionHandlingMethod::AlwaysSpawn);
    spawnedAI->GetCapsuleComponent()->SetCollisionProfileName("OverlapAll");
    spawnedAI->SetActorTickEnabled(false);
    spawnedAI->SetIsInPool(true);
    Cast<UCharacterMovementComponent>(spawnedAI->GetMovementComponent())->GravityScale = 0.0f;
    m_enemyPool.Add(spawnedAI);
    m_hookHandCount++;

    if (m_hookHandCount > m_enemyTypes[0].m_enemyNum* m_enemyMultiplier)
    {
        GetWorldTimerManager().ClearTimer(hookSpawnTimer);
    }*/
}

void AAICharacterPool::SpawnBee()
{
    //SERVER_SpawnBee();
    auto spawnedAI = GetWorld()->SpawnActorDeferred<AAIBee>(m_enemyTypes[1].m_enemyType, GetTransform(), this, nullptr, ESpawnActorCollisionHandlingMethod::AlwaysSpawn);
    spawnedAI->GetCapsuleComponent()->SetCollisionProfileName("OverlapAll");
    Cast<UCharacterMovementComponent>(spawnedAI->GetMovementComponent())->GravityScale = 0.0f;
    spawnedAI->SetActorTickEnabled(false);
    spawnedAI->SetIsInPool(true);
    m_enemyPool.Add(spawnedAI);
    m_beeCount++;
    if (m_beeCount > m_enemyTypes[1].m_enemyNum* m_enemyMultiplier)
    {
        GetWorldTimerManager().ClearTimer(beeSpawnTimer);
    }
}

void AAICharacterPool::SERVER_SpawnBee_Implementation()
{
 /* auto spawnedAI = GetWorld()->SpawnActorDeferred<AAIBee>(m_enemyTypes[1].m_enemyType, GetTransform(), this, nullptr, ESpawnActorCollisionHandlingMethod::AlwaysSpawn);
    spawnedAI->GetCapsuleComponent()->SetCollisionProfileName("OverlapAll");
    Cast<UCharacterMovementComponent>(spawnedAI->GetMovementComponent())->GravityScale = 0.0f;
    spawnedAI->SetActorTickEnabled(false);
    spawnedAI->SetIsInPool(true);
    m_enemyPool.Add(spawnedAI);
    m_beeCount++;
    if (m_beeCount > m_enemyTypes[1].m_enemyNum* m_enemyMultiplier)
    {
        GetWorldTimerManager().ClearTimer(beeSpawnTimer);
    }*/
}

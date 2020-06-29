// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "EnumMaster/EnumMaster.h"
#include "AICharacterPool.generated.h"

USTRUCT(BlueprintType)
struct FEnemyTypes
{
    GENERATED_BODY()
public:
    UPROPERTY(EditDefaultsOnly)
        TSubclassOf<class AActor> m_enemyType;
    UPROPERTY(EditDefaultsOnly)
        int m_enemyNum;
    UPROPERTY(EditDefaultsOnly)
        EAITypes m_aiType;
};

UCLASS()
class CAPSTONE_TOXICOASIS_API AAICharacterPool : public AActor
{
    GENERATED_BODY()

public:
    // Sets default values for this actor's properties
    AAICharacterPool();
    // Called every frame
    virtual void Tick(float DeltaTime) override;

    TArray<class AAIBaseCharacter*> GetEnemyPool() { return m_enemyPool; }
    bool GetPoolHasInitialized() { return m_poolHasInitialized; }
    void AddBackToPool(AAIBaseCharacter* a_charToAdd);
    void RemoveFromPool(AAIBaseCharacter* a_charToRemove);

    void InitializePool();
#pragma region
private:
    UFUNCTION(Server, Reliable, WithValidation)
        void SERVER_InitializePool();
    bool SERVER_InitializePool_Validate() { return true; }
public:
#pragma endregion
    void SpawnHookHand();
#pragma region
private:
    UFUNCTION(Server, Reliable, WithValidation)
        void SERVER_SpawnHookHand();
    bool SERVER_SpawnHookHand_Validate() { return true; }
public:
#pragma endregion
    void SpawnBee();
#pragma region
private:
    UFUNCTION(Server, Reliable, WithValidation)
        void SERVER_SpawnBee();
    bool SERVER_SpawnBee_Validate() { return true; }
public:
#pragma endregion
protected:
    // Called when the game starts or when spawned
    virtual void BeginPlay() override;

    /*Holds the types of enemies that can spawn*/
    UPROPERTY(EditDefaultsOnly, Category = "AI")
        TArray<FEnemyTypes> m_enemyTypes;
    /*The pool of enemies that will be sent to the spawner*/
    TArray<AAIBaseCharacter*> m_enemyPool;

    bool m_poolHasInitialized;

    int m_hookHandCount;
    int m_beeCount;
    int m_enemyMultiplier;
    bool m_hookTimer;
    bool m_beeTimer;

    FTimerHandle hookSpawnTimer;
    FTimerHandle beeSpawnTimer;
};

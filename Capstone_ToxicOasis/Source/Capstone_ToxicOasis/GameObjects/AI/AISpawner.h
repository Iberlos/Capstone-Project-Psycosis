// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "AISpawner.generated.h"

UCLASS()
class CAPSTONE_TOXICOASIS_API AAISpawner : public AActor
{
    GENERATED_BODY()

public:
    // Sets default values for this actor's properties
    AAISpawner();
    // Called every frame
    virtual void Tick(float DeltaTime) override;

    void SpawnCharacter();

    void InitializeList();

    UFUNCTION(BlueprintCallable)
        void SendBackToPool(class AAIBaseCharacter* a_charToAdd);

    UFUNCTION(BlueprintCallable)
        bool GetShouldActivate() { return m_shouldActivate; }
    void SetShouldActivate(bool a_shouldActivate) { m_shouldActivate = a_shouldActivate; }

    UFUNCTION()
        void OnOverlapBegin(UPrimitiveComponent* OverlappedComp,
            AActor* OtherActor,
            UPrimitiveComponent* OtherComp,
            int32 OtherBodyIndex,
            bool bFromSweep,
            const FHitResult& SweepResult);

    UFUNCTION()
        void OnOverlapEnd(class UPrimitiveComponent* OverlappedComp,
            class AActor* OtherActor,
            class UPrimitiveComponent* OtherComp,
            int32 OtherBodyIndex);

    UFUNCTION(BlueprintCallable)
        bool GetPoolIsFull() { return false; }

    void SpawnArtifact();
#pragma region
private:
    UFUNCTION(Server, Reliable, WithValidation)
        void SERVER_SpawnArtifact();
    bool SERVER_SpawnArtifact_Validate() { return true; }
public:

    void HideMesh(bool a_hide, BlockerType type);
#pragma region
private:
    UFUNCTION(NetMulticast, Reliable)
        void Net_HideMesh(bool a_hide, BlockerType type);
public:

    void ToggleCollision(bool collisionEnabled, BlockerType type);
#pragma region
private:
    UFUNCTION(NetMulticast, Reliable)
        void Net_ToggleCollision(bool collisionEnabled, BlockerType type);
public:
#pragma endregion

    void RandTime();
    void SetSpawnPointPos();
    int GetRemainingEnemies() { return m_remainingEnemies; }
    bool GetPlayerIsInArena() { return m_playerIsArena; }
    bool GetSpawnerHasBeenCleared() { return m_spawnerHasBeenCleared; }
    bool GetSpawnerHasStarted() { return m_spawnerHasStarted; }
    class AArtifactInteractable* GetSpawnedArtifacted() { return m_spawnedArtifacted_r; }
    FVector GetRandLocation(FVector SpawnPointLocation);
    FVector GetRandSpawnPoint();
protected:
    // Called when the game starts or when spawned
    virtual void BeginPlay() override;

    UPROPERTY(EditDefaultsOnly)
        class USphereComponent* m_activateCheckBox;
    UPROPERTY(EditDefaultsOnly, Category = "AI Spawning")
        float m_minSpawnTime;
    UPROPERTY(EditDefaultsOnly, Category = "AI Spawning")
        float m_maxSpawnTime;
    UPROPERTY(EditDefaultsOnly, Category = "AI Spawning")
        float m_maxChunky;
    UPROPERTY(EditDefaultsOnly, Category = "AI Spawning")
        float m_maxBee;
    UPROPERTY(EditDefaultsOnly, Category = "AI Spawning")
        float m_maxHook;
    UPROPERTY(EditDefaultsOnly, Category = "AI Spawning")
        TSubclassOf<class AActor> m_artifacts;
    UPROPERTY(Replicated)
        class AArtifactInteractable* m_spawnedArtifacted_r;
    UPROPERTY(EditDefaultsOnly, Replicated, Category = "Blocker")
        class UStaticMeshComponent* m_staticMeshCompEntrance_r;
    UPROPERTY(EditDefaultsOnly, Replicated, Category = "Blocker")
        class UStaticMeshComponent* m_staticMeshCompExit_r;

    TArray<class AAIBaseCharacter*> m_enemySpawnList;
    TArray<FVector> m_spawnPointsPos;
    FTimerHandle m_spawnTimer;
    float m_randSpawnTime;
    bool m_shouldActivate;
    bool m_playerIsArena;
    bool m_spawnerHasInitialized;
    bool m_spawnerHasBeenCleared;
    bool m_artifactHasSpawned;
    int m_defeatedEnemyCounter;
    int m_remainingEnemies;
    bool m_spawnerHasStarted;
    bool trigger;
public:
    UPROPERTY(EditInstanceOnly, AdvancedDisplay, Category = "AI Spawning")
        TArray<class AActor*> m_spawnPoints;
    UPROPERTY(EditInstanceOnly, AdvancedDisplay, Category = "AI Spawning")
        class AAICharacterPool* m_enemyPool;

};

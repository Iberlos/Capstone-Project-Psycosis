// Fill out your copyright notice in the Description page of Project Settings.

//Header include
#include "AISpawner.h"

//Custom Engine Includes
#include "GameMode/MainGameMode.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "TimerManager.h"
#include "Engine/World.h"
#include "EngineUtils.h"
#include "Kismet/GameplayStatics.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/PawnMovementComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "Net/UnrealNetwork.h"

//Custom Project Includes
#include "AICharacterPool.h"
#include "AIBaseCharacter.h"
#include "AIBee.h"
#include "AIHookHand.h"
#include "AIBird.h"
#include "AIChonk.h"
#include "Blocker.h"
#include "GameObjects/Players/PlayerCharacter.h"
#include "GameObjects/Interactables/ArtifactInteractable.h"


// Sets default values
AAISpawner::AAISpawner()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	m_activateCheckBox = CreateDefaultSubobject<USphereComponent>(FName("ActivateCheckBox"));
	m_activateCheckBox->OnComponentBeginOverlap.AddDynamic(this, &AAISpawner::OnOverlapBegin);
	m_activateCheckBox->OnComponentEndOverlap.AddDynamic(this, &AAISpawner::OnOverlapEnd);
	SetRootComponent(m_activateCheckBox);

	m_staticMeshCompEntrance_r = CreateDefaultSubobject<UStaticMeshComponent>("EntranceBlockerMesh");
	//m_staticMeshCompEntrance_r->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	m_staticMeshCompEntrance_r->SetCollisionProfileName("NoCollision");
	m_staticMeshCompEntrance_r->SetupAttachment(RootComponent);

	m_staticMeshCompExit_r = CreateDefaultSubobject<UStaticMeshComponent>("ExitBlockerMesh");
	//m_staticMeshCompExit_r->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	m_staticMeshCompExit_r->SetCollisionProfileName("NoCollision");
	m_staticMeshCompExit_r->SetupAttachment(RootComponent);

	m_enemySpawnList.Init(nullptr, 0);
	m_spawnerHasInitialized = false;
	m_spawnerHasBeenCleared = false;
	m_playerIsArena = false;
	m_remainingEnemies = 1;
	m_artifactHasSpawned = false;
	m_spawnedArtifacted_r = nullptr;
	m_spawnerHasStarted = false;
	trigger = false;
}

// Called when the game starts or when spawned
void AAISpawner::BeginPlay()
{
	Super::BeginPlay();

	if (GetLocalRole() == ROLE_Authority)
	{
		SetSpawnPointPos();
		RandTime();
		GetWorldTimerManager().SetTimer(m_spawnTimer, this, &AAISpawner::SpawnCharacter, m_randSpawnTime, true);
		// Turn Off Entrance
		HideMesh(true, BlockerType::BT_Entrance);
		ToggleCollision(false, BlockerType::BT_Entrance);
		// Turn Off Exit
		HideMesh(true, BlockerType::BT_Exit);
		ToggleCollision(false, BlockerType::BT_Exit);
	}
	// Spawn Artifact 
	SpawnArtifact();
	// Multiplier applied to the enemy count
	//int enemyMultiplier = 1;
	//// If our current game mode is AMainGameMode
	//if (AMainGameMode* gameMode = Cast<AMainGameMode>(UGameplayStatics::GetGameMode(this)))
	//{
	//	// Multiply enemy count by amount of players
	//	enemyMultiplier += gameMode->GetPlayerList().Num();
	//}
	//m_remainingEnemies = (m_maxChunky*enemyMultiplier) + (m_maxBee * enemyMultiplier) + (m_maxHook * enemyMultiplier);
}

// Called every frame
void AAISpawner::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (GetLocalRole() == ROLE_Authority)
	{
		if (GetShouldActivate() && m_spawnerHasInitialized == false && m_enemyPool->GetPoolHasInitialized() == true)
		{
			InitializeList();
		}
		if (/*m_artifactHasSpawned &&*/ m_remainingEnemies <= 0.0f && trigger == false)
		{
			//GetSpawnedArtifacted()->SetActorLocation(GetRandLocation(GetRandSpawnPoint()));
			//GetSpawnedArtifacted()->InitLifetime();
			m_spawnerHasBeenCleared = true;
			trigger = true;
		}

		RandTime();
	}
	if (GetSpawnedArtifacted() != nullptr)
	{
		if (GetSpawnedArtifacted()->GetIsActiveInteractable() == false)
		{
			if (GetLocalRole() == ROLE_Authority && m_spawnerHasBeenCleared)
			{
				// Turn Off Entrance
				HideMesh(true, BlockerType::BT_Entrance);
				ToggleCollision(false, BlockerType::BT_Entrance);

				// Turn Off Exit
				HideMesh(true, BlockerType::BT_Exit);
				ToggleCollision(false, BlockerType::BT_Exit);
			}
		}
	}
}

void AAISpawner::SpawnCharacter()
{
	if (GetShouldActivate())
	{
		if (m_enemySpawnList.Num() > 0)
		{
			/*Get a random enemy in the list and a random spawn point and move the enemies pos to that location*/
			int randIndex = FMath::RandRange(0, m_enemySpawnList.Num() - 1);
			AAIBaseCharacter* spawningEnemy = m_enemySpawnList[randIndex];
            if (spawningEnemy->GetIsFinishedSpawning() == false)
            {
                spawningEnemy->FinishSpawning(spawningEnemy->GetTransform());
                spawningEnemy->SetIsFinishedSpawning(true);
            }
            spawningEnemy->Initialize();
			spawningEnemy->SetActorLocation(GetRandLocation(GetRandSpawnPoint()));
			/*Turn on the collision and gravity*/
			spawningEnemy->GetCapsuleComponent()->SetCollisionProfileName("CharacterCollision");
			Cast<UCharacterMovementComponent>(spawningEnemy->GetMovementComponent())->GravityScale = 1.0f;
			spawningEnemy->SetActorTickEnabled(true);
			spawningEnemy->SetIsActive(true);
			/*Start BehaviorTree*/
			spawningEnemy->StartBehaviorTree();

			/*Remove it from the list so it is not spawned again*/
			m_enemySpawnList.Remove(spawningEnemy);
		}
	}
}

void AAISpawner::InitializeList()
{
	// init local variables
	int beeCounter = 0;
	int hookCounter = 0;
	int chonkCounter = 0;

	// Multiplier applied to the enemy count
	int enemyMultiplier = 1;
	// If our current game mode is AMainGameMode
	if (AMainGameMode* gameMode = Cast<AMainGameMode>(UGameplayStatics::GetGameMode(this)))
	{
		// Multiply enemy count by amount of players
		enemyMultiplier += gameMode->GetPlayerList().Num();
	}

	TArray<AAIBaseCharacter*> enemyPool = m_enemyPool->GetEnemyPool();
	// initialize each enemy in the enemy pool
	for (AAIBaseCharacter* enemy : enemyPool)
	{
		/* Hook Hand */
#pragma region init Hook Hand
		if (Cast<AAIHookHand>(enemy))
		{
			// if amount of hook hand in spawner is less than the max amount of hook hand (including player multiplier) 
			if (hookCounter < m_maxHook * enemyMultiplier)
			{
				// grab a hook hand from the pool and add it to the spawn list
				m_enemySpawnList.Add(enemy);
				hookCounter++;
				m_enemyPool->RemoveFromPool(enemy);
			}
		}
#pragma endregion init Hook Hand
		/* Bee Man */
#pragma region init Bee man
		if (Cast<AAIBee>(enemy))
		{
			// if amount of bee man in spawner is less than the max amount of bee man (including player multiplier) 
			if (beeCounter < m_maxBee * enemyMultiplier)
			{
				// grab a bee man from the pool and add it to the spawn list
				m_enemySpawnList.Add(enemy);
				beeCounter++;
				m_enemyPool->RemoveFromPool(enemy);
			}
		}
#pragma endregion init Bee man
		/* Chungy */
#pragma region init Chungy
		if (Cast<AAIChonk>(enemy))
		{
			// if amount of chungy in spawner is less than the max amount of chungy (including player multiplier) 
			if (chonkCounter < m_maxChunky * enemyMultiplier)
			{
				// grab a chungy from the pool and add it to the spawn list
				m_enemySpawnList.Add(enemy);
				chonkCounter++;
				m_enemyPool->RemoveFromPool(enemy);
			}
		}
#pragma endregion init Chungy
		// Set the owning spawner
		enemy->SetOwningSpawner(this);
	}
    m_remainingEnemies = m_enemySpawnList.Num();
	m_spawnerHasInitialized = true;
}

void AAISpawner::SendBackToPool(AAIBaseCharacter* a_charToAdd)
{
	/*Turn off the collision and gravtiy and send it back to the pool*/
	a_charToAdd->GetCapsuleComponent()->SetCollisionProfileName("OverlapAll");
	Cast<UCharacterMovementComponent>(a_charToAdd->GetMovementComponent())->GravityScale = 0.0f;
	a_charToAdd->SetActorTickEnabled(false);
	a_charToAdd->SetIsActive(false);
	a_charToAdd->StopBehaviorTree();
	m_enemyPool->AddBackToPool(a_charToAdd);
	/*Subtract from the remain enemy count when one is sent back to the pool*/
	m_remainingEnemies--;
}

void AAISpawner::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	// Setup overlap check variables
	TArray<AActor*> overlappingActors;
	TSubclassOf<APlayerCharacter> classFilter;
	// Get the list of players that login into the game 
	int PlayerNum = 0;
	if (GetLocalRole() == ROLE_Authority)
	{
		AMainGameMode* GM = Cast<AMainGameMode>(GetWorld()->GetAuthGameMode());
		// Get that number of players  
		if (GM != nullptr)
			PlayerNum = GM->GetNumPlayers();
	}
	// Get overlapping actors on the activation hitbox
	m_activateCheckBox->GetOverlappingActors(overlappingActors, classFilter);
	int OverlapNum = 0;
	for (auto overlappingCharacter : overlappingActors)
		// If the overlapping actor is a player character increase the count and block the exit
		if (APlayerCharacter* OverlappingPlayer = Cast<APlayerCharacter>(overlappingCharacter))
		{
			OverlapNum++;
			m_playerIsArena = true;

			//if (GetLocalRole() == ROLE_Authority && !m_spawnerHasBeenCleared)
			//{
			//	// Turn On Exit
			//	HideMesh(false, BlockerType::BT_Exit);
			//	ToggleCollision(true, BlockerType::BT_Exit);
			//}
		}

	// If the number of players in the spawner's sphere are the total amount of players in the world activate the spawner and the entrance blocker
	if (PlayerNum > 0)
	{
		if (OverlapNum == PlayerNum)
		{
			SetShouldActivate(true);
			m_spawnerHasStarted = true;
			//if (GetLocalRole() == ROLE_Authority && !m_spawnerHasBeenCleared)
			//{
			//	// Turn On Entrance
			//	HideMesh(false, BlockerType::BT_Entrance);
			//	ToggleCollision(true, BlockerType::BT_Entrance);
			//}
		}
	}
}

void AAISpawner::OnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	// Setup overlap check variables
	TArray<AActor*> overlappingActors;
	TSubclassOf<APlayerCharacter> classFilter;
	int overlappingPlayerNum = 0;

	m_activateCheckBox->GetOverlappingActors(overlappingActors, classFilter);
	for (auto overlappingCharacter : overlappingActors)
	{
		// If the overlapping actor is a player character, increase amount of overlapping players
		if (APlayerCharacter* OverlappingPlayer = Cast<APlayerCharacter>(overlappingCharacter))
		{
			overlappingPlayerNum += 1;
		}
	}
	// If there are no overlapping players, deactivate the spawner and stop the blockers from updating
	if (overlappingPlayerNum <= 0)
	{
		SetShouldActivate(false);
		m_playerIsArena = false;
		

		if (GetLocalRole() == ROLE_Authority && !m_spawnerHasBeenCleared)
		{
			// Turn Off Entrance
			HideMesh(true, BlockerType::BT_Entrance);
			ToggleCollision(false, BlockerType::BT_Entrance);

			// Turn Off Exit
			HideMesh(true, BlockerType::BT_Exit);
			ToggleCollision(false, BlockerType::BT_Exit);
		}
	}
}

void AAISpawner::SpawnArtifact()
{
	SERVER_SpawnArtifact();

}

void AAISpawner::SERVER_SpawnArtifact_Implementation()
{
	if (m_artifacts)
	{
		auto spawnedArtifact = GetWorld()->SpawnActor<AArtifactInteractable>(m_artifacts, GetRandLocation(GetRandSpawnPoint())/*m_enemyPool->GetActorLocation()*/, GetActorRotation());
		if (spawnedArtifact)
		{
			m_artifactHasSpawned = true;
			if (m_spawnedArtifacted_r == nullptr)
			{
				m_spawnedArtifacted_r = spawnedArtifact;
			}
		}
	}
}

void AAISpawner::HideMesh(bool a_hide, BlockerType type)
{
	Net_HideMesh(a_hide, type);
}

void AAISpawner::Net_HideMesh_Implementation(bool a_hide, BlockerType type)
{
	switch (type)
	{
	case BlockerType::BT_Entrance:
		m_staticMeshCompEntrance_r->SetHiddenInGame(a_hide);
		break;
	case BlockerType::BT_Exit:
		m_staticMeshCompExit_r->SetHiddenInGame(a_hide);
		break;
	default:
		break;
	}
}

void AAISpawner::ToggleCollision(bool collisionEnabled, BlockerType type)
{
	Net_ToggleCollision(collisionEnabled, type);
}

void AAISpawner::Net_ToggleCollision_Implementation(bool collisionEnabled, BlockerType type)
{
	if (collisionEnabled)
	{
		switch (type)
		{
		case BlockerType::BT_Entrance:
			m_staticMeshCompEntrance_r->SetCollisionProfileName("BlockAll");
			break;
		case BlockerType::BT_Exit:
			m_staticMeshCompExit_r->SetCollisionProfileName("BlockAll");
			break;
		default:
			break;
		}
	}
	else if (!collisionEnabled)
	{
		switch (type)
		{
		case BlockerType::BT_Entrance:
			m_staticMeshCompEntrance_r->SetCollisionProfileName("NoCollision");
			break;
		case BlockerType::BT_Exit:
			m_staticMeshCompExit_r->SetCollisionProfileName("NoCollision");
			break;
		default:
			break;
		}
	}
}

void AAISpawner::RandTime()
{
	m_randSpawnTime = FMath::RandRange(m_minSpawnTime, m_maxSpawnTime);
}

void AAISpawner::SetSpawnPointPos()
{
	for (AActor* sp : m_spawnPoints)
	{
		if (sp != nullptr)
		{
			m_spawnPointsPos.Add(sp->GetActorLocation());
		}
	}
}

FVector AAISpawner::GetRandLocation(FVector SpawnPointLocation)
{
	FVector NewLoc = FVector::ZeroVector;
	FVector2D randPointInCircle = FMath::RandPointInCircle(64.0f);
	return FVector(SpawnPointLocation.X + randPointInCircle.X, SpawnPointLocation.Y + randPointInCircle.Y, SpawnPointLocation.Z);
}

FVector AAISpawner::GetRandSpawnPoint()
{
	int randIndex = FMath::RandRange(0, m_spawnPoints.Num() - 1);
	return m_spawnPointsPos[randIndex];
}

void AAISpawner::GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AAISpawner, m_spawnedArtifacted_r)
	DOREPLIFETIME(AAISpawner, m_staticMeshCompEntrance_r)
	DOREPLIFETIME(AAISpawner, m_staticMeshCompExit_r)
}
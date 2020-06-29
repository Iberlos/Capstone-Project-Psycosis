// Fill out your copyright notice in the Description page of Project Settings.


#include "AIBaseCharacter.h"

#include "GameObjects/Players/PlayerCharacter.h"
#include "Custom_Components/ObjectHealthComponent.h"
#include "AICharacterController.h"
#include "AISpawner.h"
#include "GameMode/MainGameMode.h"
#include "Custom_Components/InventoryComponent.h"
#include "Custom_Components/ItemGeneratorComponent.h"
#include "GameObjects/Interactables/Pickups/PickupInteractable.h"

#include "Perception/PawnSensingComponent.h"
#include "GameFramework/Pawn.h"
#include "Net/UnrealNetwork.h"

#include "Kismet/GameplayStatics.h"

AAIBaseCharacter::AAIBaseCharacter()
{
    // Setup Pawn Sensing Component
    m_pawnSensingComp = CreateDefaultSubobject<UPawnSensingComponent>("Pawn Sensing");
    m_pawnSensingComp->SightRadius = 0.0f;
    m_pawnSensingComp->SetPeripheralVisionAngle(180.0f);
	m_pawnSensingComp->bOnlySensePlayers = true;
    m_pawnSensingComp->bSeePawns = true;
    m_pawnSensingComp->OnSeePawn.AddDynamic(this, &AAIBaseCharacter::OnSeePawn);

    // Setup other variables
    //m_healthComponent->SetHealth(m_health);
    m_aiState = EAIStates::AI_Idle;
    m_senseTimeLimit = 2.5f;
    m_distanceToPlayer = 0;
    // Attack variables
    m_pAttackStrength = 0;
    m_sAttackStrength = 0;
    m_pAttackRange = 0;

	//Drop
	m_itemGeneratorComponent = CreateDefaultSubobject<UItemGeneratorComponent>(TEXT("Item Generator Component"));

    //Add Cues Related to the AiBase base class
    m_soundCues.Add("Attack");
    m_soundCues.Add("SpotPlayer");
    m_soundCues.Add("Chase");

    //Add partciles Related to the AiBase base class
    m_particleFXs.Add("Attack");
    m_particleFXs.Add("SpotPlayer");
    m_particleFXs.Add("Chase");

    //Add anim montages related to the AiBase base class
    m_animationMontages_TPM.Add("Attack");
    m_animationMontages_TPM.Add("SpotPlayer");
    m_animationMontages_TPM.Add("Chase");

    // Replication
    SetReplicates(true);
    SetReplicateMovement(true);

    //Release 3 
    //Cast<UPrimitiveComponent>(RootComponent)->SetSimulatePhysics(true);
    //Cast<UPrimitiveComponent>(RootComponent)->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
}

void AAIBaseCharacter::BeginPlay()
{
    // Super
    Super::BeginPlay();
    
    // If the controller is an AI controller, cast it and set a reference to it
    if (Cast<AAICharacterController>(GetController()))
        m_AIController = Cast<AAICharacterController>(GetController());
    if (GetLocalRole() == ROLE_Authority)
    {
        if (!m_isInPool && m_AIController != nullptr)
        {
            SetActorTickEnabled(true);
            SetIsActive(true);
            m_AIController->StartBehaviorTree();
        }
        // Set attack ranges
        SetPrimaryAttackRange(m_pAttackRange);
        SetSecondaryAttackRange(m_sAttackRange);
    }
    // Multiplier applied to the health based on number of players in the world
    int healthMultiplier = 1;
    // If our current game mode is AMainGameMode
    if(AMainGameMode* gameMode = Cast<AMainGameMode>(UGameplayStatics::GetGameMode(this)))
    {
        // Multiply health by amount of players
        healthMultiplier += gameMode->GetPlayerList().Num();
    }

    //GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Health Multiplier: ") + FString::FromInt(healthMultiplier));
    m_healthComponent->SetMaxHealth(m_health * healthMultiplier);
    if (GetLocalRole() == ROLE_Authority)
    {
        /*Set the sense raduis a second after spawning so the spawner is properly set*/
        FTimerHandle attackTimer;
        GetWorldTimerManager().SetTimer(attackTimer, this, &AAIBaseCharacter::SetAttackRange, 1.0f, false);
        /*Reset the targeted player to null*/
        GetWorldTimerManager().SetTimer(m_resetTimer, this, &AAIBaseCharacter::Reset, 20.0f, true);
        /*Calulate the distance between this enemy and the player*/
        FTimerHandle distTimer;
        GetWorldTimerManager().SetTimer(distTimer, this, &AAIBaseCharacter::SetDistToPlayer, 1.0f, true);
    }
}

void AAIBaseCharacter::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
    /*Reset the targeted player to null*/
    //Reset();
    /*Calulate the distance between this enemy and the player*/
    //SetDistToPlayer();

    //// Check if dead
    //if (!IsAlive())
    //    Die();
    if (GetLocalRole() == ROLE_Authority)
    {
        if (m_AIController && m_currentTarget_r != nullptr)
        {
            // Tell the controller if the targeted player is within attack range
            m_TargetedPlayerIsWithinPrimaryAttackRange = GetTargetedPlayerIsWithinPrimaryAttackRange();
            m_AIController->SetTargetedPlayerIsWithinPrimaryAttackRange(m_TargetedPlayerIsWithinPrimaryAttackRange);
        }
    }
}

void AAIBaseCharacter::OnSeePawn(APawn* Pawn)
{
    /*if (!IsAlive())
        return;*/
    /*Get the last time the player has been seen by this AI*/

		m_LastTimePlayerSeen = GetWorld()->GetTimeSeconds();
		m_targetIsSensed = true;
		/*Set the target to the player that has been sensed*/
		auto c = Cast<APlayerCharacter>(Pawn);
		if (c && !c->GetHealthComponent()->CheckIfDead())
		{
			if (m_currentTarget_r != c)
			{
				if (m_currentTarget_r == nullptr)
				{
					PlaySound("SpotPlayer", true);
					PlayParticle("SpotPlayer", true);
					PlayAnimationMontage("SpotPlayer");

					m_currentTarget_r = c;
					if (m_AIController)
						m_AIController->SetTarget(m_currentTarget_r);
				}
				else
				{
					if (FVector::DistSquared(m_currentTarget_r->GetActorLocation(), GetActorLocation()) > FVector::DistSquared(c->GetActorLocation(), GetActorLocation()))
					{
						m_currentTarget_r = c;
						if (m_AIController)
							m_AIController->SetTarget(m_currentTarget_r);
					}
				}
			}
			else
			{
				GetWorldTimerManager().SetTimer(m_resetTimer, this, &AAIBaseCharacter::Reset, 20.0f, true);
			}
		}

 //   FTimerHandle t;
 //   GetWorldTimerManager().SetTimer(t, this, &AAIBaseCharacter::PlayChasePlayerFeedback, 3.0f, false);
}

// Check if this AI Character is alive
bool AAIBaseCharacter::IsAlive()
{
   // Get health
	return !m_hasDied;
}

void AAIBaseCharacter::Reset()
{
    //SERVER_Reset();
    if (m_targetIsSensed)
    {
        /*If the last time the player was seen is greater than the sense time limit set the target to null*/
        float TimeOut = GetWorld()->GetTimeSeconds() - m_LastTimePlayerSeen;
        if (TimeOut > m_senseTimeLimit)
        {
            if (m_AIController)
            {
                m_targetIsSensed = false;
                m_currentTarget_r = nullptr;
                m_AIController->SetTarget(m_currentTarget_r);
            }
        }
    }
}

void AAIBaseCharacter::SERVER_Reset_Implementation()
{
    /*if (m_targetIsSensed)
    {
        float TimeOut = GetWorld()->GetTimeSeconds() - m_LastTimePlayerSeen;
        if (TimeOut > m_senseTimeLimit)
        {
            if (m_AIController)
            {
                m_targetIsSensed = false;
                m_currentTarget_r = nullptr;
                m_AIController->SetTarget(m_currentTarget_r);
            }
        }
    }*/
}

// Send this character back to spawner or destroy it
void AAIBaseCharacter::Die()
{
	Super::Die();
	StopBehaviorTree();
	TArray<APickupInteractable*> generatedPickups = m_itemGeneratorComponent->Generate(1);
	//Interact with all pickups
	if (generatedPickups.Num() != 0)
	{
		for (APickupInteractable* pickup : generatedPickups)
		{
			pickup->Interact(this);
		}
	}
}

void AAIBaseCharacter::FinalizeDie()
{
	if (GetLocalRole() == ROLE_Authority)
	{
		//Drop all the pickups in the world
		m_inventoryComponent->FlushInventory();

		if (!IsAlive())
		{
			// If this character came from a spawner
			if (GetOwningSpawner() != nullptr)
			{
				// Send back to spawner
				GetOwningSpawner()->SendBackToPool(this);
				GetHealthComponent()->SetMaxHealth(100.0f);
				GetHealthComponent()->SetHealth(100.0f);
			}
			else // If this character was placed in the world
			{
				Destroy();
			}

			if (m_attackingPlayer_r != nullptr)
			{
				m_attackingPlayer_r->GetHealthComponent()->AddHealth(10.0f, true);
			}
		}
	}
}

/*void AAIBaseCharacter::SERVER_RegenPlayerHealthOnDeath(float a_regenAmount, bool a_regenInPercent)
{
	m_attackingPlayer_r->GetHealthComponent()->AddHealth(a_regenAmount, a_regenInPercent);
}*/

#pragma region Setters for AI Controller variables

void AAIBaseCharacter::SetAIState(EAIStates a_aiState)
{
    if (m_AIController)
    {
        m_AIController->SetAIState(a_aiState);
    }
}

void AAIBaseCharacter::SetPrimaryAttackRange(float a_pAttackRange)
{
    if (m_AIController)
    {
        m_AIController->SetPrimaryAttackRange(a_pAttackRange);
    }
}

void AAIBaseCharacter::SetSecondaryAttackRange(float a_sAttackRange)
{
    if (m_AIController)
    {
        m_AIController->SetSecondaryAttackRange(a_sAttackRange);
    }
}

void AAIBaseCharacter::SetDistToPlayer()
{
    if (m_AIController && m_currentTarget_r != nullptr)
    {
        FVector PlayerPos = FVector::ZeroVector;
        PlayerPos = m_currentTarget_r->GetActorLocation();

        FVector EnemeyPos = FVector::ZeroVector;
        EnemeyPos = GetActorLocation();

        m_distanceToPlayer = FVector::Dist(PlayerPos, EnemeyPos);

        m_AIController->SetDistToPlayer(m_distanceToPlayer);
    }
}
#pragma endregion Setters for AI Controller variables end

// Get if the distance from the player is within our primary attack range
bool AAIBaseCharacter::GetTargetedPlayerIsWithinPrimaryAttackRange()
{
    // If the distance from the player is within our primary attack range
    if(m_distanceToPlayer <= m_pAttackRange)
        return true;
    else
        return false;
}

void AAIBaseCharacter::Initialize()
{
	Respawn();
}

void AAIBaseCharacter::PrimaryAttack()
{
	//Play an animation with a random range. *The worng key won't cause crashes but it will cause the animation to not be played*
	PlaySound("Attack", true);
	PlayParticle("Attack", true);
	PlayAnimationMontage("Attack");
}

void AAIBaseCharacter::SetAttackRange()
{
    /*If there is an owing spawner set the sense radius to be huge so they always move towards the player*/
    if(GetOwningSpawner())
        m_pawnSensingComp->SightRadius = 8000.0f;
    /*If there isn't make it low so they only move when the player is close*/
    else
        m_pawnSensingComp->SightRadius = 2000.0f;
}

void AAIBaseCharacter::StartBehaviorTree()
{
	m_AIController->StartBehaviorTree();
}

void AAIBaseCharacter::StopBehaviorTree()
{
	m_AIController->StopBehaviorTree();
}

//void AAIBaseCharacter::GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const
//{
    //Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    //// Replicated variables
    //DOREPLIFETIME(AAIBaseCharacter, m_currentTarget_r);
    //DOREPLIFETIME(AAIBaseCharacter, m_attackingPlayer_r);
    //DOREPLIFETIME(AAIBaseCharacter, m_pawnSensingComp);
//}
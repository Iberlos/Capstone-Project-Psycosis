// Fill out your copyright notice in the Description page of Project Settings.

//Header include
#include "PortalInteractable.h"

//Custom Engine Includes
#include "Engine/Engine.h"
#include "Net/UnrealNetwork.h"
#include "GameFramework/GameModeBase.h"
#include "Components/BoxComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/Public/TimerManager.h"

//Custom Project Includes
#include "../Players/PlayerCharacter.h"
#include "GameInstance/CapstoneGameInstance.h"
#include "ArtifactInteractable.h"

APortalInteractable::APortalInteractable()
{
	m_portalTriggerBox = CreateDefaultSubobject<UBoxComponent>("Portal Trigger");
	m_portalTriggerBox->SetupAttachment(RootComponent);
	m_portalTriggerBox->SetGenerateOverlapEvents(true);
	m_portalTriggerBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	m_portalTriggerBox->OnComponentBeginOverlap.AddDynamic(this, &APortalInteractable::OnOverlap);

	m_colliderComponent->SetSimulatePhysics(false);
	m_artifactsInLevel_r = 0;
	m_artifactsPickedUp_r = 0;
	m_levelName = "Level_Lobby_2";

	//Add Cues Related to the InteractableActor base class
	m_soundCues.Add("PortalOpen");

	//Add partciles Related to the InteractableActor base class
	m_particleFXs.Add("PortalOpen");

	//Add Animation Montages Related to the InteractableActor base class
	m_animationMontages.Add("PortalOpen");
}

void APortalInteractable::BeginPlay()
{
	Super::BeginPlay();
	//setup a delegate to call FindAllArtifactsAndRegister in 10 seconds only on server side
	if (GetLocalRole() == ROLE_Authority)
	{
		FTimerHandle t;
		GetWorldTimerManager().SetTimer(t, this, &APortalInteractable::GetAllArtifactsAndRegister, 10.0f, false);
	}
}

void APortalInteractable::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

bool APortalInteractable::Interact(AActor* a_actor)
{
	if (Super::Interact(a_actor) && m_artifactsInLevel_r <= m_artifactsPickedUp_r)//if you can't be highlited don't call your functions too
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("The portal is open! to level " + m_levelName.ToString()));
		m_portalTriggerBox->SetCollisionProfileName("OverlapAllDynamic");
		PlaySound("PortalOpen", true);
		PlayParticle("PortalOpen", true);
		return true;
	}
	else
	{
		return false;
	}
}

void APortalInteractable::GetAllArtifactsAndRegister()
{
	//get all objects of class artifact interactable and artifact->ed_ArtifactInteractedWith.AddDynamic(this, &APortalInteractable::OnArtifactReportInteracted);
	TSubclassOf<AArtifactInteractable> classToFind = AArtifactInteractable::StaticClass();
	TArray<AActor*> foundArtifacts;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), classToFind, foundArtifacts);

	m_artifactsInLevel_r = foundArtifacts.Num();

	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("Found Artifacts: %d"), m_artifactsInLevel_r));

	for (AActor* actor: foundArtifacts)
	{
		AArtifactInteractable* artifact = Cast<AArtifactInteractable>(actor);
		if (artifact)
		{
			if (artifact->GetIsActiveInteractable())
			{
				artifact->ed_ArtifactInteractedWith.AddDynamic(this, &APortalInteractable::OnArtifactReportInteracted);
			}
			else
			{
				m_artifactsPickedUp_r++; //This artifact was picked up before it was registered so count it since the even won't reach you in time.
			}
		}
	}
}

void APortalInteractable::ServerTravel()
{
	UWorld* World = GetWorld();
	World->GetAuthGameMode()->bUseSeamlessTravel = true;
	World->ServerTravel("/Game/Levels/" + m_levelName.ToString() + "?listen");
}

void APortalInteractable::ChangeLevel()
{
	UGameplayStatics::OpenLevel(GetWorld(), m_levelName);
}

void APortalInteractable::OnArtifactReportInteracted()
{
	m_artifactsPickedUp_r++;
}


void APortalInteractable::OnOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	APlayerCharacter* player = Cast<APlayerCharacter>(OtherActor);
	if (player)
	{
		UCapstoneGameInstance* GI = Cast<UCapstoneGameInstance>(GetWorld()->GetGameInstance());

		if (GI->GetMutltiplayerActive())
			ServerTravel();
		else if (!GI->GetMutltiplayerActive())
			ChangeLevel();
	}
}

void APortalInteractable::GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(APortalInteractable, m_artifactsInLevel_r)
		DOREPLIFETIME(APortalInteractable, m_artifactsPickedUp_r)
}

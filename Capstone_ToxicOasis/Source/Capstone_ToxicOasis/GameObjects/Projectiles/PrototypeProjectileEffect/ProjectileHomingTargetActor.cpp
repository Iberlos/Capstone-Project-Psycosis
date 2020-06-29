// Fill out your copyright notice in the Description page of Project Settings.


#include "ProjectileHomingTargetActor.h"
#include "UnrealNetwork.h"

// Sets default values
AProjectileHomingTargetActor::AProjectileHomingTargetActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	m_targetComp = CreateDefaultSubobject<USceneComponent>("Target");
	m_targetComp->SetIsReplicated(true);
	RootComponent = m_targetComp;

	SetReplicates(true);
	SetReplicateMovement(true);
}

// Called when the game starts or when spawned
void AProjectileHomingTargetActor::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AProjectileHomingTargetActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AProjectileHomingTargetActor::GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AProjectileHomingTargetActor, m_targetComp);
}
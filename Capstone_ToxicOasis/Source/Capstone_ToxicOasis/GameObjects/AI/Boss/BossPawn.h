// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "BossPawn.generated.h"

class USphereComponent;
class UObjectHealthComponent;

enum Operation
{
	OP_ADD,
	OP_SUBTRACT,
	OP_DEFAULT,
};

UCLASS()
class CAPSTONE_TOXICOASIS_API ABossPawn : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	ABossPawn();


#pragma region 	/* PROTECTED FUNCTIONS */

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UFUNCTION()
		void OnActivate(); // Function that is called when the boss is activated

#pragma region SETTERS
	//BossState
	UFUNCTION()
	void SetBossState(); // Set the boss state automatically based off the boss' health
	void SetBossState(class UBossStateBaseComponent* a_currentBossState) { m_CurrentBossState = a_currentBossState; } // Set Boss State Manually
	// Health
	//void SetHealth(float a_health) { m_health = a_health; }
	//void SetHealth(float a_health, int a_operation);
	// Vulnerability
	void SetVulnerabilityDuration(float a_vulnerabilityDuration) { m_vulnerabilityDuration = a_vulnerabilityDuration; }
	void SetVulnerableState(bool a_vulnerable) { m_vulnerable = a_vulnerable; }
	// Is active
	void SetActive(bool a_active) { m_active = a_active; }
#pragma endregion

#pragma region GETTERS
	UBossStateBaseComponent* GetBossState() { return m_CurrentBossState; }
	//float GetHealth() { return m_health; }
	float GetVulnerabilityDuration() { return m_vulnerabilityDuration; }
	bool GetVulnerableState() { return m_vulnerable; }
	bool GetActive() { return m_active; }
#pragma endregion

	void ToggleVulnerableState(bool a_vulnerable); // Set up or remove the vulnerable state
	void VulnerableBeginAnimTransition();
	void VulnerableBegin(); // Start the Boss' vulnerable State
	void VulnerableEndAnimTransition();
	void VulnerableEnd();   // When the Boss' vulnerable State ends

	void DieAnim();
	void Die(); // Make the boss die
#pragma endregion

#pragma region /* PUBLIC FUNCTIONS */
public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

#pragma region /* SETTERS */
#pragma endregion
#pragma region /* GETTERS */
	UObjectHealthComponent* GetHealthComponent() { return m_healthComponent; }
#pragma endregion

	UFUNCTION()
		void BeginOverlap(UPrimitiveComponent* OverlappedComponent,
			AActor* OtherActor,
			UPrimitiveComponent* OtherComp,
			int32 OtherBodyIndex,
			bool bFromSweep,
			const FHitResult& SweepResult);

	void TakeDamage(float a_damage);

	void BeginStateLoop();
	void EndStateLoop();

#pragma endregion

#pragma region 	/* PROTECTED MEMBERS */
protected:

	UPROPERTY(EditDefaultsOnly)
		class USkeletalMeshComponent* m_BossTempMesh;
	UPROPERTY(EditDefaultsOnly)
		class UBoxComponent* m_bossBasicCollision;
	UPROPERTY(EditDefaultsOnly)
		USphereComponent* m_ActivateTriggerSphere; // Trigger hitbox
	UPROPERTY(EditDefaultsOnly)
		USphereComponent* m_CrystalDamageSphere;   // Crystal damage hitbox

	//UPROPERTY(EditAnywhere)
	//class UBossStateDenialComponent* m_BossStateDenial;    // Denial
	//UPROPERTY(EditAnywhere)
	UPROPERTY(EditAnywhere)
		class UBossStateAngerComponent* m_BossStateAnger;      // Anger
	UPROPERTY(EditAnywhere)
		class UBossStateBargainComponent* m_BossStateBargain;  // Bargain
	UPROPERTY(EditAnywhere)
		class UBossStateSadComponent* m_BossStateSad;          // Sad

	UBossStateBaseComponent* m_CurrentBossState;     // Current Boss State

	FTimerHandle m_vulnerabilityTimer;  // Timer for the vulnerability state
	float m_vulnerabilityDuration;

	FTimerHandle m_deathTimer;

	//float m_health; // Boss health
	UObjectHealthComponent* m_healthComponent;
	bool m_active;  // is the boss active

	bool m_vulnerable; // is the boss vulnerable to attacks

	// ANIMATION
	UPROPERTY(EditDefaultsOnly)
		class UAnimationAsset* m_IdleAnim;
	UPROPERTY(EditDefaultsOnly)
		UAnimationAsset* m_VulnerableEnterAnim;
	UPROPERTY(EditDefaultsOnly)
		UAnimationAsset* m_VulnerableIdleAnim;
	UPROPERTY(EditDefaultsOnly)
		UAnimationAsset* m_VulnerableExitAnim;

	// WATER
	UPROPERTY(EditDefaultsOnly)
		UAnimationAsset* m_WaterEnterAnim;
	UPROPERTY(EditDefaultsOnly)
		UAnimationAsset* m_WaterIdleAnim;
	UPROPERTY(EditDefaultsOnly)
		UAnimationAsset* m_WaterExitAnim;

	// DEATH
	UPROPERTY(EditDefaultsOnly)
		UAnimationAsset* m_DeathEnterAnim;
	UPROPERTY(EditDefaultsOnly)
		UAnimationAsset* m_DeathIdleAnim;
#pragma endregion

public:
	void WaterBeginAnimTransition();
	void WaterIdle(); // Start the Boss' water State
	void WaterEndAnimTransition();

	float m_health;
};

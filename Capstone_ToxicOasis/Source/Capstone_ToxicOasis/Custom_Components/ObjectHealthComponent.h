//Default Unreal Includes
#pragma once

//Custom Unreal includes #There shouldn't be any includes in the .h file, but if one is needed it goes here.#

#include "Engine.h" //here to avoid timer forward decleration issue, just here to avoid red squiggly line

//Custom project includes #There shouldn't be any includes in the .h file, but if one is needed it goes here.#

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ObjectHealthComponent.generated.h"

/*
 * #Description of the class. Not required, but usefull if you have time to make a description.#
 */

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class CAPSTONE_TOXICOASIS_API UObjectHealthComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    //Essentials
    UObjectHealthComponent(); //Constructor should exist on any class even if it is empty.
    virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override; //DeltaTime doesn't follow naming convention because it is in a default function.

    //Other methods
    UFUNCTION(BlueprintCallable)
        float GetHealth() { return m_objectHealth_r; }
    UFUNCTION(BlueprintCallable)
        float GetMaxHealth() { return m_maxHealth; }
    UFUNCTION(BlueprintCallable)
        bool CheckIfDead() { return m_isDead; }
    void SetHealth(float a_objectHealth);// used for debug perposes
    void SetMaxHealth(float a_maxObjectHealth);//
    void AddHealth(float a_objectHealth, bool Percentage);
    void SubtractHealth(float a_objectHealth, bool Percentage);
    void BleedHealth(float a_initialDamage, float a_bleedTimerLimit, float a_damagePerSecond);
    void StunHealth(float a_stunTimerLimit);

protected:
    //Essentials
    virtual void BeginPlay() override;

    //Variables
    UPROPERTY(Replicated)
    float m_objectHealth_r;
    float m_maxHealth;
    bool m_isDead;
    bool m_isHealthStuned;
    bool m_startBleed;
    float m_bleedDamagePerSecond;
    float m_desiredPercentHealth;

    FTimerHandle StunTimerHandle;
    FTimerHandle BleedTimerHandle;

    //Other methods
    void BleedHealthEnd() { m_startBleed = false; }
    void StunHealthEnd() { m_isHealthStuned = false; }

};




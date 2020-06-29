//Header include
#include "ObjectHealthComponent.h"

//Custom Engine Includes
#include <Runtime/Engine/Classes/Engine/Engine.h>
#include <Runtime/Engine/Classes/Engine/World.h>
#include "TimerManager.h"
#include "Net/UnrealNetwork.h"

//Custom Project Includes

//Method implementation should be in the same order as the header to keep things organized.The only exception are protected essentials, those should be moved in order to keep all essential methods on the top.#
UObjectHealthComponent::UObjectHealthComponent()
{
    // Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
    // off to improve performance if you don't need them.
    PrimaryComponentTick.bCanEverTick = true;

    //Initializations
    m_maxHealth = 50.0f;
    m_objectHealth_r = m_maxHealth;
    m_isDead = false;
    m_isHealthStuned = false;
    m_startBleed = false;
    m_bleedDamagePerSecond = 0.0f;
}

//BeginPlay is the only function that should be out of order in relation to the header.
void UObjectHealthComponent::BeginPlay()
{
    Super::BeginPlay();

    //Post construction initializations
}

void UObjectHealthComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

    //if health is at or below 0, set to dead
    if (m_objectHealth_r <= 0.0f)
    {
        m_isDead = true;
    }

    //if bleed is turned on damage taken per second based on specified damage
    if (m_startBleed == true && !CheckIfDead())
    {
        m_objectHealth_r -= m_bleedDamagePerSecond * DeltaTime;
    }

}

//sets the object health for debug purposes
void UObjectHealthComponent::SetHealth(float a_objectHealth)
{
        m_objectHealth_r = a_objectHealth;
        if (a_objectHealth != 0.0f)
        {
            m_isDead = false;
        }
}

//sets the max health for object
void UObjectHealthComponent::SetMaxHealth(float a_maxObjectHealth)
{
    if (a_maxObjectHealth != 0.0f)
    {
        m_maxHealth = a_maxObjectHealth;
    }
    else
    {
        GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Yellow, TEXT("Cannot set max health to 0"));
    }
}

//adds a value to the current health, if percentage if true, then the value give will be added as a percentage of the total health
void UObjectHealthComponent::AddHealth(float a_objectHealth, bool Percentage)
{
    if (!CheckIfDead())
    {
        if (!Percentage)
        {
            if (m_objectHealth_r <= m_maxHealth)
            {
                if (m_isHealthStuned == false)
                {
                    m_objectHealth_r += a_objectHealth;

                    if (m_objectHealth_r > m_maxHealth)
                    {
                        m_objectHealth_r = m_maxHealth;
                    }
                }
                else
                {
                    if (GEngine != NULL)
                    {
                        GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Yellow, TEXT("Player health is stunned"));
                    }
                }
            }
        }
        else
        {
            if (m_isHealthStuned == false)
            {
                m_objectHealth_r += ((GetMaxHealth() * a_objectHealth) / 100.0f);

                if (m_objectHealth_r > m_maxHealth)
                {
                    m_objectHealth_r = m_maxHealth;
                }
            }
            else
            {
                if (GEngine != NULL)
                {
                    GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Yellow, TEXT("Player health is stunned"));
                }
            }
        }
    }
}

// subtacts an amount from current health, if percentage if true, then the value give will be subtracted as a percentage of the total health
void UObjectHealthComponent::SubtractHealth(float a_objectHealth, bool Percentage)
{
    if (!CheckIfDead())
    {
        if (!Percentage)
        {
            m_objectHealth_r -= a_objectHealth;
        }
        else
        {
            m_objectHealth_r -= ((GetMaxHealth() * a_objectHealth) / 100.0f);
        }
    }
}

//bleeds the object health over a certain amount of time, at specified rate, an initial damage could be added if required
void UObjectHealthComponent::BleedHealth(float a_initialDamage, float a_bleedTimerLimit, float a_damagePerSecond)
{
    if (!CheckIfDead())
    {
        m_objectHealth_r -= a_initialDamage;
        m_startBleed = true;
        m_bleedDamagePerSecond = a_damagePerSecond;

        GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, TEXT("health bleed started"));
        GetWorld()->GetTimerManager().SetTimer(BleedTimerHandle, this, &UObjectHealthComponent::BleedHealthEnd, a_bleedTimerLimit);
    }
}

//flips a bool to stop object health from increasing for a specified amount of time
void UObjectHealthComponent::StunHealth(float a_stunTimerLimit)
{
    if (!CheckIfDead())
    {
        if (m_isHealthStuned == false)
        {
            m_isHealthStuned = true;

            GetWorld()->GetTimerManager().SetTimer(StunTimerHandle, this, &UObjectHealthComponent::StunHealthEnd, a_stunTimerLimit);
        }
        else
        {
            GetWorld()->GetTimerManager().ClearTimer(StunTimerHandle);
            GetWorld()->GetTimerManager().SetTimer(StunTimerHandle, this, &UObjectHealthComponent::StunHealthEnd, a_stunTimerLimit);
        }
    }
    
}

void UObjectHealthComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    DOREPLIFETIME(UObjectHealthComponent, m_objectHealth_r);
}


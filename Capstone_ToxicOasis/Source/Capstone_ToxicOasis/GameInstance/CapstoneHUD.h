// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "CapstoneHUD.generated.h"

/**
 * 
 */
UCLASS()
class CAPSTONE_TOXICOASIS_API ACapstoneHUD : public AHUD
{
	GENERATED_BODY()
public:
	ACapstoneHUD();

	virtual void BeginPlay() override;
	
private:
	// Reference UMG Asset in the Editor
	UPROPERTY(EditAnywhere, Category = "Widgets")
		TSubclassOf<class UUserWidget> HUDWidgetClass;

	// Variable to hold the widget After Creating it.
	UPROPERTY(EditAnywhere, Category = "Widgets")
		class UUserWidget* CurrentWidget;
	
};

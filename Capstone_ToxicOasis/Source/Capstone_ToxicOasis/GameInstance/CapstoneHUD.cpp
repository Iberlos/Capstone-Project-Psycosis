// Fill out your copyright notice in the Description page of Project Settings.


#include "CapstoneHUD.h"
#include "Engine/Canvas.h"
#include "Engine/Texture2D.h"
#include "TextureResource.h"
#include "CanvasItem.h"
#include "UObject/ConstructorHelpers.h"
#include "Blueprint/UserWidget.h"

ACapstoneHUD::ACapstoneHUD()
{
	static ConstructorHelpers::FClassFinder<UUserWidget>MainMenuObj(TEXT("/Game/Blueprints/GameInstance/MasterWidget"));
	HUDWidgetClass = MainMenuObj.Class;
}

void ACapstoneHUD::BeginPlay()
{
	if (HUDWidgetClass != nullptr)
	{
		CurrentWidget = CreateWidget<UUserWidget>(GetWorld(), HUDWidgetClass);

		if (CurrentWidget)
		{
			CurrentWidget->AddToViewport();
		}
	}
}

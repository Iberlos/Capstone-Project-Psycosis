#pragma once
#include "CoreMinimal.h"
#include "Widgets/SCompoundWidget.h"
#include "Widgets/DeclarativeSyntaxSupport.h"
#include "Widgets/SCompoundWidget.h"
#include "Widgets/SBoxPanel.h"
#include "Widgets/Text/STextBlock.h"
#include "Widgets/Images/SThrobber.h"
#include "MoviePlayer.h"

class CAPSTONE_TOXICOASIS_API LoadingScreenSlateWidget : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(LoadingScreenSlateWidget)
	{}

	// ++ We add a new argument called OwnerHUDArg
	//SLATE_ARGUMENT(TWeakObjectPtr<class AStandardHUD>, OwnerHUDArg);

	SLATE_END_ARGS()

		// Constructs this widget with InArgs. Needed for every widget. Builds this widget and any of it's children
		void Construct(const FArguments& InArgs);

private:

	// ++ Pointer to our parent HUD. To make sure HUD's lifetime is controlled elsewhere, use "weak" ptr.
	// ++ HUD has a "strong" pointer to Widget, circular ownership would prevent/break self-destruction of hud/widget (cause a memory leak).
	//TWeakObjectPtr<class AStandardHUD> OwnerHUD;

	EVisibility GetLoadIndicatorVisibility() const
	{
		return GetMoviePlayer()->IsLoadingFinished() ? EVisibility::Collapsed : EVisibility::Visible;
	}

	EVisibility GetMessageIndicatorVisibility() const
	{
		return GetMoviePlayer()->IsLoadingFinished() ? EVisibility::Visible : EVisibility::Collapsed;
	}

};

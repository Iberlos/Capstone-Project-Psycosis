#include "LoadingScreenSlateWidget.h"
#include "MoviePlayer.h"
#include "SlateOptMacros.h"

BEGIN_SLATE_FUNCTION_BUILD_OPTIMIZATION

// ++ This is needed in order to use the localization macro LOCTEXT
#define LOCTEXT_NAMESPACE "LoadingScreenSlateWidget"

void LoadingScreenSlateWidget::Construct(const FArguments& InArgs)
{
	// ++ Asign the argument to our local variable
		// name will be _OwnerHUDArg instead of OwnerHUDArg, see comments about SLATE_ARGUMENT before
	//OwnerHUD = InArgs._OwnerHUDArg;

	// ++ Add all this Slate code
// If the code below doesn't look like C++ to you it's because it (sort-of) isn't,
// Slate makes extensive use of the C++ Prerocessor(macros) and operator overloading,
// Epic is trying to make our lives easier, look-up the macro/operator definitions to see why.

	ChildSlot
		[
			SNew(SVerticalBox)
			//Jump loading Screen UI element
			+ SVerticalBox::Slot()
		.VAlign(VAlign_Bottom)
		.HAlign(HAlign_Right)
		[
			SNew(SThrobber)
			.Visibility(this, &LoadingScreenSlateWidget::GetLoadIndicatorVisibility)
		]
	+ SVerticalBox::Slot()
		/*.VAlign(VAlign_Bottom)
		.HAlign(HAlign_Right)
		[
			SNew(STextBlock)
			.Text(NSLOCTEXT("MoviePlayerTestLoadingScreen", "LickScreenToSkip", "Click Screen to Skip"))
		.Visibility(this, &LoadingScreenSlateWidget::GetMessageIndicatorVisibility)
		]*/
		];

}

// ++ We need to undefine this namespace after we finish creating the Slate widget
#undef LOCTEXT_NAMESPACE

END_SLATE_FUNCTION_BUILD_OPTIMIZATION
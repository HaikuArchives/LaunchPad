// $Id$

#include "WindowTab.h"
#include "TriStateButton.h"

WindowTab::WindowTab(
					BRect		frame,
					const char*	name,
					uint32		resizingMode,
					uint32		flags
					)
:	WindowDragger( frame, name, resizingMode, flags )
{
	TriStateButton* close = new TriStateButton(
										BRect( 4, 4, 14, 14 ),
										"Close",
										NULL,
										NULL,
										NULL,
										false,
										new BMessage( B_QUIT_REQUESTED )
										);
	AddChild( close );
}

WindowTab::~WindowTab()
{
}

void
WindowTab::AttachedToWindow( void )
{
	WindowDragger::AttachedToWindow();
	SetViewColor( tint_color( ui_color( B_PANEL_BACKGROUND_COLOR ), B_LIGHTEN_2_TINT ) );

	BControl* close = dynamic_cast<BControl*>( FindView( "Close" ) );
	if ( close )
	{
		close->SetTarget( (BHandler*)Window() );
	}
}

void
WindowTab::Draw( BRect updateRect )
{
}

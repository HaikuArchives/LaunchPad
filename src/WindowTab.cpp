// $Id$

#include <be/interface/Bitmap.h>
#include "WindowTab.h"
#include "TriStateButton.h"

WindowTab::WindowTab(
					BRect					frame,
					const char*				name,
					windowtab_orientation	orientation,
					uint32					resizingMode,
					uint32					flags
					)
:	WindowDragger( frame, name, resizingMode, flags ),
	mOrientation( orientation )
{
	// Init button bitmaps
	BBitmap* close_off = new BBitmap( BRect( 0, 0, 8, 8 ), B_CMAP8 );
	close_off->SetBits( CLOSE_BUTTON_BITS_OFF, 108, 0, B_CMAP8 );
	BBitmap* close_on = new BBitmap( BRect( 0, 0, 8, 8 ), B_CMAP8 );
	close_on->SetBits( CLOSE_BUTTON_BITS_ON, 108, 0, B_CMAP8 );

	// Close button
	TriStateButton* close = new TriStateButton(
										BRect( 2, 2, 2, 2 ),
										"Close",
										close_off,
										close_on,
										NULL,
										false,
										new BMessage( B_QUIT_REQUESTED )
										);
	AddChild( close );

	// Resize
	if ( mOrientation == WindowTab::HORIZONTAL )
	{
		ResizeTo( Bounds().Width(), 12 );
	}
	else
	{
		ResizeTo( 12, Bounds().Height() );
	}
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
	rgb_color light = tint_color( ViewColor(), B_LIGHTEN_MAX_TINT );
	rgb_color dark  = tint_color( ViewColor(), B_DARKEN_1_TINT );

	BRect bounds( Bounds() );

	SetHighColor( dark );
	StrokeRect( bounds );

	SetHighColor( light );
	MovePenTo( 0, bounds.bottom );
	StrokeLine( B_ORIGIN );
	StrokeLine( BPoint( bounds.right, 0 ) );
}

unsigned char
WindowTab::CLOSE_BUTTON_BITS_OFF[] = {
//	  1   2   3   4   5   6   7   8   9   x   x   x
	 20, 20, 20, 20, 20, 20, 20, 20, 30,255,255,255,	//	1
	 20, 63, 63, 63, 63, 63, 63, 63, 63,255,255,255,	//	2
	 20, 63, 30, 27, 27, 22, 22, 20, 63,255,255,255,	//	3
	 20, 63, 27, 27, 22, 27, 22, 20, 63,255,255,255,	//	4
	 20, 63, 27, 22, 27, 22, 22, 20, 63,255,255,255,	//	5
	 20, 63, 22, 27, 22, 22, 21, 20, 63,255,255,255,	//	6
	 20, 63, 22, 22, 21, 21, 21, 20, 63,255,255,255,	//	7
	 20, 63, 20, 20, 20, 20, 20, 20, 63,255,255,255,	//	8
	 30, 63, 63, 63, 63, 63, 63, 63, 63,255,255,255		//	9
};

unsigned char
WindowTab::CLOSE_BUTTON_BITS_ON[] = {
//	  1   2   3   4   5   6   7   8   9   x   x   x
	 20, 20, 20, 20, 20, 20, 20, 20, 30,255,255,255,	//	1
	 20, 17, 17, 17, 17, 17, 17, 22, 63,255,255,255,	//	2
	 20, 17, 19, 19, 22, 20, 24, 29, 63,255,255,255,	//	3
	 20, 17, 19, 27, 22, 27, 24, 29, 63,255,255,255,	//	4
	 20, 17, 19, 22, 27, 24, 27, 29, 63,255,255,255,	//	5
	 20, 17, 20, 27, 24, 27, 27, 29, 63,255,255,255,	//	6
	 20, 17, 24, 24, 27, 27, 30, 29, 63,255,255,255,	//	7
	 20, 22, 29, 29, 29, 29, 29, 29, 63,255,255,255,	//	8
	 30, 63, 63, 63, 63, 63, 63, 63, 63,255,255,255		//	9
};

// vi: set ts=4:

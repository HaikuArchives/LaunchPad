// $Id$

#include <be/app/Application.h>
#include "LaunchPadWindow.h"
#include "WindowTab.h"

LaunchPadWindow::LaunchPadWindow( BRect frame, const char* title )
:	BWindow( frame, title, B_BORDERED_WINDOW_LOOK, B_NORMAL_WINDOW_FEEL, 0 )
{
	frame.OffsetTo( B_ORIGIN );
	WindowTab* tab = new WindowTab(
								frame,
								"WindowDragger",
								B_FOLLOW_NONE,
								B_WILL_DRAW
								);
	AddChild( tab );
}

LaunchPadWindow::~LaunchPadWindow()
{
}

bool
LaunchPadWindow::QuitRequested( void )
{
	be_app->PostMessage( B_QUIT_REQUESTED );
	return true;
}

// vi: set ts=4:

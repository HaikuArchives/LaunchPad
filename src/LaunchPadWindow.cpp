// $Id$

#include "LaunchPadWindow.h"
#include <be/app/Application.h>

LaunchPadWindow::LaunchPadWindow( BRect frame, const char* title )
:	BWindow( frame, title, B_BORDERED_WINDOW_LOOK, B_NORMAL_WINDOW_FEEL, 0 )
{
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

// $Id$

#include "config.h"
#include "LaunchPadApp.h"
#include "LaunchPadWindow.h"

const char* LAUNCHPAD_SIGNATURE = "application/x-vnd.HK-LaunchPad2";

LaunchPadApp::LaunchPadApp()
:	BApplication( LAUNCHPAD_SIGNATURE )
{
	LaunchPadWindow* win = new LaunchPadWindow( BRect( 100, 100, 140, 200 ), "LaunchPad" );
	win->Show();
}

// vi: set ts=4:

// $Id$

#include <be/app/Application.h>
#include "LaunchPadWindow.h"
#include "LRowColumn.h"
#include "WindowTab.h"
#include "PaneView.h"

LaunchPadWindow::LaunchPadWindow( BRect frame, const char* title )
:	BWindow(
		frame,
		title,
		B_BORDERED_WINDOW_LOOK,
		B_NORMAL_WINDOW_FEEL,
		B_WILL_ACCEPT_FIRST_CLICK
		) 
{
	frame.OffsetTo( B_ORIGIN );

	// root view is added to the window in the last step.
	LRowColumn* root = new LRowColumn(
								frame,
								"RootRowColumn",
								LRowColumn::COLUMN_LAYOUT,
								true,
								B_FOLLOW_NONE,
								B_WILL_DRAW
								);

	WindowTab* tab = new WindowTab(
								BRect( 0, 0, frame.right, 15 ),
								"WindowDragger",
								WindowTab::HORIZONTAL,
								B_FOLLOW_NONE,
								B_WILL_DRAW
								);
	root->AddChild( tab );

	// blah blah...
	for ( int i = 0; i < 4; i++ )
	{
		PaneView* pane = new PaneView(
									BRect( 0, 0, frame.right, 40 ),
									"Pane",
									B_FOLLOW_NONE,
									B_WILL_DRAW
									);
		root->AddChild( pane );
	}

	// Finally add all the views.
	AddChild( root );
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

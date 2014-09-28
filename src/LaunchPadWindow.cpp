// $Id: LaunchPadWindow.cpp,v 1.4 2000/06/30 20:46:42 hiromasa Exp $

#include "LaunchPadWindow.h"
#include "PaletteWindow.h"
#include "LaunchPadDefs.h"
#include "PrefHandler.h"
#include <be/storage/Path.h>
#include <be/interface/Alert.h>
#include <be/interface/Screen.h>

LaunchPadWindow::LaunchPadWindow(	BRect		frame,
									const char*	name,
									uint32		tabOrientation,
									BMessage*	padSetting	)
:	PaletteWindow( frame, name, tabOrientation ),
	mSaveNeeded( true )
{
	BScreen	screen;
	mScreenRect = screen.Frame();

	BRect	rect( padSetting->FindRect( "Frame" ) );
	rect.OffsetTo( B_ORIGIN );

	mPadView = new LaunchPadView(	rect,
									"PadView",
									padSetting->FindInt32( "PaneLayout" ),
									padSetting,
									B_FOLLOW_NONE,
									B_WILL_DRAW		);
	AddChild( mPadView );
	Show();
}

LaunchPadWindow::~LaunchPadWindow()
{
}

void
LaunchPadWindow::MessageReceived( BMessage* m )
{
	switch ( m->what )
	{
		default:
			PaletteWindow::MessageReceived( m );
			break;
	}
}

bool
LaunchPadWindow::QuitRequested( void )
{
	BAlert*	alert = new BAlert(	"",
								"Do you want savethe settings for this pad?",
								"Don't save", "Save", "Cancel",
								B_WIDTH_AS_USUAL, B_WARNING_ALERT );
	switch ( alert->Go() )
	{
		case 0:	// Don' save
			// The following check should be removed. much better to do it thru
			// atomic operations of static member var within the dtor.
			mSaveNeeded = false;
			if ( be_app->Lock() )
			{
				int32 windowCount = be_app->CountWindows();
				be_app->Unlock();
				PRINT(( "WindowCount = %d\n", windowCount ));
				if ( windowCount == 1 )
				{
					be_app->PostMessage( B_QUIT_REQUESTED );
				}
			}
			else
			{
				BAlert* errorDialog = new BAlert(
											"",
											"Coundn't lock window",
											"", "", "Continue",
											B_WIDTH_AS_USUAL, B_WARNING_ALERT
											);
				errorDialog->Go();
			}
			return true;
			break;
		case 1:	// Save
		{
			BMessage padSetting( kMsgPadSetting );
			if ( GetSetting( &padSetting ) < B_NO_ERROR )
			{
				BAlert*	errorDialog = new BAlert(
											"",
											"Couldn't save setting",
											"", "", "Continue",
											B_WIDTH_AS_USUAL, B_WARNING_ALERT
											);
				errorDialog->Go();
				return false;
			}
			BMessage saveRequest( kMsgSaveSettings );
			saveRequest.AddMessage( "PadSetting", &padSetting );
			be_app->PostMessage( &saveRequest );
			mSaveNeeded = false;
			if ( be_app->Lock() )
			{
				int32 windowCount = be_app->CountWindows();
				be_app->Unlock();
				if ( windowCount == 1 )
				{
					be_app->PostMessage( B_QUIT_REQUESTED );
				}
			}
			else
			{
				BAlert* errorDialog = new BAlert(
											"",
											"Coundn't lock window",
											"", "", "Continue",
											B_WIDTH_AS_USUAL, B_WARNING_ALERT
											);
				errorDialog->Go();
			}
			return true;
			break;
		}
		case 2:	// Cancel
		default:
			break;
	}

	return false;
}

void
LaunchPadWindow::WorkspaceActivated( int32, bool )
{
	SetWorkspaces( B_CURRENT_WORKSPACE );
	RelocateMyselfIfNeeded();
}

void
LaunchPadWindow::RelocateMyselfIfNeeded( void )
{
	BPoint newLeftTop;

	BScreen screen;
	BRect newFrame = screen.Frame();

	newLeftTop.x =
		( Frame().LeftTop().x / mScreenRect.Width() ) * newFrame.Width();
	newLeftTop.y =
		( Frame().LeftTop().y / mScreenRect.Height() ) * newFrame.Height();
	newLeftTop.ConstrainTo( newFrame );
	MoveTo( newLeftTop );

	mScreenRect = newFrame;
}

//-------------------------------------------------------------------------
// GetSetting
// Inspect the current state of the window and record it in
// padSetting.
//-------------------------------------------------------------------------
status_t
LaunchPadWindow::GetSetting( BMessage* padSetting )
{
	if ( Lock() )
	{
		padSetting->AddRect( "Frame", Frame() );
		padSetting->AddInt32( "NumberOfPanes", mPadView->CountPanes() );
		padSetting->AddInt32( "PaneAppearance", mPadView->PaneAppearance() );
		padSetting->AddInt32( "PaneLayout", mPadView->PaneArrangement() );
		for ( int i = 0; i < mPadView->CountPanes(); i++ )
		{
			BMessage	paneSetting;
			DockPane*	pane = mPadView->PaneAt(i);
			if ( pane->HasRef() )
			{
				paneSetting.AddInt32( "PaneContentType", kPaneTypeEntryRef );

#ifdef EVIL_PREF_FORMAT_COMPATIBILITY
				entry_ref	ref;
				pane->GetRef( &ref );
				paneSetting.AddRef( "EntryRefs", &ref );
#endif

				BPath path;
				pane->GetPath( &path );
				paneSetting.AddString( "Paths", path.Path() );
			}
			else
			{
				paneSetting.AddInt32( "PaneContentType", kPaneTypeEmpty );
			}
#if DEBUG
			paneSetting.PrintToStream();
#endif
			padSetting->AddMessage( "PaneSetting", &paneSetting );
		}
		Unlock();
		return B_NO_ERROR;
	}
	else
	{
		BAlert*	errorDialog = new BAlert(	"",
											"Couldn't lock window",
											"", "", "Continue",
											B_WIDTH_AS_USUAL, B_STOP_ALERT );
		errorDialog->Go();
		return B_ERROR;
	}
}

// vi: set ts=4:

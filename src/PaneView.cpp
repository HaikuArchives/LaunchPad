// $Id$

#define DEBUG 1
#include <be/interface/PopUpMenu.h>
#include <be/interface/MenuItem.h>
#include <be/interface/Window.h>
#include "PaneView.h"
#include "DockItem.h"
#include "EntryItem.h"
#include "LThread.h"

#if DEBUG
#include <be/support/Debug.h>
#include <string.h>
#define REPORT_ERROR(a) fprintf( stderr, "ERROR: %s at line %d : %s\n", __FILE__, __LINE__, strerror(a) )
#else
static inline void REPORT_ERROR( int a ) {}
#endif

PaneView::PaneView(
				BRect		frame,
				const char*	name,
				uint32		resizingMode,
				uint32		flags
				)
:	BView( frame, name, resizingMode, flags ),
	mItem( NULL ),
	mClicks( 0 ),
	mLastTimeClicked( 0 ),
	mMouseWatcherThread( new LThread( "MouseWatcher" ) ),
	mHilite( false )
{
}

PaneView::~PaneView()
{
	if ( mItem ) delete mItem;
	if ( mMouseWatcherThread ) delete mMouseWatcherThread;
}

void
PaneView::AttachedToWindow( void )
{
	BView::AttachedToWindow();
	SetViewColor( tint_color( ui_color( B_PANEL_BACKGROUND_COLOR ), B_LIGHTEN_2_TINT ) );
}

void
PaneView::Draw( BRect updateRect )
{
	PushState();

	// Background
	rgb_color light = tint_color( ViewColor(), B_LIGHTEN_MAX_TINT );
	rgb_color dark  = tint_color( ViewColor(), B_DARKEN_1_TINT );

	BRect bounds( Bounds() );

	SetHighColor( dark );
	StrokeRect( bounds );

	SetHighColor( light );
	MovePenTo( 0, bounds.bottom );
	StrokeLine( B_ORIGIN );
	StrokeLine( BPoint( bounds.right, 0 ) );

	// Content
	if ( mItem )
	{
		const BBitmap* bitmap;

		// FIXME: large or small?
		bitmap = mItem->Bitmap( DockItem::LARGE, mHilite );
		if ( bitmap )
		{
			SetDrawingMode( B_OP_OVER );
			MovePenTo( 5, 5 );
			DrawBitmap( bitmap );
		}
	}

	PopState();
}

void
PaneView::MessageReceived( BMessage* message )
{
	switch ( message->what )
	{
	case B_SIMPLE_DATA:
		{
			// FIXME: need to handle multiple refs
			status_t err;
			entry_ref ref;
			if ( message->FindRef( "refs", &ref ) == B_OK )
			{
				PRINT(( "gotcha!\n" ));
				if ( ( err = SetItem( new EntryItem( &ref ) ) ) < B_NO_ERROR )
				{
					REPORT_ERROR( err );
				}
				else
				{
					Invalidate();
				}
			}
		}
		break;
	default:
		BView::MessageReceived( message );
		break;
	}
}

void
PaneView::MouseDown( BPoint point )
{
	ConvertToScreen( &point );

	// what button is down?
	BMessage* currentMsg = Window()->CurrentMessage();
	uint32 buttons = currentMsg->FindInt32( "buttons" );
	currentMsg->FindInt64( "when", &mLastTimeClicked );

	Hilite( true );

	if ( buttons & B_SECONDARY_MOUSE_BUTTON )
	{
		mClicks = 0;
		LetPopUpGo( point );
	}
	else if ( buttons & B_PRIMARY_MOUSE_BUTTON )
	{
		// remember the number of clicks, used when the button is up.
		// clicks field in the message is not very convenient.
		++mClicks;

		// spawn a thread to track the mousedown state.
		mMouseWatcherThread->Create( _MouseWatcher, this );
		mMouseWatcherThread->Resume();
	}
	else
	{
		mClicks = 0;
		return;
	}
}

void
PaneView::MouseUp( BPoint point )
{
	Hilite( false );

	mMouseWatcherThread->Kill();
}

void
PaneView::LetPopUpGo( BPoint point )
{
	BPopUpMenu* popUp = new BPopUpMenu( "PopUp" );
	popUp->SetFont( be_plain_font );

	BuildPopUp( popUp );

	BRect clickToOpenRect( point.x-5, point.y-5, point.x+5, point.y+5 );
	popUp->Go( point, true, true, clickToOpenRect, true );
}

void
PaneView::BuildPopUp( BPopUpMenu* popUp )
{
	status_t err;

	// Item specific menu items.
	if ( ( err = mItem->BuildMenu( popUp ) ) < B_NO_ERROR )
	{
		REPORT_ERROR( err );
	}

	// Common menu items.
	if ( popUp->CountItems() > 0 )
	{
		popUp->AddSeparatorItem();
	}
	popUp->AddItem( new BMenuItem( "Test", NULL ) );
	popUp->AddItem( new BMenuItem( "Test2", NULL ) );
}

status_t
PaneView::SetItem( DockItem* item )
{
	if ( Window() ) LockLooper();

	if ( mItem )
	{
		delete mItem;
		mItem = NULL;
	}
	mItem = item;

	if ( Window() ) UnlockLooper();

	return B_NO_ERROR;
}

void
PaneView::Hilite( bool hilite )
{
	mHilite = hilite;
	Invalidate();
}

//
// Thread to watch for the mouse down state.
//

status_t
PaneView::_MouseWatcher( void* arg )
{
	return ((PaneView*)arg)->MouseWatcher();
}

status_t
PaneView::MouseWatcher( void )
{
	bigtime_t clickSpeed;
	get_click_speed( &clickSpeed );
	snooze( 2 * clickSpeed );

	// is the button still down?
	if ( LockLooper() )
	{
		BPoint point;
		uint32 buttons;
		GetMouse( &point, &buttons, true );
		if ( buttons & B_PRIMARY_MOUSE_BUTTON )
		{
			ConvertToScreen( &point );
			LetPopUpGo( point );
		}

		UnlockLooper();
		return B_NO_ERROR;
	}
	else
	{
		return B_ERROR;
	}
}

// vi: set ts=4:

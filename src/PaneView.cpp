// $Id$

#define DEBUG 1
#include <be/interface/PopUpMenu.h>
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
	mMouseWatcherThread( new LThread( "MouseWatcher" ) )
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
		bitmap = mItem->Bitmap( DockItem::LARGE );
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

	// click count should be set only for the primary mouse button.
	mClicks = 0;

	// what button is down, and how many clicks?
	BMessage* currentMsg = Window()->CurrentMessage();
	uint32 buttons = currentMsg->FindInt32( "buttons" );
	uint32 clicks = currentMsg->FindInt32( "clicks" );
	currentMsg->FindInt64( "when", &mLastTimeClicked );

	if ( buttons & B_SECONDARY_MOUSE_BUTTON )
	{
		LetPopUpGo( point );
	}
	else if ( buttons & B_PRIMARY_MOUSE_BUTTON )
	{
		// remember the number of clicks, used when the button is up.
		mClicks = clicks;

		// spawn a thread to track the mousedown state.
		mMouseWatcherThread->Create( _MouseWatcher, this );
		mMouseWatcherThread->Resume();
	}
	else
	{
		return;
	}
}

void
PaneView::MouseUp( BPoint point )
{
	printf( "%ld clicks\n", mClicks );
	mMouseWatcherThread->Kill();
}

void
PaneView::LetPopUpGo( BPoint point )
{
	BPopUpMenu* popUp = new BPopUpMenu( "PopUp" );
	BRect clickToOpenRect( point.x-5, point.y-5, point.x+5, point.y+5 );
	popUp->Go( point, true, true, clickToOpenRect, true );
}

status_t
PaneView::SetItem( DockItem* item )
{
	LockLooper();

	if ( mItem )
	{
		delete mItem;
		mItem = NULL;
	}
	mItem = item;

	UnlockLooper();

	return B_NO_ERROR;
}

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

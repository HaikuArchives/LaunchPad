// $Id$

#define DEBUG 1
#include "PaneView.h"
#include "DockItem.h"
#include "EntryItem.h"

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
	mItem( NULL )
{
}

PaneView::~PaneView()
{
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

// vi: set ts=4:

// $Id$

#define DEBUG 1
#include <be/support/Debug.h>
#include <be/interface/Window.h>
#include "WindowDragger.h"

#if DEBUG
#include <string.h>
#define REPORT_ERROR(a) fprintf( stderr, "ERROR: %s at line %d : %s\n", __FILE__, __LINE__, strerror(a) )
#else
static inline void REPORT_ERROR( int a ) {}
#endif

WindowDragger::WindowDragger(
							BRect		frame,
							const char*	name,
							uint32		resizingMode,
							uint32		flags
							)
:	BView( frame, name, resizingMode, flags ),
	mIsDragging( false ),
	mHotSpot( B_ORIGIN )
{
}

WindowDragger::~WindowDragger()
{
}

void
WindowDragger::MouseDown( BPoint point )
{
	if ( mIsDragging ) return;

	// Activate the window, since it's receiving the B_MOUSE_DOWN event,
	// it's safe to assume Window() is valid. (right?)
	Window()->Activate( true );

	// Start receiving mouse events.
	status_t err = SetMouseEventMask( B_POINTER_EVENTS, B_NO_POINTER_HISTORY );
	if ( err < B_NO_ERROR )
	{
		REPORT_ERROR( err );
	}
	else
	{
		mHotSpot = point;
		mIsDragging = true;
	}
}

void
WindowDragger::MouseMoved(
						BPoint			point,
						uint32			transit,
						const BMessage*	message
						)
{
	if ( !mIsDragging ) return;

	// Compute the new top-left corner of the window.
	BPoint pt = ConvertToScreen( point );
	pt.x -= mHotSpot.x;
	pt.y -= mHotSpot.y;
	Window()->MoveTo( pt );

	// This Sync is very important, otherwise the above BWindow::MoveTo don't
	// get sent to app_server and executed right away, causing the subsequent
	// BView::ConvertToScreen to use the wrong value for the window's framerect.
	Sync();
}

void
WindowDragger::MouseUp( BPoint point )
{
	// Turn off the window dragging switch.
	mIsDragging = false;
	mHotSpot = B_ORIGIN;
}

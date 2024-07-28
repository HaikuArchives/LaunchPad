//
// LClickFilter.cpp
//

#include "LClickFilter.h"
#include <View.h>	// for B_PRIMARY_MOUSE_BUTTON, etc...

#define PRINT

const uint32 LClickFilter::kDrag			= 'Drag';
const uint32 LClickFilter::kDoubleClick		= 'DClk';
const uint32 LClickFilter::kSingleClick		= 'SClk';
const uint32 LClickFilter::kSingleClickHold	= 'SClH';

LClickFilter::LClickFilter()
:	BMessageFilter( B_ANY_DELIVERY, B_ANY_SOURCE, (filter_hook)NULL ),
	mAssociatedTarget( NULL ),
	mLastMouseDown( 0 ),
	mLastMouseButton( 0 )
{
}

LClickFilter::~LClickFilter()
{
}

filter_result
LClickFilter::Filter( BMessage* message, BHandler** target )
{
	BView*	view = dynamic_cast<BView*>( *target );
	if ( !view ) {
		return B_DISPATCH_MESSAGE;
	}

	if ( mAssociatedTarget != NULL && mAssociatedTarget == *target ) {
		if ( message->what == B_MOUSE_DOWN ) {
			PRINT(( "B_MOUSE_DOWN\n" ));
			uint32		buttons = 0;
			bigtime_t	clickSpeed;
			get_click_speed( &clickSpeed );
			buttons = (uint32)message->FindInt32( "buttons" );
			if ( buttons == B_PRIMARY_MOUSE_BUTTON ) {
				// First check for double clicks...
				bigtime_t	when;
				message->FindInt64( "when", &when );
				if ( (when - mLastMouseDown) <= clickSpeed &&
					mLastMouseButton == B_PRIMARY_MOUSE_BUTTON ) {
					PRINT(( "Double click\n" ));
					message->what = kDoubleClick;
					mAssociatedTarget = NULL;
					return B_DISPATCH_MESSAGE;
				}
				mLastMouseDown = when;

				// Then check for single click
				BPoint	lastPoint;
				message->FindPoint( "where", &lastPoint );
				BPoint	p( lastPoint );
				while ( buttons ) {
					if ( p != lastPoint ) {
						message->what = kDrag;
						mAssociatedTarget = NULL;
						return B_DISPATCH_MESSAGE;
					} else {
						if ( (system_time() - mLastMouseDown) > 2 * clickSpeed ) {
							message->what = kSingleClickHold;
							mAssociatedTarget = NULL;
							return B_DISPATCH_MESSAGE;
						}
					}
					snooze( 20 * 1000 );
					view->GetMouse( &p, &buttons, true );
				}
				if ( (system_time() - mLastMouseDown) < clickSpeed ) {
					PRINT(( "SingleClick\n" ));
					message->what = kSingleClick;
					mAssociatedTarget = NULL;
					return B_DISPATCH_MESSAGE;
				}
			}

			mAssociatedTarget = NULL;
		}
	} else {
		if ( message->what == B_MOUSE_DOWN ) {
			PRINT(( "B_MOUSE_DOWN 1\n" ));
			mLastMouseButton = (uint32)message->FindInt32( "buttons" );
			message->FindInt64( "when", (int64*)&mLastMouseDown );
			mAssociatedTarget = *target;
		}
	}

	return B_DISPATCH_MESSAGE;
}

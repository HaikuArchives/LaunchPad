// $Id$

#include <assert.h>
#define DEBUG 1
#include <be/support/Debug.h>
#include <be/interface/Bitmap.h>
#include "TriStateButton.h"

#if DEBUG
#include <string.h>
#define REPORT_ERROR(a) fprintf( stderr, "ERROR: %s at line %d : %s\n", __FILE__, __LINE__, strerror(a) )
#else
static inline void REPORT_ERROR( int a ) {}
#endif

TriStateButton::TriStateButton(
							BRect		frame,
							const char*	name,
							BBitmap*	offBitmap,
							BBitmap*	onBitmap,
							BBitmap*	glowBitmap,
							bool		ownsBitmaps,
							BMessage*	message
							)
:	BControl( frame, name, name, message, B_FOLLOW_NONE, B_WILL_DRAW ),
	mOffBitmap( offBitmap ),
	mOnBitmap( onBitmap ),
	mGlowBitmap( glowBitmap ),
	mOwnsBitmaps( ownsBitmaps ),
	mState( TriStateButton::OFF ),
	mMouseDown( false )
{
	// Resize myself based on the size of mOffBitmap
	if ( mOffBitmap )
	{
		BRect bounds = mOffBitmap->Bounds();
		ResizeTo( bounds.Width(), bounds.Height() );
	}
}

TriStateButton::~TriStateButton()
{
	if ( mOwnsBitmaps )
	{
		if( mOnBitmap   ) { delete mOnBitmap;   mOnBitmap   = NULL; }
		if( mOffBitmap  ) { delete mOffBitmap;  mOffBitmap  = NULL; }
		if( mGlowBitmap ) { delete mGlowBitmap; mGlowBitmap = NULL; }
	}
}

void
TriStateButton::AttachedToWindow( void )
{
}

void
TriStateButton::Draw( BRect updateRect )
{
	switch ( mState )
	{
	case TriStateButton::OFF:
		if ( mOffBitmap )
		{
			DrawBitmap( mOffBitmap );
		}
		break;
	case TriStateButton::ON:
		if ( mOnBitmap )
		{
			DrawBitmap( mOnBitmap );
		}
		else
		{
			FillRect( Bounds() );
		}
		break;
	case TriStateButton::GLOW:
		if ( mGlowBitmap )
		{
			DrawBitmap( mOnBitmap );
		}
		break;
	default:
		break;
	}
}

void
TriStateButton::MouseDown( BPoint point )
{
	// We need to keep track of the mouse events happening outside our
	// bound rect, to keep us notified of mouseup event outside the bound
	// rect. See the comment below too.
	status_t err = SetMouseEventMask( B_POINTER_EVENTS );
	if ( err < B_NO_ERROR )
	{
		REPORT_ERROR( err );
		return;
	}

	// MouseDown state must be remembered to handle the case where the user
	// moved the pointer outside the boundary and then back inside again
	// while pressing the button.
	mMouseDown = true;

	mState = TriStateButton::ON;
	Invalidate();
}

void
TriStateButton::MouseMoved(
						BPoint			point,
						uint32			transit,
						const BMessage*	message
						)
{
	if ( transit == B_EXITED_VIEW )
	{
		mState = TriStateButton::OFF;
		Invalidate();
	}
	else if ( transit == B_ENTERED_VIEW ||
			( transit == B_INSIDE_VIEW && mState == TriStateButton::OFF ) )
	{
		if ( mMouseDown )
		{
			mState = TriStateButton::ON;
			Invalidate();
		}
		else
		{
			mState = TriStateButton::GLOW;
			Invalidate();
		}
	}
}

void
TriStateButton::MouseUp( BPoint point )
{
	mMouseDown = false;

	if ( mState == TriStateButton::ON )
	{
		Invoke();
		mState = TriStateButton::OFF;
		Invalidate();
	}
}

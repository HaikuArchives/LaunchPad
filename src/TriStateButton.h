// $Id$
#ifndef _TRI_STATE_BUTTON_H
#define _TRI_STATE_BUTTON_H

#include <be/interface/Control.h>

class TriStateButton : public BControl
{
public:
						TriStateButton(
									BRect		frame,
									const char*	name,
									BBitmap*	offBitmap,	// required
									BBitmap*	onBitmap,	// can be NULL
									BBitmap*	glowBitmap,	// can be NULL
									bool		ownsBitmaps,
									BMessage*	message
									);
	virtual				~TriStateButton();
	virtual void		AttachedToWindow( void );
	virtual void		Draw( BRect updateRect );
	virtual void		MouseDown( BPoint point );
	virtual void		MouseMoved(
									BPoint			point,
									uint32			transit,
									const BMessage*	message
									);
	virtual void		MouseUp( BPoint point );

	enum button_state { OFF, ON, GLOW };

protected:

private:
	BBitmap*			mOffBitmap;
	BBitmap*			mOnBitmap;
	BBitmap*			mGlowBitmap;
	bool				mOwnsBitmaps;
	button_state		mState;
	bool				mMouseDown;
};

#endif // _TRI_STATE_BUTTON_H

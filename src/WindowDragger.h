// $Id$
#ifndef _WINDOW_DRAGGER_H
#define _WINDOW_DRAGGER_H

#include <be/interface/View.h>

class WindowDragger : public BView
{
public:
						WindowDragger(
									BRect		frame,
									const char*	name,
									uint32		resizingMode,
									uint32		flags
									);
	virtual				~WindowDragger();
	virtual void		MouseDown( BPoint point );
	virtual void		MouseMoved(
									BPoint			point,
									uint32			transit,
									const BMessage*	message
									);
	virtual void		MouseUp( BPoint point );

protected:

private:
	bool				mIsDragging;
	BPoint				mHotSpot;
};

// vi: set ts=4:
#endif // _WINDOW_DRAGGER_H

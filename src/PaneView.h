// $Id$
#ifndef _PANE_VIEW_H
#define _PANE_VIEW_H

#include <be/interface/View.h>

class DockItem;
class LThread;

class PaneView : public BView
{
public:
							PaneView(
									BRect		frame,
									const char*	name,
									uint32		resizingMode,
									uint32		flags
									);
	virtual					~PaneView();
	virtual void			AttachedToWindow( void );
	virtual void			Draw( BRect updateRect );
	virtual void			MessageReceived( BMessage* message );
	virtual void			MouseDown( BPoint point );
	virtual void			MouseUp( BPoint point );

	status_t				SetItem( DockItem* item );
	void					Hilite( bool hilite );

protected:

private:
	void					BuildPopUp( BPopUpMenu* popUp );
	void					LetPopUpGo( BPoint point ); // in screen coord
	static status_t			_MouseWatcher( void* arg );
	status_t				MouseWatcher( void );

	DockItem*				mItem;
	uint32					mClicks;
	bigtime_t				mLastTimeClicked;
	LThread*				mMouseWatcherThread;
	bool					mHilite;
};

// vi: set ts=4:
#endif // _PANE_VIEW_H

// $Id$
#ifndef _PANE_VIEW_H
#define _PANE_VIEW_H

#include <be/interface/View.h>

class DockItem;

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
	status_t				SetItem( DockItem* item );

protected:

private:
	DockItem*				mItem;
};

// vi: set ts=4:
#endif // _PANE_VIEW_H

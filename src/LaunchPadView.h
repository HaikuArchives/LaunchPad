// $Id$
// LaunchPadView.h
//
#ifndef _LAUNCH_PAD_VIEW_H
#define _LAUNCH_PAD_VIEW_H

#include "LaunchPadDefs.h"
#include "GUIRowColumn.h"
#include "DockPane.h"
#include <Dragger.h>

class LaunchPadView : public GUIRowColumn
{
public:
					LaunchPadView(	BRect		frame,
									const char*	name,
									uint32		orientation,
									BMessage*	padSetting,
									uint32		resizingMode,
									uint32		flags		);
	virtual			~LaunchPadView();
	virtual void	AttachedToWindow( void );
	virtual void	Draw( BRect r );
	virtual void	MessageReceived( BMessage* m );
	DockPane*		InsertPaneAt( DockPane* siblingPane = NULL );
	void			RemovePane( DockPane* pane );
	void			Refresh( void );
	void			SetPaneArrangement( uint32 arrangement );
	void			SetPaneAppearance( uint32 paneAppearance );
	void			BuildPopUpMenu( BPopUpMenu* popUp );
	const uint32	PaneAppearance( void ) const
						{ return mPaneAppearance; }
	const uint32	PaneArrangement( void ) const
						{ return Orientation(); }
	const uint32	DisplayMode( void ) const
						{ return PaneAppearance(); }
	const uint32	CountPanes( void ) const
						{ return mDockHolder->CountChildren(); }
	DockPane*		PaneAt( uint32 index ) const
						{ return (DockPane*)mDockHolder->ChildAt(index); }

protected:

private:
	BDragger*		mDragger;
	GUIRowColumn*	mDockHolder;
	uint32			mPaneAppearance;
	int64			mLastTimeClicked;
};

#endif // _LAUNCH_PAD_VIEW_H

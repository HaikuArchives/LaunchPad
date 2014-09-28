// $Id: DockPane.h,v 1.6 2000/06/28 02:26:24 hiromasa Exp $
#ifndef _DOCK_PANE_H
#define _DOCK_PANE_H

#include <Control.h>
#include <StorageKit.h>
#include <PopUpMenu.h>
#include "DockItem.h"
#include "LaunchPadDefs.h"

const float kBitmapPaddingL = 5;
const float kBitmapPaddingS = 5;

class BPath;

class DockPane : public BControl
{
public:
						DockPane(	BPoint	p,
									uint32	paneAppearance
											= kLargeIconWithCaption );
						DockPane( BMessage* archive );
						~DockPane();
	virtual void		InitObject( void );
	virtual void		AttachedToWindow( void );
	virtual void		Draw( BRect r );
	virtual void		GetPreferredSize( float *w, float *h );
	virtual void		MessageReceived( BMessage *m );
	virtual void		MouseDown( BPoint p );
	virtual void		MouseMoved( BPoint p, uint32 code, const BMessage *m );
	virtual void		MouseUp( BPoint p );

	void				DroppedToOtherPane( void );	// called from peer panes.
	void				SetTo( const entry_ref* ref );
	void				SetTo( const char* ref );
	void				GetPath( BPath* path );
	void				GetRef( entry_ref *ref ) const;
	bool				HasRef( void );
	const char*			Caption( void ) const
							{ return mDockItem->Caption(); }
	status_t			SetPaneAppearance( uint32 paneAppearance );
	const uint32		PaneAppearance( void ) const
							{ return mPaneAppearance; }
	DockItem*			Item( void )
							{ return mDockItem; }
	const BBitmap*		Icon( void );
	// ----------- obsolete ----------------
	status_t			SetDisplayMode( uint32 displayMode )
							{ return SetPaneAppearance( displayMode ); }
	const uint32		DisplayMode( void ) const
							{ return PaneAppearance(); }

protected:

private:
	void				DoubleClicked();
	void				Launch( BMessage* droppedMsg = NULL );
	void				OpenFolder( void );
	void				ClearDockPane( void );
	void				SetWillAccept( bool flag );
	bool				WillAccept( void )
							{ return mWillAccept; }
	void				HighlightBitmap( bool flag );
	void				HighlightPane( bool flag );
	void				PopUpGo( BPoint p );
	void				PopUpForFolder( BPopUpMenu *popUp );
	void				PopUpForFile( BPopUpMenu *popUp );
	void				StartMouseWatchDog( BPoint p );
	void				StopMouseWatchDog( void );

	DockItem*			mDockItem;
	rgb_color			mBgColor;
	rgb_color			mFgColor;
	bool				mPaneHighlight;
	bool				mBitmapHighlight;
	bool				mWillAccept;
	int64				mLastTimeClicked;
	BRect				mBitmapRect;
	uint32				mPaneAppearance;
	static bool			mEnableSingleClick;
	bool				mTracking;
	BPoint				mHotSpot;
	int32				mClicks;
	BMessageRunner*		mMouseWatchDog;
};

// vi: set ts=4:
#endif // _DOCK_PANE_H

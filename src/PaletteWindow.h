// $Id: PaletteWindow.h,v 1.3 2000/07/01 03:02:49 hiromasa Exp $
// Modified by Hiromasa Kato. Original version by Steve Sprang.
#ifndef _PALETTE_WINDOW_H
#define _PALETTE_WINDOW_H

#include <be/interface/Window.h>
#include <be/interface/View.h>
#include <be/interface/Bitmap.h>
#include <be/interface/Control.h>

enum {
	kMsgWindowTabDoubleClicked	= 'WTDC',
	kMsgSetTabOrientation		= 'STOr',
	kMsgResizeWindow			= 'RszW'
};

class CloseButton : public BView
{
public:
						CloseButton( BRect r );
	virtual				~CloseButton();
	virtual void 		AttachedToWindow( void );
	virtual void 		Draw( BRect r );
	virtual void 		MouseDown( BPoint p );

private:
	BBitmap*			mButton;
	BBitmap*			mButtonPressed;
	bool				mIsPressed;
};

class WindowTab : public BControl
{
public:
						WindowTab(	BRect	r,
									uint32	mode = B_HORIZONTAL,
									BView*	view = NULL );
						WindowTab(	BPoint	leftTop,
									uint32	mode = B_HORIZONTAL,
									BView*	view = NULL	);
	virtual				~WindowTab();
	virtual void		AttachedToWindow( void );
	virtual void		Draw( BRect r );
	virtual void		GetPreferredSize( float *w, float *h );
	virtual void		MouseDown( BPoint p );
	virtual void		MouseMoved(	BPoint			point,
									uint32			transit,
									const BMessage*	message );
	virtual void		MouseUp( BPoint p );

	void				SetTargetView( BView *view );
	void				SetMode( uint32 mode )
							{ SetOrientation( mode ); }
	void				SetOrientation( uint32 orientation );
	void				GetTargetViewOrigin( BPoint *p );
	uint32				Orientation( void ) const
							{ return mOrientation; }

private:
	void				InitObject( void );
	static int32		_DragEntry( void *arg );
	int32				DragWindow( void );
	void				DoubleClicked( BPoint p );

	uint32				mOrientation;
	BView*				mTargetView;
	CloseButton*		mCloseButton;
	BPoint				mHotSpot;
	bool				mIsTracking;
};

class PaletteWindow : public BWindow
{
public:
						PaletteWindow(
							BRect		r,
							const char*	name,
							uint32		tabOrientation = B_HORIZONTAL
							);
	virtual				~PaletteWindow();
	virtual bool		QuitRequested( void );
	void				AddChild( BView* aView, BView* sibling = NULL )
							{ mRoot->AddChild( aView, sibling ); }
	bool				RemoveChild( BView* aView)
							{ return mRoot->RemoveChild( aView ); }
	BView*				ChildAt( int32 index ) const
							{ return mRoot->ChildAt( index ); }
	int32				CountChildren( void ) const
							{ return mRoot->CountChildren(); }
	void				ResizeTo( float width, float height );
	void				MessageReceived( BMessage* m );
	void				SetTabOrientation( uint32 tabOrientation );

private:
	WindowTab*			mTab;
	BView*				mRoot;	// The root biew containing window contents
};

#endif // _PALETTE_WINDOW_H

// $Id$
// PaletteWindow.h
// Modified by Hiromasa Kato. Original version by Steve Sprang.
//
#ifndef _PALETTE_WINDOW_H
#define _PALETTE_WINDOW_H

#include <interface/Window.h>
#include <interface/View.h>
#include <interface/Bitmap.h>
#include <interface/Control.h>

// 9x9 bitmap (x word-boundary padding)
static unsigned char kButtonBits[] = {
//	  1   2   3   4   5   6   7   8   9   x   x   x
	 20, 20, 20, 20, 20, 20, 20, 20, 30,255,255,255,	//	1
	 20, 63, 63, 63, 63, 63, 63, 63, 63,255,255,255,	//	2
	 20, 63, 30, 27, 27, 22, 22, 20, 63,255,255,255,	//	3
	 20, 63, 27, 27, 22, 27, 22, 20, 63,255,255,255,	//	4
	 20, 63, 27, 22, 27, 22, 22, 20, 63,255,255,255,	//	5
	 20, 63, 22, 27, 22, 22, 21, 20, 63,255,255,255,	//	6
	 20, 63, 22, 22, 21, 21, 21, 20, 63,255,255,255,	//	7
	 20, 63, 20, 20, 20, 20, 20, 20, 63,255,255,255,	//	8
	 30, 63, 63, 63, 63, 63, 63, 63, 63,255,255,255		//	9
};
/*
0x5d,0x5d,0x5d,0x5d,0x5d,0x5d,0x5d,0x5d,0xf9,0x3f,0x3f,0x3f,0x5d,0xfc,0xfc,
0xfc,0xfc,0xfc,0xfc,0xfc,0xfc,0x3f,0x3f,0x3f,0x5d,0xfc,0xfc,0xe5,0xe5,0xf9,
0xf9,0x5d,0xfc,0x3f,0x3f,0x3f,0x5d,0xfc,0xe5,0xe5,0xf9,0xe5,0xf9,0x5d,0xfc,
0x3f,0x3f,0x3f,0x5d,0xfc,0xe5,0xf9,0xe5,0xf9,0xf9,0x5d,0xfc,0x3f,0x3f,0x3f,
0x5d,0xfc,0xf9,0xe5,0xf9,0xf9,0xbd,0x5d,0xfc,0x3f,0x3f,0x3f,0x5d,0xfc,0xf9,
0xf9,0xbd,0xbd,0xbd,0x5d,0xfc,0x3f,0x3f,0x3f,0x5d,0xfc,0x5d,0x5d,0x5d,0x5d,
0x5d,0x5d,0xfc,0x3f,0x3f,0x3f,0xf9,0xfc,0xfc,0xfc,0xfc,0xfc,0xfc,0xfc,0xfc,
0x3f,0x3f,0x3f,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,
0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,
0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff};
*/

// 9x9 bitmap (x word-boundary padding)
static unsigned char kButtonPressedBits[] = {
//	  1   2   3   4   5   6   7   8   9   x   x   x
	 20, 20, 20, 20, 20, 20, 20, 20, 30,255,255,255,	//	1
	 20, 17, 17, 17, 17, 17, 17, 22, 63,255,255,255,	//	2
	 20, 17, 19, 19, 22, 20, 24, 29, 63,255,255,255,	//	3
	 20, 17, 19, 27, 22, 27, 24, 29, 63,255,255,255,	//	4
	 20, 17, 19, 22, 27, 24, 27, 29, 63,255,255,255,	//	5
	 20, 17, 20, 27, 24, 27, 27, 29, 63,255,255,255,	//	6
	 20, 17, 24, 24, 27, 27, 30, 29, 63,255,255,255,	//	7
	 20, 22, 29, 29, 29, 29, 29, 29, 63,255,255,255,	//	8
	 30, 63, 63, 63, 63, 63, 63, 63, 63,255,255,255	//	9
};
/*
static unsigned char kButtonPressedBits[] = {
0x98,0x98,0x98,0x98,0x98,0x98,0x98,0x98,0x98,0x3f,0x3f,0x3f,0x98,0x98,0x98,
0x98,0x98,0x98,0x98,0x98,0xfc,0x3f,0x3f,0x3f,0x98,0x98,0xbd,0xbd,0xbd,0xf9,
0xf9,0xfc,0xfc,0x3f,0x3f,0x3f,0x98,0x98,0xbd,0xf9,0xf9,0xf9,0xe5,0xfc,0xfc,
0x3f,0x3f,0x3f,0x98,0x98,0xbd,0xf9,0xf9,0xe5,0xe5,0xfc,0xfc,0x3f,0x3f,0x3f,
0x98,0x98,0xf9,0xf9,0xe5,0xe5,0xe5,0xfc,0xfc,0x3f,0x3f,0x3f,0x98,0x98,0xf9,
0xe5,0xe5,0xe5,0xe5,0xfc,0xfc,0x3f,0x3f,0x3f,0x98,0x98,0xfc,0xfc,0xfc,0xfc,
0xfc,0xfc,0xfc,0x3f,0x3f,0x3f,0x98,0xfc,0xfc,0xfc,0xfc,0xfc,0xfc,0xfc,0xfc,
0x3f,0x3f,0x3f,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,
0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,
0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff};
*/

const BRect kButtonRect = BRect(0,0,8,8);

const float kWindowTabThickness = 13;
const rgb_color kWindowTabColor = { 255, 203, 0 };

const rgb_color kWhite = { 255, 255, 255 };
const rgb_color kLightGray = { 238, 238, 238 };
const rgb_color kGray = { 210, 210, 210 };
const rgb_color kDarkGray = { 100, 100, 100 };

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
						WindowTab(	BRect r,
									uint32 mode = B_HORIZONTAL,
									BView *view = NULL );
						WindowTab(	BPoint	leftTop,
									uint32	mode = B_HORIZONTAL,
									BView*	view = NULL	);
	virtual				~WindowTab();
	virtual void		AttachedToWindow( void );
	virtual void		Draw( BRect r );
	virtual void		GetPreferredSize( float *w, float *h );
	virtual void		MouseDown( BPoint p );

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
	BPoint				mDragPoint;
	uint64				mLastTimeClicked;
};

class PaletteWindow : public BWindow
{
public:
						PaletteWindow(	BRect		r,
										const char*	name,
										uint32		tabOrientation = B_HORIZONTAL );
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

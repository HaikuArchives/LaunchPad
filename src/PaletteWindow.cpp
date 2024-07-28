// $Id: PaletteWindow.cpp,v 1.3 2000/07/01 03:02:49 hiromasa Exp $

#include "PaletteWindow.h"

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
	 30, 63, 63, 63, 63, 63, 63, 63, 63,255,255,255		//	9
};

const BRect kButtonRect = BRect(0,0,8,8);

const float kWindowTabThickness = 13;
const rgb_color kWindowTabColor = { 255, 203, 0 };

const rgb_color kWhite = { 255, 255, 255 };
const rgb_color kLightGray = { 238, 238, 238 };
const rgb_color kGray = { 210, 210, 210 };
const rgb_color kDarkGray = { 100, 100, 100 };

// ****************************************************************
//  CloseButton
// ****************************************************************
CloseButton::CloseButton( BRect r )
	: BView( r, "CloseButton", B_FOLLOW_LEFT||B_FOLLOW_TOP, B_WILL_DRAW )
{
	mButton = new BBitmap( kButtonRect, B_COLOR_8_BIT );
	mButton->SetBits( kButtonBits, 144, 0, B_COLOR_8_BIT );
	mButtonPressed = new BBitmap( kButtonRect, B_COLOR_8_BIT );
	mButtonPressed->SetBits( kButtonPressedBits, 144, 0, B_COLOR_8_BIT );

	mIsPressed = false;
}

CloseButton::~CloseButton()
{
}

void
CloseButton::AttachedToWindow( void )
{
	if ( Parent() )
		SetViewColor( Parent()->ViewColor() );
	BView::AttachedToWindow();
}

void
CloseButton::Draw( BRect )
{
	if ( mIsPressed )
		DrawBitmapAsync( mButtonPressed );
	else
		DrawBitmapAsync( mButton );
}

void
CloseButton::MouseDown( BPoint p )
{
	bool	oldState;
	uint32	buttons;

	GetMouse( &p, &buttons, true );
	while ( B_PRIMARY_MOUSE_BUTTON & buttons )
	{
		oldState = mIsPressed;
		if ( Bounds().Contains( p ) )
		{
			mIsPressed = true;
		}
		else
		{
			mIsPressed = false;
		}

		if ( mIsPressed != oldState )
		{
			Draw( Bounds() );
		}
		snooze( 20000 );
		GetMouse( &p, &buttons );
	}

	// mouse is up.
	mIsPressed = false;
	Draw( Bounds() );

	// quit if necessary.
	if ( Bounds().Contains( p ) )
	{
		Window()->PostMessage( B_QUIT_REQUESTED );
	}
}

// ****************************************************************
//  WindowTab
// ****************************************************************
WindowTab::WindowTab( BRect r, uint32 mode, BView *view )
:	BControl( r, "WindowTab", "WindowTab", new BMessage( kMsgWindowTabDoubleClicked ), B_FOLLOW_NONE, B_WILL_DRAW )
{
	SetTargetView( view );
	SetMode( mode );
	InitObject();
}

WindowTab::WindowTab(	BPoint	leftTop,
						uint32	mode,
						BView*	view	)
:	BControl(	BRect( leftTop.x, leftTop.y, leftTop.x, leftTop.y ),
				"WindowTab",
				"WindowTab",
				new BMessage( kMsgWindowTabDoubleClicked ),
				B_FOLLOW_NONE,
				B_WILL_DRAW ),
	mHotSpot( B_ORIGIN ),
	mIsTracking( false )
{
	SetTargetView( view );
	SetMode( mode );
	InitObject();
}

WindowTab::~WindowTab()
{
}

void
WindowTab::InitObject( void )
{
	BRect buttonRect = kButtonRect;
	buttonRect.OffsetBy( 2, 2 );
	mCloseButton = new CloseButton( buttonRect );
	ResizeToPreferred();

	AddChild( mCloseButton );
}

void
WindowTab::AttachedToWindow( void )
{
	SetViewColor( kLightGray );
	ResizeToPreferred();
	BView::AttachedToWindow();
}

void
WindowTab::Draw( BRect )
{
	BRect b = Bounds();
	SetHighColor( kGray );
	MovePenTo( b.LeftBottom() );
	StrokeLine( b.RightBottom() );
	StrokeLine( b.RightTop() );
	SetHighColor( kWhite );
	StrokeLine( b.LeftTop() );
	StrokeLine( b.LeftBottom() );
}

void
WindowTab::GetPreferredSize( float *w, float *h )
{
	if (!Window())
		return;

	if ( Orientation() == B_HORIZONTAL )
	{
		*w = (Window()->Bounds()).Width();
		*h = kWindowTabThickness;
	}
	else
	{
		*w = kWindowTabThickness;
		*h = (Window()->Bounds()).Height();
	}
}

void
WindowTab::MouseDown( BPoint p )
{
	Window()->Activate();

	BMessage*	m = Window()->CurrentMessage();
	uint32		buttons = m->FindInt32( "buttons" );
	uint32		clicks = m->FindInt32( "clicks" );

	if ( buttons == B_PRIMARY_MOUSE_BUTTON )
	{
		if ( clicks == 2 )
		{
			DoubleClicked( p );
		}
		else
		{
			BScreen screen;
			mScreenFrame = screen.Frame();
			mWindowFrame = Window()->Frame();
			SetMouseEventMask( B_POINTER_EVENTS, B_NO_POINTER_HISTORY );
			ConvertToScreen( &p );
			mHotSpot = p - Window()->Frame().LeftTop();
			mIsTracking = true;
		}
	}
}

void
WindowTab::MouseMoved( BPoint p, uint32 transit, const BMessage* message )
{
	if ( !mIsTracking ) return;

	// Compute the new top-left corner of the window.
	BPoint ps = ConvertToScreen( p );
	ps -= mHotSpot;
	SnapWindowPosition( &ps );
	Window()->MoveTo( ps );

	// This Sync is very important, otherwise the above BWindow::MoveTo doesn't
	// get sent to app_server and executed right away, causing the subsequent
	// BView::ConvertToScreen to use the wrong value for the window's framerect.
	Sync();
}

void
WindowTab::MouseUp( BPoint p )
{
	mIsTracking = false;
	mHotSpot = B_ORIGIN;
}

void
WindowTab::DoubleClicked( BPoint )
{
	Window()->Minimize( true );
}

void
WindowTab::SnapWindowPosition( BPoint* p )
{
	const float SNAP_THRESHOLD = 10.0;

	if ( fabsf( mScreenFrame.left - p->x ) < SNAP_THRESHOLD )
	{
		p->x = mScreenFrame.left;
	}
	if ( fabsf( mScreenFrame.right - ( p->x + mWindowFrame.Width() ) )
		 < SNAP_THRESHOLD )
	{
		p->x = mScreenFrame.right - mWindowFrame.Width();
	}
	if ( fabsf( mScreenFrame.top - p->y ) < SNAP_THRESHOLD )
	{
		p->y = mScreenFrame.top;
	}
	if ( fabsf( mScreenFrame.bottom - ( p->y + mWindowFrame.Height() ) )
		 < SNAP_THRESHOLD )
	{
		p->y = mScreenFrame.bottom - mWindowFrame.Height();
	}
}

void
WindowTab::SetTargetView( BView *view )
{
	mTargetView = view;
}

void
WindowTab::SetOrientation( uint32 orientation )
{
	mOrientation = orientation;

	if ( Orientation() == B_HORIZONTAL )
	{
		SetResizingMode( B_FOLLOW_TOP|B_FOLLOW_LEFT_RIGHT );
		ResizeToPreferred();
	}
	else
	{
		SetResizingMode( B_FOLLOW_LEFT|B_FOLLOW_TOP_BOTTOM );
		ResizeToPreferred();
	}
	Invalidate();
}

void
WindowTab::GetTargetViewOrigin( BPoint* p )
{
	if ( Orientation() == B_HORIZONTAL )
	{
		p->Set( Frame().left, Frame().bottom + 1 );
	}
	else
	{
		p->Set( Frame().right + 1, Frame().top );
	}
}

//--------------------------------------------------------------------------
//  PaletteWindow
//--------------------------------------------------------------------------
PaletteWindow::PaletteWindow(	BRect		r,
								const char*	name,
								uint32		tabOrientation )
	: BWindow(	BRect(r.left,r.top,r.right+13,r.bottom),
				name,
				B_BORDERED_WINDOW,
				B_WILL_ACCEPT_FIRST_CLICK|B_NOT_RESIZABLE )
{
	r.OffsetTo( B_ORIGIN );
	BRect contentsRect( r );

	mTab = new WindowTab(	r.LeftTop(),
							tabOrientation );
	if ( tabOrientation == B_HORIZONTAL )
	{
		contentsRect.OffsetBy( 0, kWindowTabThickness+1 );
	}
	else
	{
		contentsRect.OffsetBy( kWindowTabThickness+1, 0 );
	}
	mRoot = new BView(	contentsRect,
						"PaletteRoot",
						B_FOLLOW_NONE,
						0				);

	if ( Lock() )
	{
		BWindow::AddChild( mTab );
		BWindow::AddChild( mRoot );
		Unlock();
	}
}

PaletteWindow::~PaletteWindow()
{
}

bool
PaletteWindow::QuitRequested( void )
{
	return true;
}

void
PaletteWindow::MessageReceived( BMessage* m )
{
	switch ( m->what )
	{
		case kMsgSetTabOrientation:
			uint32 orientation;
			m->FindInt32( "orientation", (int32*)&orientation );
			SetTabOrientation( orientation );
			break;
		case kMsgResizeWindow:
			float width, height;
			m->FindFloat( "width", &width );
			m->FindFloat( "height", &height );
			ResizeTo( width, height );
			break;
		default:
			BWindow::MessageReceived( m );
			break;
	}
}

void
PaletteWindow::ResizeTo( float width, float height )
{
	mRoot->ResizeTo( width, height );
	BPoint rightBottom( mRoot->Frame().RightBottom() );
	BWindow::ResizeTo( rightBottom.x, rightBottom.y );
}

void
PaletteWindow::SetTabOrientation( uint32 tabOrientation )
{
	mTab->SetOrientation( tabOrientation );

	// Relocate the root container view.
	BPoint rootLeftTop;
	mTab->GetTargetViewOrigin( &rootLeftTop );
	mRoot->MoveTo( rootLeftTop );

	// Resize self.
	BPoint rightBottom = mRoot->Frame().RightBottom();
	BWindow::ResizeTo( rightBottom.x, rightBottom.y );
}

// vi: set ts=4:

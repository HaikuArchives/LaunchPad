//
// PaletteWindow.cpp
//

#include "PaletteWindow.h"

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
	while ( B_PRIMARY_MOUSE_BUTTON & buttons ) {
		oldState = mIsPressed;
		if ( Bounds().Contains( p ) ) {
			mIsPressed = true;
		} else {
			mIsPressed = false;
		}

		if ( mIsPressed != oldState ) {
			Draw( Bounds() );
		}
		snooze( 20000 );
		GetMouse( &p, &buttons );
	}

	// mouse is up.
	mIsPressed = false;
	Draw( Bounds() );

	// quit if necessary.
	if ( Bounds().Contains( p ) ) {
		Window()->PostMessage( B_QUIT_REQUESTED );
	}
}

// ****************************************************************
//  WindowTab
// ****************************************************************
WindowTab::WindowTab( BRect r, uint32 mode, BView *view )
	: BControl( r, "WindowTab", "WindowTab", new BMessage(kMsgWindowTabDoubleClicked), B_FOLLOW_NONE, B_WILL_DRAW )
{
	SetTargetView( view );
	SetMode( mode );
	InitObject();
}

WindowTab::WindowTab(	BPoint	leftTop,
						uint32	mode,
						BView*	view	)
	: BControl(	BRect( leftTop.x, leftTop.y, leftTop.x, leftTop.y ),
				"WindowTab",
				"WindowTab",
				new BMessage( kMsgWindowTabDoubleClicked ),
				B_FOLLOW_NONE,
				B_WILL_DRAW										)
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
	mLastTimeClicked = 0;
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
	if ( Orientation() == B_HORIZONTAL ) {
		*w = (Window()->Bounds()).Width();
		*h = kWindowTabThickness;
	} else {
		*w = kWindowTabThickness;
		*h = (Window()->Bounds()).Height();
	}
}

void
WindowTab::MouseDown( BPoint p )
{
	Window()->Activate();

	// double click interval.
	bigtime_t clickSpeed;
	get_click_speed( &clickSpeed );

	// which button is pressed?
	uint32 buttons = 0; 
	BMessage *m = Window()->CurrentMessage();
	m->FindInt32( "buttons", (int32 *)&buttons ); 

	// primary button double clicked?
	if ( buttons == B_PRIMARY_MOUSE_BUTTON ) {
		uint64 when;
		m->FindInt64( "when", (int64 *)&when );
		if ( (when-mLastTimeClicked) <= clickSpeed ) {
			DoubleClicked( p );
			return;
		}
		mLastTimeClicked = when;

		mDragPoint = p;
		thread_id dragThread = spawn_thread( _DragEntry, "", B_NORMAL_PRIORITY, this );
		resume_thread( dragThread );
	}
}

int32
WindowTab::_DragEntry( void *arg )
{
	return ( ((WindowTab *)arg)->DragWindow() );
}

int32
WindowTab::DragWindow( void )
{
	BPoint p;
	uint32 buttons;
	Window()->Lock();
	GetMouse( &p, &buttons, true );
	Window()->Unlock();

	while( B_PRIMARY_MOUSE_BUTTON & buttons ) {
		Window()->Lock();
		ConvertToScreen( &p );
		p -= mDragPoint;
		Window()->MoveTo( p.x, p.y );
		GetMouse( &p, &buttons, true );
		Window()->Unlock();
		snooze( 20000 );
	}

	return B_NO_ERROR;
}

void
WindowTab::DoubleClicked( BPoint )
{
	Window()->Minimize( true );
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

	if ( Orientation() == B_HORIZONTAL ) {
		SetResizingMode( B_FOLLOW_TOP|B_FOLLOW_LEFT_RIGHT );
		ResizeToPreferred();
	} else {
		SetResizingMode( B_FOLLOW_LEFT|B_FOLLOW_TOP_BOTTOM );
		ResizeToPreferred();
	}
	Invalidate();
}

void
WindowTab::GetTargetViewOrigin( BPoint *p )
{
	if ( Orientation() == B_HORIZONTAL ) {
		p->Set( Frame().left, Frame().bottom + 1 );
	} else {
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
	if ( tabOrientation == B_HORIZONTAL ) {
		contentsRect.OffsetBy( 0, kWindowTabThickness+1 );
	} else {
		contentsRect.OffsetBy( kWindowTabThickness+1, 0 );
	}
	mRoot = new BView(	contentsRect,
						"PaletteRoot",
						B_FOLLOW_NONE,
						0				);

	if ( Lock() ) {
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
	switch ( m->what ) {
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

//
// DockPane.cpp
//

#include "DockPane.h"
#include "colors.h"
#include "LaunchPadWindow.h"
#include "LTrackerMenu.h"
#include <Path.h>
#include <Entry.h>
#include <AppFileInfo.h>
#include <PopUpMenu.h>
#include <MenuItem.h>
#include <stdlib.h>
#include <stdio.h>

#define DRAG_BITMAP	0

const char kTrackerSig[] = "application/x-vnd.Be-TRAK";
static void LaunchRef( entry_ref* ref );

DockPane::DockPane( BPoint p, uint32 paneAppearance )
:	BControl(	BRect(p.x,p.y,p.x,p.y),
				"DockPane", "Label",
				new BMessage( kMsgFromDockPane ),
				B_FOLLOW_NONE, B_WILL_DRAW )
{
	SetPaneAppearance( paneAppearance );
	InitObject();
}

DockPane::DockPane( BMessage *archive )
:	BControl( archive )
{
	SetPaneAppearance( archive->FindInt32( "PaneAppearance" ) );
	InitObject();
}

DockPane::~DockPane()
{
	delete mDockItem;
}

void
DockPane::InitObject( void )
{
	PRINT(( "DockPane \"%s\" init..\n", Name() ));

//	mClickFilter = new LClickFilter();
//	AddFilter( mClickFilter );

	mDockItem = new DockItem();
	mBgColor = ltGray;
	mFgColor = black;
	mPaneHighlight = false;
	mBitmapHighlight = false;
	mLastTimeClicked = 0;
	ResizeToPreferred();
}

void
DockPane::AttachedToWindow( void )
{
	ResizeToPreferred();
	SetViewColor( B_TRANSPARENT_32_BIT );
	BView::AttachedToWindow();
}

void
DockPane::Draw( BRect r )
{
	SetHighColor( mBgColor );
	FillRect( r );

	SetHighColor( mFgColor );
	SetLowColor( mBgColor );
	MovePenTo( mBitmapRect.LeftTop() );
	SetDrawingMode( B_OP_OVER );
	switch ( PaneAppearance() ) {
	case kLargeIconWithCaption:
	case kLargeIconWithoutCaption:
		if ( mDockItem->HasRef() ) {
			if ( mBitmapHighlight )
				DrawBitmap( mDockItem->LargeIconHi() );
			else
				DrawBitmap( mDockItem->LargeIcon() );
		}
		break;
	case kMiniIconWithCaption:
	case kMiniIconWithoutCaption:
		if ( mDockItem->HasRef() ) {
			if ( mBitmapHighlight )
				DrawBitmap( mDockItem->SmallIconHi() );
			else
				DrawBitmap( mDockItem->SmallIcon() );
		}
		break;
	}
	SetDrawingMode( B_OP_COPY );
	if ( PaneAppearance() != kLargeIconWithoutCaption &&
			PaneAppearance() != kMiniIconWithoutCaption ) {
		MovePenTo( 0, 50 );
		DrawString( mDockItem->Caption() );
	}
	if ( mPaneHighlight ) {
		SetHighColor( blue );
		StrokeRect( Bounds() );
	} else {
		SetHighColor( mdGray );
		MovePenTo( Bounds().LeftBottom() );
		StrokeLine( Bounds().RightBottom() );
		StrokeLine( Bounds().RightTop() );
		SetHighColor( white );
		StrokeLine( Bounds().LeftTop() );
		StrokeLine( BPoint(Bounds().left, Bounds().bottom-1) );
	}
}

void
DockPane::GetPreferredSize( float *w, float *h )
{
	switch ( PaneAppearance() ) {
	case kLargeIconWithCaption:
		*w = 70;
		*h = 60;
		break;
	case kLargeIconWithoutCaption:
		*w = 32+2*kBitmapPaddingL;
		*h = 32+2*kBitmapPaddingL;
		break;
	case kMiniIconWithCaption:
		*w = 70;
		*h = 30;
		break;
	case kMiniIconWithoutCaption:
		*w = 16+2*kBitmapPaddingS;
		*h = 16+2*kBitmapPaddingS;
		break;
	default:
		*w = 70;
		*h = 60;
		break;
	}

	// Because width and height actually cover (width+1) and (height+1) pixels.
	(*w)-=1;
	(*h)-=1;
}

void
DockPane::MessageReceived( BMessage *m )
{
#if defined(DEBUG)
	puts( class_name( this ) );
	m->PrintToStream();
#endif

	entry_ref	droppedRef;
	entry_ref	paneRef;
	entry_ref	ref;
	DockPane*	source;
	BWindow*	window;

	switch ( m->what ) {
	case kMsgDraggedFromDockPane:
	case B_SIMPLE_DATA:
		if ( m->what == B_SIMPLE_DATA ) {	// uh.. butt ugly.
			m->FindRef( "refs", &droppedRef );
		} else {
			m->FindRef( "Refs", &droppedRef );	// probably not used any more.
		}
		// Dropped onto the bitmap? (then launch it)
		if ( WillAccept() ) {
			Launch( m );
			break;
		}
		// Dragged from other pane?
		if ( m->FindPointer( "SourcePad", (void**)&window ) == B_OK ) {
			if ( window == Window() && m->FindPointer( "SourceDockPane", (void**)&source ) == B_OK ) {
				source->DroppedToOtherPane();
			}
		}
		// Set new item
		mDockItem->SetTo( &droppedRef );
		mPaneHighlight = false;
		Invalidate();
		break;
	case kMsgRemoveItem:
		ClearDockPane();
		break;
	case kMsgLaunchItem:
		Launch();
		break;
	case kMsgLaunchRef:
		m->FindRef( "refs", &ref );
		LaunchRef( &ref );
		break;
	case kMsgRemovePane:	// remove myself!
		m->AddPointer( "SourceDockPane", this );
		Invoke( m );
		break;
	case kMsgInsertPaneAt:	// insert pane after myself.
		m->AddPointer( "SourceDockPane", this );
		Invoke( m );
		break;
	default:
		BView::MessageReceived( m );
		break;
	}
}

void
DockPane::MouseDown( BPoint p )
{
	Window()->Activate();

	HighlightBitmap( true );

	bigtime_t clickSpeed;
	get_click_speed( &clickSpeed );

	uint32 buttons = 0; 
	BMessage *m = Window()->CurrentMessage();
	m->FindInt32( "buttons", (int32 *)&buttons ); 

	if ( buttons == B_PRIMARY_MOUSE_BUTTON ) {
		// Double click?
		int64 when;
		m->FindInt64( "when", &when );
		if ( (when-mLastTimeClicked) <= clickSpeed ) {
			DoubleClicked();
			HighlightBitmap( false );
			return;
		}
		mLastTimeClicked = when;
	}

	BPoint lastPoint( p );

	switch ( buttons ) {
	case B_PRIMARY_MOUSE_BUTTON:
		while ( buttons ) {
			if ( p != lastPoint ) {		// Drag
				entry_ref ref;
				if ( mDockItem->GetRef( &ref ) != B_NO_ERROR ) {
					break; // Invalid ref
				}
				BMessage dragMsg( B_SIMPLE_DATA );
				dragMsg.AddRef( "refs", &ref );
				dragMsg.AddPointer( "SourceDockPane", this );
				dragMsg.AddPointer( "SourcePad", Window() );
#if DRAG_BITMAP
				DragMessage( &dragMsg, (BBitmap*)Icon(), B_OP_ALPHA, BPoint( 0, 0 ) );
#else
				DragMessage( &dragMsg, Bounds() );
#endif
				HighlightBitmap( false );
				return;
			} else {
				if ( (system_time()-mLastTimeClicked) > 2 * clickSpeed ) {
					PopUpGo( p );
					break;
				}
			}
			snooze( 20 * 1000 );
			GetMouse( &p, &buttons, true );
		}
		if ( true &&
			(system_time() - mLastTimeClicked) < clickSpeed ) {
			DoubleClicked();
			HighlightBitmap( false );
		}
		break;
	case B_SECONDARY_MOUSE_BUTTON:
		PopUpGo( p );
		break;
	default:
		break;
	}

	HighlightBitmap( false );
}

void
DockPane::MouseMoved( BPoint, uint32 code, const BMessage *m )
{
	bool bitmapHighlightOld = mBitmapHighlight;
	bool paneHighlightOld = mPaneHighlight;
	DockPane *sourcePane;

	switch ( code ) {
	case B_ENTERED_VIEW:
	case B_INSIDE_VIEW:
		if ( m == NULL )
			break;
		mPaneHighlight = true;
		if ( m->FindPointer( "SourceDockPane", (void**)&sourcePane ) == B_OK && sourcePane == this ) {
			SetWillAccept( false );
			break;
		}
		if ( mDockItem->HasRef() ) {
			SetWillAccept( true );
		} else {
			SetWillAccept( false );
		}
		break;
	case B_EXITED_VIEW:
		if ( !mPaneHighlight && !mBitmapHighlight ) break;
		mPaneHighlight = false;
		mBitmapHighlight = false;
		break;
	default:
		break;
	}

	if ( mBitmapHighlight != bitmapHighlightOld || mPaneHighlight != paneHighlightOld )
		Invalidate();
}

void
DockPane::PopUpGo( BPoint p )
{
	BMenuItem*	item;
	BMenuItem*	selected;
	char		label[40];

	BPopUpMenu popUp( "OnBitmap", false, false );
	popUp.SetFont( be_plain_font );

	// Item-specific pop-up menu items
	if ( mDockItem->HasRef() ) {
	if ( mDockItem->IsFolder() ) {
		PopUpForFolder( &popUp );
	} else {
		PopUpForFile( &popUp );
	}
	}

	// Common items
	if ( mDockItem->HasRef() ) {
		sprintf( label, "Remove %s", mDockItem->Caption() );
		item = new BMenuItem( label, new BMessage( kMsgRemoveItem ) );
		item->SetTarget( this, Window() );
		popUp.AddItem( item );
	} else {
		item = new BMenuItem( "Remove this pane", new BMessage( kMsgRemovePane ) );
		item->SetTarget( this, Window() );
		popUp.AddItem( item );
	}

	item = new BMenuItem( "Insert a pane here", new BMessage( kMsgInsertPaneAt ) );
	item->SetTarget( this, Window() );
	popUp.AddItem( item );

	// Menu added by container object.
	((LaunchPadView*)Target())->BuildPopUpMenu( &popUp );

	// Let go!!
	HighlightBitmap( true );

	ConvertToScreen( &p );
	BRect	clickToOpenRect( p.x-10, p.y-10, p.x+10, p.y+10 );
	snooze( 20*1000 );
	selected = popUp.Go( p, true, true, clickToOpenRect, false );

	HighlightBitmap( false );
}

void
DockPane::PopUpForFolder( BPopUpMenu *popUp )
{
	entry_ref	ref;
	GetRef( &ref );
	BMessage msg( kMsgLaunchRef );
//	::BuildNavMenu( popUp, &ref, &msg, this );
	BMenuItem*	item = LTrackerMenu::BuildMenu( &ref, &msg, this );
	if ( item != NULL ) {
		popUp->AddItem( item );
	}
}

void
DockPane::PopUpForFile( BPopUpMenu *popUp )
{
	BMenuItem*	item;
	char		label[40];

	sprintf( label, "Launch %s", mDockItem->Caption() );
	item = new BMenuItem( label, new BMessage( kMsgLaunchItem ) );
	item->SetTarget( this, Window() );
	popUp->AddItem( item );
}

void
DockPane::DoubleClicked( void )
{
	entry_ref ref;
	if ( mDockItem->GetRef( &ref ) != B_NO_ERROR )
		return; // Invalid ref.

	BNode node( &ref );
	if ( node.InitCheck() != B_NO_ERROR )
		return;

	if ( node.IsFile() ) {
		Launch();
	}
	else if ( node.IsDirectory() ) {
		// Open a folder.
		Launch();
	}
	else if ( node.IsSymLink() ) {
		Launch();
	}
	else {
		PRINT(( "That's nothing, forget it\n" ));
	}
}

void
DockPane::ClearDockPane( void )
{
	delete mDockItem;
	mDockItem = new DockItem();
	Invalidate();
}

void
DockPane::DroppedToOtherPane( void )
{
	ClearDockPane();
}

void
DockPane::Launch( BMessage*			droppedMsg )
{
	if ( mDockItem->IsFolder() ) {
		OpenFolder();
		return;
	}

	entry_ref	ref;
	if ( mDockItem->GetRef( &ref ) != B_NO_ERROR ) {
		return;
	}

	type_code	typeFound;
	int32		countFound;
	if ( droppedMsg != NULL
			&& ( droppedMsg->GetInfo( "refs", &typeFound, &countFound ) ) == B_OK
			&& typeFound == B_REF_TYPE ) {
		BMessage	msg( droppedMsg );
		msg.what = B_REFS_RECEIVED;
		be_roster->Launch( &ref, &msg );
	} else {
		be_roster->Launch( &ref );
	}

	HighlightPane( false );
	HighlightBitmap( false );

	return;
}

void
DockPane::OpenFolder( void )
{
	status_t err;
	BMessenger *tracker = new BMessenger( kTrackerSig, -1, &err );
	if ( err < B_NO_ERROR ) {
		return;
	}

	entry_ref ref;
	if ( mDockItem->GetRef( &ref ) != B_NO_ERROR )
		return;

	BMessage *m = new BMessage( B_REFS_RECEIVED );
	m->AddRef( "refs", &ref );
	tracker->SendMessage( m );
}

status_t
DockPane::SetPaneAppearance( uint32 paneAppearance )
{
	mPaneAppearance = paneAppearance;
	switch ( PaneAppearance() ) {
	case kLargeIconWithCaption:
	case kLargeIconWithoutCaption:
		mBitmapRect.Set( 0, 0, 31, 31 );
		mBitmapRect.OffsetBy( kBitmapPaddingL, kBitmapPaddingL );
		break;
	case kMiniIconWithCaption:
	case kMiniIconWithoutCaption:
		mBitmapRect.Set( 0, 0, 15, 15 );
		mBitmapRect.OffsetBy( kBitmapPaddingS, kBitmapPaddingS );
		break;
	}

	ResizeToPreferred();
	Invalidate();

	return B_OK;
}

void
DockPane::SetWillAccept( bool flag )
{
	mBitmapHighlight = flag;
	mWillAccept = flag;
}

void
DockPane::HighlightBitmap( bool flag )
{
	mBitmapHighlight = flag;
	Draw( Bounds() );
}

void
DockPane::HighlightPane( bool flag )
{
	mPaneHighlight = flag;
	Draw( Bounds() );
}

void
DockPane::SetTo( const entry_ref *ref )
{
	mDockItem->SetTo( ref );
}

void
DockPane::GetRef( entry_ref *ref ) const
{
	mDockItem->GetRef( ref );
}

bool
DockPane::HasRef( void )
{
	return mDockItem->HasRef();
}

const BBitmap*
DockPane::Icon( void )
{
	const BBitmap*	iconBitmap;

	switch ( PaneAppearance() ) {
	case kLargeIconWithCaption:
	case kLargeIconWithoutCaption:
		iconBitmap = Item()->LargeIcon();
		break;
	case kMiniIconWithCaption:
	case kMiniIconWithoutCaption:
		iconBitmap = Item()->SmallIcon();
		break;
	}

	return iconBitmap;
}

void LaunchRef( entry_ref* ref )
{
	BNode	node( ref );
	if ( node.IsDirectory() ) {
		status_t err;
		BMessenger *tracker = new BMessenger( kTrackerSig, -1, &err );
		if ( err == B_NO_ERROR ) {
			BMessage* m = new BMessage( B_REFS_RECEIVED );
			m->AddRef( "refs", ref );
			tracker->SendMessage( m );
		}
	} else {
		be_roster->Launch( ref );
	}
}
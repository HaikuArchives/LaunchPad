//
// LaunchPadView.cpp
//

#include "LaunchPadView.h"
#include "PaletteWindow.h"
#include <MenuItem.h>
#include <Debug.h>

LaunchPadView::LaunchPadView(	BRect		frame,
								const char*	name,
								uint32		orientation,
								BMessage*	padSetting,
								uint32		resizingMode,
								uint32		flags		)
	:	GUIRowColumn( frame, name, orientation, resizingMode, flags ),
		mLastTimeClicked( 0 )
{
#ifdef DEBUG
	padSetting->PrintToStream();
#endif
	frame.OffsetTo( B_ORIGIN );

	mDragger = new BDragger( frame, this, B_FOLLOW_NONE, B_WILL_DRAW );
	mDockHolder = new GUIRowColumn( frame, "DockHolder", orientation, B_FOLLOW_NONE, B_WILL_DRAW );
	AddChild( mDockHolder );

	SetPaneAppearance( padSetting->FindInt32( "PaneAppearance" ) );

	// Build panes. Target for each pane specified in AttachedToWindow.
	DockPane*	newPane;
	entry_ref	ref;
	BMessage	paneSetting;
	for ( int i = 0; i < padSetting->FindInt32( "NumberOfPanes" ); i++ )
	{
		padSetting->FindMessage( "PaneSetting", i, &paneSetting );
		newPane = new DockPane( BPoint(0,0), PaneAppearance() );
		if ( paneSetting.FindInt32( "PaneContentType" ) == kPaneTypeEntryRef )
		{
			const char* path = paneSetting.FindString( "Paths" );
			if ( path )
			{
				PRINT(( "path = %s\n", path ));
				newPane->SetTo( path );
			}
			else
			{
				// for evil brain dead pref format that saves entry_ref.
				paneSetting.FindRef( "EntryRefs", &ref );
				newPane->SetTo( &ref );
			}
		}
		mDockHolder->AddChild( newPane );
	}
	PRINT(( "there're %d panes in mDockHolder\n", mDockHolder->CountChildren() ));
}

LaunchPadView::~LaunchPadView()
{
}

void
LaunchPadView::AttachedToWindow( void )
{
	SetViewColor( B_TRANSPARENT_32_BIT );
	for ( int i = 0; i < mDockHolder->CountChildren(); i++ ) {
		((DockPane*)(mDockHolder->ChildAt(i)))->SetTarget( this, Window() );
	}
	Refresh();

	BView::AttachedToWindow();
}

void
LaunchPadView::MessageReceived( BMessage* m )
{
	DockPane* pane;

#if defined(DEBUG)
	puts( class_name( this ) );
	m->PrintToStream();
#endif

	switch ( m->what ) {
	case kMsgChangeDisplayMode:
		switch ( PaneAppearance() ) {
		case kLargeIconWithCaption:
		case kLargeIconWithoutCaption:
			SetPaneAppearance( kMiniIconWithoutCaption );
			break;
		case kMiniIconWithCaption:
		case kMiniIconWithoutCaption:
			SetPaneAppearance( kLargeIconWithoutCaption );
			break;
		default:
			break;
		}
		break;
	case kMsgInsertPaneAt:
		m->FindPointer( (const char*)"SourceDockPane", (void**)&pane );
		InsertPaneAt( pane );
		break;
	case kMsgRemovePane:
		m->FindPointer( (const char*)"SourceDockPane", (void**)&pane );
		RemovePane( pane );
		break;
	case kMsgChangePaneLayout:
		if ( PaneArrangement() == B_ITEMS_IN_COLUMN ) {
			SetPaneArrangement( B_ITEMS_IN_ROW );
		} else {
			SetPaneArrangement( B_ITEMS_IN_COLUMN );
		}
		break;
	default:
		BView::MessageReceived( m );
		break;
	}
}

void
LaunchPadView::Draw( BRect )
{
}

void
LaunchPadView::BuildPopUpMenu( BPopUpMenu* popUp )
{
	BMenuItem*	item;
	char		label[40];

	// Separator
	item = new BSeparatorItem();
	popUp->AddItem( item );

	// Pane appearance
	if ( PaneAppearance() == kLargeIconWithoutCaption ) {
		sprintf( label, "Mini icons" );
	} else {
		sprintf( label, "Large icons" );
	}
	item = new BMenuItem( label, new BMessage( kMsgChangeDisplayMode ) );
	item->SetTarget( this, Window() );
	popUp->AddItem( item );

	// Pane layout
	if ( PaneArrangement() == B_ITEMS_IN_ROW ) {
		sprintf( label, "Arrange panes horizontally" );
	} else {
		sprintf( label, "Arrange panes vertically" );
	}
	item = new BMenuItem( label, new BMessage( kMsgChangePaneLayout ) );
	item->SetTarget( this, Window() );
	popUp->AddItem( item );

	// New pad
	item = new BMenuItem( "New pad", new BMessage( kMsgCreateNewPad ) );
	item->SetTarget( be_app, be_app );
	popUp->AddItem( item );

	// Separator
	item = new BSeparatorItem();
	popUp->AddItem( item );

	// About box
	item = new BMenuItem( "About LaunchPad" B_UTF8_ELLIPSIS, new BMessage( B_ABOUT_REQUESTED ) );
	item->SetTarget( be_app, be_app );
	popUp->AddItem( item );
}

DockPane*
LaunchPadView::InsertPaneAt( DockPane* siblingPane )
{
	if ( Window()->Lock() ) {
		DockPane *newPane = new DockPane(	BPoint(0,0),
											DisplayMode() );
		newPane->SetTarget( this, Window() );
		mDockHolder->AddChild( newPane, siblingPane );
		Refresh();
#ifdef DEBUG
		printf( "LaunchPadView: Added pane #%d\n", ((BView*)mDockHolder)->CountChildren() );
#endif
		Window()->Unlock();
		return newPane;
	} else {
		return NULL;
	}
}

void
LaunchPadView::RemovePane( DockPane* pane )
{
	if ( mDockHolder->CountChildren() == 1 ) {
		return;
	}

	if ( Window()->Lock() ) {
		if ( mDockHolder->RemoveChild( pane ) == true || true ) {
			// successfully removed
			delete pane;
			Refresh();
		}
		Window()->Unlock();
	}
}

void
LaunchPadView::SetPaneArrangement( uint32 arrangement )
{
	// Resize dock holder
	SetOrientation( arrangement );
	Refresh();
}

void
LaunchPadView::SetPaneAppearance( uint32 paneAppearance )
{
	mPaneAppearance = paneAppearance;
	Refresh();
}

void
LaunchPadView::Refresh( void )
{
	BMessage* msg;

	// Set pane appearance
	for ( int i = 0; i < mDockHolder->CountChildren(); i++ ) {
		((DockPane*)mDockHolder->ChildAt(i))->SetPaneAppearance( PaneAppearance() );
	}

	// Set orientation and at the same time refresh the layout.
	mDockHolder->SetOrientation( PaneArrangement() );

	// Resize myself (GUIRowColumn)
	BPoint rightBottom( mDockHolder->Frame().RightBottom() );
	ResizeTo( rightBottom.x, rightBottom.y );

	// Relocate window tab
	msg = new BMessage( kMsgSetTabOrientation );
	if ( PaneArrangement() == B_ITEMS_IN_ROW ) {
		msg->AddInt32( "orientation", B_HORIZONTAL );
	} else {
		msg->AddInt32( "orientation", B_VERTICAL );
	}
	Window()->PostMessage( msg );

	// Resize window
	msg = new BMessage( kMsgResizeWindow );
	msg->AddFloat( "width", rightBottom.x );
	msg->AddFloat( "height", rightBottom.y );
#if DEBUG
	msg->PrintToStream();
#endif
	Window()->PostMessage( msg );

	delete msg;
}

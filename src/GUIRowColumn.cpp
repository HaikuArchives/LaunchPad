//
// GUIRowColumn.cpp
//

#include "GUIRowColumn.h"
#include <be/app/Message.h>
#include <interface/Window.h>
#include <interface/Menu.h>		// for B_ITEMS_IN_ROW, B_ITEMS_IN_COLUMN

GUIRowColumn::GUIRowColumn(	BRect		frame,
							const char*	name,
							uint32		orientation,
							uint32		resizingMode,
							uint32		flags			)
:	BView( frame, name, resizingMode, flags )
{
	SetOrientation( orientation );
}

GUIRowColumn::GUIRowColumn(	BMessage*	archive )
:	BView( archive )
{
	SetOrientation( (uint32)archive->FindInt32( "orientation" ) );
#ifdef _USE_ADDCHILD_HACK
	// retrieve mChildren..
#endif
}

GUIRowColumn::~GUIRowColumn()
{
}

BArchivable*
GUIRowColumn::Instantiate( BMessage* archive )
{
	if ( !validate_instantiation( archive, "GUIRowColumn" ) ) {
		return NULL;
	}

	return new GUIRowColumn( archive );
}

status_t
GUIRowColumn::Archive( BMessage* archive, bool deep ) const
{
	BView::Archive( archive, deep );
	archive->AddInt32( "orientation", (int32)Orientation() );
#ifdef _USE_ADDCHILD_HACK
	// something must be done to preserve mChildren..
#endif

	return B_NO_ERROR;
}

void
GUIRowColumn::AttachedToWindow( void )
{
	if ( Parent() != NULL ) {
		SetViewColor( Parent()->ViewColor() );
	}
	UpdateLayout();
	BView::AttachedToWindow();
}

void
GUIRowColumn::AddChild( BView* view, BView* siblingView )
{
	// This is an ugly hack to get around the BView::AddChild bug that
	// you can't properly insert BView in front of the first children;
	// i.e. AddChild( someView, ChildAt( 0 ) ) doesn't work right.

#ifdef _USE_ADDCHILD_HACK
	if ( siblingView == NULL ) {
		mChildren.AddItem( view );
	} else {
		mChildren.AddItem( view, mChildren.IndexOf( siblingView ) );
	}
	BView::AddChild( view );
#else
	BView::AddChild( view, siblingView );
#endif
	return;
}

bool
GUIRowColumn::RemoveChild( BView* view )
{
#ifdef _USE_ADDCHILD_HACK
	mChildren.RemoveItem( view );
#endif
	return BView::RemoveChild( view );
}

BView *
GUIRowColumn::ChildAt( uint32 index ) const
{
#ifdef _USE_ADDCHILD_HACK
	return (BView *)(mChildren.ItemAt( index ));
#else
	return BView::ChildAt( index );
#endif
}

void
GUIRowColumn::Draw( BRect rect )
{
	BView::Draw( rect );
}

void
GUIRowColumn::SetOrientation( uint32 orientation )
{
	mOrientation = orientation;
	UpdateLayout();
}

void
GUIRowColumn::UpdateLayout( void )
{
	if ( CountChildren() == 0 ) return;

	Window()->Lock();

	// First, move the first children to the origin.
	ChildAt( 0 )->MoveTo( B_ORIGIN );

	// then rearrange each child BView's location accordingly.
	BPoint leftTop;
	for ( int i=1; i < CountChildren(); i++ ) {
		if ( Orientation() == B_ITEMS_IN_ROW ) {
			leftTop = ChildAt( i-1 )->Frame().LeftBottom();
			leftTop.y++;
		} else {
			leftTop = ChildAt( i-1 )->Frame().RightTop();
			leftTop.x++;
		}
		ChildAt( i )->MoveTo( leftTop );
	}

	// finally resize myself to accomodate all the children BView's.
	BView *lastChild = ChildAt( CountChildren()-1 );
	ResizeTo( lastChild->Frame().right, lastChild->Frame().bottom );

	Window()->Unlock();

	Draw( Bounds() );
}

// $Id$
// LRowColumn.cpp
//

#include "LRowColumn.h"
#include <be/interface/Window.h>
#define DEBUG 1
#include <be/support/Debug.h>

LRowColumn::LRowColumn(
					BRect				frame,
					const char*			name,
					rowcolumn_layout	layout,
					bool				followChildFrame,
					uint32				resizingMode,
					uint32				flags
					)
:	BView( frame, name, resizingMode, flags ),
	mLayout( layout ),
	mSpacing( 0 ),
	mFollowChildFrame( followChildFrame )
{
	SetLayout( layout );
}

LRowColumn::~LRowColumn()
{
}

void
LRowColumn::AttachedToWindow( void )
{
	BView::AttachedToWindow();

	if ( Parent() != NULL )
	{
		SetViewColor( Parent()->ViewColor() );
	}
	UpdateLayout();
}

void
LRowColumn::AddChild( BView* view, BView* siblingView )
{
	// This is an ugly hack to get around the BView::AddChild bug that
	// you can't properly insert BView in front of the first children;
	// i.e. AddChild( someView, ChildAt( 0 ) ) doesn't work right.

#if _USE_ADDCHILD_HACK
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
LRowColumn::RemoveChild( BView* view )
{
	bool	ret;

#if _USE_ADDCHILD_HACK
	mChildren.RemoveItem( view );
#endif
	ret = BView::RemoveChild( view );
	UpdateLayout();
	return ret;
}

BView *
LRowColumn::ChildAt( uint32 index ) const
{
#if _USE_ADDCHILD_HACK
	return (BView *)(mChildren.ItemAt( index ));
#else
	return BView::ChildAt( index );
#endif
}

void
LRowColumn::Draw( BRect rect )
{
	BView::Draw( rect );
}

void
LRowColumn::SetSpacing( float spacing )
{
	mSpacing = spacing;
	UpdateLayout();
}

void
LRowColumn::SetLayout( rowcolumn_layout layout )
{
	mLayout = layout;
	UpdateLayout();
}

void
LRowColumn::UpdateLayout( void )
{
	if ( CountChildren() == 0 ) return;

	if( Window() ) LockLooper();

	BRect	bounds( Bounds() );
	BView*	child;

	// First, move the first children to the origin.
	child = ChildAt( 0 );
	if ( !mFollowChildFrame )
	{
		if ( Layout() == LRowColumn::COLUMN_LAYOUT )
		{
			child->ResizeTo( bounds.Width(), child->Bounds().Height() );
		}
		else
		{
			child->ResizeTo( child->Bounds().Width(), bounds.Height() );
		}
	}
	child->MoveTo( B_ORIGIN );

	// then rearrange each child BView's location accordingly.
	BPoint leftTop;
	for ( int i = 1; i < CountChildren(); i++ )
	{
		child = ChildAt( i );
		if ( Layout() == LRowColumn::COLUMN_LAYOUT )
		{
			leftTop = ChildAt( i-1 )->Frame().LeftBottom();
			leftTop.y += mSpacing;
			leftTop.y++;
			if ( !mFollowChildFrame )
			{
				child->ResizeTo( bounds.Width(), child->Bounds().Height() );
			}
		}
		else
		{
			leftTop = ChildAt( i-1 )->Frame().RightTop();
			leftTop.x += mSpacing;
			leftTop.x++;
			if ( !mFollowChildFrame )
			{
				child->ResizeTo( child->Bounds().Width(), bounds.Height() );
			}
		}
		child->MoveTo( leftTop );
	}

	// finally resize myself to accomodate all the children BView's.
	// FIXME: does this work if children is B_FOLLOW_*? I don't think so, this will pro'ly
	//        call child view's FrameResized again and screw up the lauout.
	if ( mFollowChildFrame )
	{
		BView*	lastChild = ChildAt( CountChildren()-1 );
		ResizeTo( lastChild->Frame().right, lastChild->Frame().bottom );
	}

	// if we are the root view and attached to a window, resize that window, too.
	if ( Window() && Parent() == NULL )
	{
		Window()->ResizeTo( Bounds().Width(), Bounds().Height() );
	}

	if ( Window() ) UnlockLooper();

	Draw( Bounds() );
}

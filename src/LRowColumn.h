// $Id$
// LRowColumn.h
// Container class that manages children in row or column
// includes hacked AddChild, so BView can be inserted before the first child.
//
// COLUMN_LAYOUT		ROW_LAYOUT
//  +--+				+-+-+-+
//  +--+				| | | |
//  +--+				+-+-+-+
//
#ifndef _L_ROW_COLUMN_H
#define _L_ROW_COLUMN_H

#define _USE_ADDCHILD_HACK	0

#include <be/interface/View.h>
#if _USE_ADDCHILD_HACK
#include <support/List.h>
#endif

class LRowColumn : public BView
{
public:
	enum rowcolumn_layout { COLUMN_LAYOUT, ROW_LAYOUT };

public:
							LRowColumn(
									BRect				frame,
									const char*			name,
									rowcolumn_layout	orientation,
									bool				followChildFrame,
									uint32				resizingMode,
									uint32				flags
									);
	virtual					~LRowColumn();
	virtual void			AttachedToWindow( void );
	virtual void			Draw( BRect rect );
	void					AddChild( BView* view, BView* siblingView = NULL );
	bool					RemoveChild( BView *view );
	BView*					ChildAt( uint32 index ) const;
	void					SetSpacing( float spacing );
	float					Spacing( void ) const { return mSpacing; }
	void					UpdateLayout( void );
	void					SetLayout( rowcolumn_layout layout );
	const rowcolumn_layout	Layout( void ) const
								{ return mLayout; }

protected:

private:
	rowcolumn_layout		mLayout;
#if _USE_ADDCHILD_HACK
	BList					mChildren;
#endif
	float					mSpacing;
	bool					mFollowChildFrame;
};

// vi: set ts=4:
#endif // _L_ROW_COLUMN_H

//
// GUIRowColumn.h
// Container class that manages children in row or column
// includes hacked AddChild, so BView can be inserted before the first child.
//
#ifndef _GUI_ROW_COLUMN_H
#define _GUI_ROW_COLUMN_H

#define _USE_ADDCHILD_HACK

#include <interface/View.h>
#include <support/List.h>

class GUIRowColumn : public BView
{
public:
							GUIRowColumn(	BRect		frame,
											const char*	name,
											uint32		orientation,
											uint32		resizingMode,
											uint32		flags			);
							GUIRowColumn(	BMessage*	archive );
	virtual					~GUIRowColumn();
	static	BArchivable*	Instantiate( BMessage* archive );
	virtual status_t		Archive( BMessage* archive, bool deep = true ) const;
	virtual void			AttachedToWindow( void );
	virtual void			Draw( BRect rect );
	void					AddChild( BView *view, BView *siblingView = NULL );
	bool					RemoveChild( BView *view );
	BView*					ChildAt( uint32 index ) const;
	void					UpdateLayout( void );
	void					SetOrientation( uint32 orientation );
	uint32					Orientation( void ) const
								{ return mOrientation; }

protected:

private:
	uint32					mOrientation;
#ifdef _USE_ADDCHILD_HACK
	BList					mChildren;
#endif
};

#endif // _GUI_ROW_COLUMN_H

// $Id$
#ifndef _DOCK_ITEM_H
#define _DOCK_ITEM_H

#include <be/support/Locker.h>
#include <be/support/String.h>

class BLocker;
class BBitmap;

class DockItem
{
public:
	virtual					~DockItem();
	const BBitmap*			Bitmap( int32 size );
	const char*				Label( void ) const;
	status_t				SetName( const char* name );
	status_t				SetLabel( const char* label );
	status_t				SetBitmap( BBitmap* bitmap, int32 size );

	enum { LARGE, SMALL };

protected:
							DockItem( const char* name );
	BLocker*				Locker( void );

private:
	BLocker					mLocker;
	BBitmap*				mLargeBitmap;
	BBitmap*				mSmallBitmap;
	BString					mName;
	BString					mLabel;
};

inline BLocker*
DockItem::Locker( void )
{
	return &mLocker;
}

// vi: set ts=4:
#endif // _DOCK_ITEM_H

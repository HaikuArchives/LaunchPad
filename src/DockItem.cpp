// $Id$

#include <be/support/Autolock.h>
#include <be/interface/Bitmap.h>
#include "DockItem.h"

DockItem::DockItem( const char* path )
:	mLargeBitmap( NULL ),
	mSmallBitmap( NULL )
{
}

DockItem::~DockItem()
{
	mLocker.Lock();

	if ( mLargeBitmap )
	{
		delete mLargeBitmap;
		mLargeBitmap = NULL;
	}

	if ( mSmallBitmap )
	{
		delete mSmallBitmap;
		mSmallBitmap = NULL;
	}
}

const BBitmap*
DockItem::Bitmap( int32 size )
{
	switch ( size )
	{
	case DockItem::LARGE:
		return mLargeBitmap;
		break;
	case DockItem::SMALL:
		return mSmallBitmap;
		break;
	default:
		return NULL;
		break;
	}
}

const char*
DockItem::Label( void ) const
{
	return mLabel.String();
}

status_t
DockItem::SetName( const char* name )
{
	BAutolock autoLock( Locker() );
	if ( !autoLock.IsLocked() ) return B_ERROR;

	mName = name;
	return B_NO_ERROR;
}

status_t
DockItem::SetLabel( const char* label )
{
	BAutolock autoLock( Locker() );
	if ( !autoLock.IsLocked() ) return B_ERROR;

	mLabel = label;
	return B_NO_ERROR;
}

status_t
DockItem::SetBitmap( BBitmap* bitmap, int32 size )
{
	BAutolock autoLock( Locker() );
	if ( !autoLock.IsLocked() ) return B_ERROR;

	status_t err = B_NO_ERROR;

	switch ( size )
	{
	case LARGE:
		mLargeBitmap = bitmap;
		break;
	case SMALL:
		mSmallBitmap = bitmap;
		break;
	default:
		err = B_BAD_VALUE;
		break;
	}

	return err;
}

// vi: set ts=4:

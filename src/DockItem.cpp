// $Id$

#define DEBUG 1
#include <be/support/Autolock.h>
#include <be/interface/Bitmap.h>
#include <be/interface/Screen.h>
#include "DockItem.h"

#if DEBUG
#include <be/support/Debug.h>
#include <string.h>
#define REPORT_ERROR(a) fprintf( stderr, "ERROR: %s at line %d : %s\n", __FILE__, __LINE__, strerror(a) )
#else
static inline void REPORT_ERROR( int a ) {}
#endif

DockItem::DockItem( const char* name )
:	mLargeBitmap( NULL ),
	mSmallBitmap( NULL ),
	mHilightenLargeBitmap( NULL ),
	mHilightenSmallBitmap( NULL ),
	mName( name ),
	mLabel( name )
{
}

DockItem::~DockItem()
{
	mLocker.Lock();

	if ( mLargeBitmap ) delete mLargeBitmap;
	if ( mSmallBitmap ) delete mSmallBitmap;
	if ( mHilightenLargeBitmap ) delete mHilightenLargeBitmap;
	if ( mHilightenSmallBitmap ) delete mHilightenSmallBitmap;
}

status_t
DockItem::BuildMenu( BMenu* menu )
{
	return B_NO_ERROR;
}

const BBitmap*
DockItem::Bitmap( int32 size, bool hilite )
{
	BBitmap* bitmap;
	BBitmap* hilightenBitmap;

	switch ( size )
	{
	case DockItem::LARGE:
		bitmap = mLargeBitmap;
		hilightenBitmap = mHilightenLargeBitmap;
		break;
	case DockItem::SMALL:
		bitmap = mSmallBitmap;
		hilightenBitmap = mHilightenSmallBitmap;
		break;
	default:
		return NULL;
		break;
	}

	if ( hilite )
	{
		if ( hilightenBitmap )
		{
			return hilightenBitmap;
		}
		else
		{
			hilightenBitmap = HiliteBitmap( bitmap );
			return hilightenBitmap;
		}
	}
	else
	{
		return bitmap;
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

	mName.SetTo( name );
	return B_NO_ERROR;
}

status_t
DockItem::SetLabel( const char* label )
{
	BAutolock autoLock( Locker() );
	if ( !autoLock.IsLocked() ) return B_ERROR;

	mLabel.SetTo( label );
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

BBitmap*
DockItem::HiliteBitmap( BBitmap* bitmap )
{
	if ( bitmap->ColorSpace() != B_CMAP8 )
	{
		return NULL; // not supported.
	}

	BScreen screen;

	BBitmap* hilighten = new BBitmap( bitmap->Bounds(), bitmap->ColorSpace() );

	uint8 hiliteBits[ bitmap->BitsLength() ];
	uint8* hiliteBitsPtr = &hiliteBits[0];
	uint8* bitsPtr = (uint8*)bitmap->Bits();
	rgb_color hiliteColor;

	for ( int i = 0; i < bitmap->BitsLength(); i++ )
	{
		if ( *bitsPtr != B_TRANSPARENT_MAGIC_CMAP8 )
		{
			// Darken (I think I saw Pavel Cisler saying this is the
			// particular darkening coefficient used in Tracker.)
			hiliteColor = screen.ColorForIndex( *bitsPtr++ );
			hiliteColor.red   = uint8( 0.7 * hiliteColor.red   );
			hiliteColor.green = uint8( 0.7 * hiliteColor.green );
			hiliteColor.blue  = uint8( 0.7 * hiliteColor.blue  );
			*hiliteBitsPtr++ = screen.IndexForColor( hiliteColor );
		}
		else
		{
			// Don't touch the tranparent portion of the bitmap.
			*hiliteBitsPtr++ = *bitsPtr++;
		}
	}

	hilighten->SetBits( hiliteBits, bitmap->BitsLength(), 0, B_CMAP8 );

	return hilighten;
}

// vi: set ts=4:

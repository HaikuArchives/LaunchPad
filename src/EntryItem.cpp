// $Id$

#define DEBUG 1
#include <be/storage/Entry.h>
#include <be/storage/NodeInfo.h>
#include <be/interface/Bitmap.h>
#include "EntryItem.h"

#if DEBUG
#include <be/support/Debug.h>
#include <string.h>
#define REPORT_ERROR(a) fprintf( stderr, "ERROR: %s at line %d : %s\n", __FILE__, __LINE__, strerror(a) )
#else
static inline void REPORT_ERROR( int a ) {}
#endif

EntryItem::EntryItem( const char* path )
:	DockItem( "Not yet" )
{
	status_t err;

	// Fully initialize the object.
	BEntry entry( path, true );
	if ( ( err = entry.InitCheck() ) < B_NO_ERROR )
	{
		REPORT_ERROR( err );
		return;
	}

	err = entry.GetRef( &mRef );
	if ( err < B_NO_ERROR )
	{
		REPORT_ERROR( err );
		return;
	}

	err = FinalizeInit();
	if ( err < B_NO_ERROR )
	{
		REPORT_ERROR( err );
	}
}

EntryItem::EntryItem( entry_ref* ref )
:	DockItem( "Not yet" ),
	mRef( *ref )
{
	status_t err = FinalizeInit();
	if ( err < B_NO_ERROR )
	{
		REPORT_ERROR( err );
	}
}

status_t
EntryItem::FinalizeInit( void )
{
	status_t err;

	BEntry entry( &mRef );
	if ( ( err = entry.InitCheck() ) < B_NO_ERROR )
	{
		REPORT_ERROR( err );
		return err;
	}

	// Set name
	char name[ B_FILE_NAME_LENGTH ];
	entry.GetName( name );
	SetName( name );

	// Set bitmaps, or icons.
	BBitmap* largeIcon = new BBitmap( BRect( 0, 0, 31, 31 ), B_CMAP8 );
	BBitmap* miniIcon  = new BBitmap( BRect( 0, 0, 15, 15 ), B_CMAP8 );
	err = BNodeInfo::GetTrackerIcon( &mRef, largeIcon, B_LARGE_ICON );
	err = BNodeInfo::GetTrackerIcon( &mRef, miniIcon,  B_MINI_ICON  );
	if ( err < B_NO_ERROR )
	{
		REPORT_ERROR( err );
	}

	err = SetBitmap( largeIcon, DockItem::LARGE );
	err = SetBitmap( miniIcon,  DockItem::SMALL );
	if ( err < B_NO_ERROR )
	{
		REPORT_ERROR( err );
	}

	return err;
}

EntryItem::~EntryItem()
{
}

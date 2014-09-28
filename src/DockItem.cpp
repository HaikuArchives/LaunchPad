// $Id: DockItem.cpp,v 1.2 2000/04/19 08:55:58 hiro Exp $
// DockItem.cpp
//

#include "DockItem.h"
#include "colors.h"
#include <be/support/Debug.h>
#include <StorageKit.h>
#include <be/storage/AppFileInfo.h>
#include <be/app/Roster.h>
#include <be/interface/Screen.h>
#include <stdio.h>

const BRect kLargeIconRect(0,0,31,31);
const BRect kSmallIconRect(0,0,15,15);

DockItem::DockItem()
{
//	mIndex = index;
	mHighlight = false;
	mHasRef = false;
	*mMimeType = NULL;
	*mCaption = NULL;
	mLargeIcon = new BBitmap( kLargeIconRect, B_COLOR_8_BIT );
	mSmallIcon = new BBitmap( kSmallIconRect, B_COLOR_8_BIT );
	mLargeIconHi = new BBitmap( kLargeIconRect, B_COLOR_8_BIT );
	mSmallIconHi = new BBitmap( kSmallIconRect, B_COLOR_8_BIT );
}

DockItem::DockItem( entry_ref* )
{
}

DockItem::~DockItem()
{
	delete mSmallIcon;
	delete mLargeIcon;
	delete mLargeIconHi;
	delete mSmallIconHi;
}

void
DockItem::InitObject( void )
{
}

status_t
DockItem::GetRef( entry_ref *ref ) const
{
	*ref = mRef;
	if ( mHasRef )
		return B_NO_ERROR;
	else
		return B_ERROR;
}

void
DockItem::SetTo( const entry_ref *ref )
{
	BEntry entry;
	BPath path;
	BSymLink symLink;
	BMimeType type;
	BMimeType appType;

	// Traverse the symbolic link if necessary.
	entry.SetTo( ref, true );
	entry.GetRef( &mRef );
	/*
	if ( entry.IsSymLink() ) {
		symLink.SetTo( ref );
		symLink.GetLinkedPath( &path );
		printf( "path = %s\n", path.Path() );
		entry.SetTo( path.Path() );
		entry.GetRef( &mRef );
	} else {
		mRef = *ref;
	}
	*/

	// Retrieve the ref's name.
	entry.SetTo( &mRef );
	entry.GetName( mCaption );
	entry.GetPath( &path );
	PRINT(( "entry ref name (%s)\n", mCaption ));
	PRINT(( "path = %s\n", path.Path() ));

	// Retrieve mime type.
	BNode node;
	node.SetTo( &mRef );
	BNodeInfo nodeInfo( &node );
	nodeInfo.GetType( mMimeType );
	type.SetType( mMimeType );

	// Remember the node type.
	if ( node.IsDirectory() ) {
		mNodeType = kFolder;
	} else {
		mNodeType = kFile;
	}

/*
	// First retrieve custom icon. (GetTrackerIcon's not perfect yet)
	if ( ::GetTrackerIcon( &mRef, mLargeIcon, mSmallIcon ) != B_NO_ERROR ) {
		this->GetGenericIcon();
	}
*/
	if ( BNodeInfo::GetTrackerIcon( &mRef, mLargeIcon, B_LARGE_ICON ) < B_NO_ERROR ) {
		PRINT(( "Couldn't retrieve icon\n" ));
		GetGenericIcon();
	}
	if ( BNodeInfo::GetTrackerIcon( &mRef, mSmallIcon, B_MINI_ICON ) < B_NO_ERROR ) {
		PRINT(( "Couldn't retrive mini icon\n" ));
		GetGenericIcon();
	}

	MakeHighlitIcon();

	mHasRef = true;
}

void
DockItem::GetGenericIcon( void )
{
	if ( IsFolder() ) {
		BMimeType type;
		type.SetType( "application/x-vnd.Be-directory" );
		type.GetIcon( mLargeIcon, B_LARGE_ICON );
		type.GetIcon( mSmallIcon, B_MINI_ICON );
	} else {
		app_info appInfo;
		BFile appFile;
		BAppFileInfo appFileInfo;
		be_app->GetAppInfo( &appInfo );
		appFile.SetTo( &appInfo.ref, B_READ_WRITE );
		appFileInfo.SetTo( &appFile );
		appFileInfo.GetIconForType( "application/x-be-executable", mLargeIcon, B_LARGE_ICON );
		appFileInfo.GetIconForType( "application/x-be-executable", mSmallIcon, B_MINI_ICON );
		/*
		BMimeType appType;
		appType.SetType( "application/x-vnd.HK-LaunchPad" );
		appType.GetIcon( mLargeIcon, B_LARGE_ICON );
		appType.GetIcon( mSmallIcon, B_MINI_ICON );
		*/
	}
}

void
DockItem::MakeHighlitIcon( void )
{
	size_t size;
	BBitmap *icon, *iconHi;
	BScreen screen;

	for ( int i=0; i<2; i++ ) {

		switch ( i ) {
		case 0:
			size = 1024;
			icon = mLargeIcon;
			iconHi = mLargeIconHi;
			break;
		case 1:
			size = 256;
			icon = mSmallIcon;
			iconHi = mSmallIconHi;
			break;
		}
		
		uchar *hilitBits = new uchar[size];
		uchar *hilitBitsPtr = &hilitBits[0];
		uchar *bitsPtr = (uchar *)icon->Bits();
		rgb_color hilitColor;
		for ( int i=0; i<size; i++ ) {
			if ( *bitsPtr != B_TRANSPARENT_8_BIT ) {
				hilitColor = system_colors()->color_list[*bitsPtr++];
				hilitColor.red *= 0.7;
				hilitColor.green *= 0.7;
				hilitColor.blue *= 0.7;
				*hilitBitsPtr++ = screen.IndexForColor( hilitColor );
			} else {
				*hilitBitsPtr++ = *bitsPtr++;
			}
		}
		iconHi->SetBits( hilitBits, size, 0, B_COLOR_8_BIT );

	}
}

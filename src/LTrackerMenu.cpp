//
// LTrackerMenu.cpp
//

#include <stdio.h>
#include <be/support/Debug.h>
#include <be/storage/Entry.h>
#include <be/storage/Directory.h>
#include <be/storage/Mime.h>	// B_MINI_ICON etc...
#include <be/storage/NodeInfo.h>
#include <be/interface/Bitmap.h>
#include "LTrackerMenu.h"

// --------------------------------------------------------------------------
//  LTrackerMenu
// --------------------------------------------------------------------------
LTrackerMenu::LTrackerMenu(	entry_ref*		ref,
							menu_layout		layout )
:	BMenu( "", layout ),
	mIsValid( false ),
	mRef( *ref )
{
	BEntry	entry( &mRef );
	char	label[B_FILE_NAME_LENGTH];
	entry.GetName( label );
	SetName( label );
}

LTrackerMenu::~LTrackerMenu()
{
}

void
LTrackerMenu::AttachedToWindow( void )
{
	PRINT(( "%s::AttachedToWindow\n", class_name(this) ));
	if ( !mIsValid )
	{
		ScanFolder();
		SetTargetForItems( Superitem()->Target() );
	}

	BMenu::AttachedToWindow();
}

void
LTrackerMenu::ScanFolder( void )
{
	BNode				node;
	BDirectory			folder;
	entry_ref			itemRef;
	LTrackerMenuItem*	item;
	LTrackerMenu*		subMenu;

	node.SetTo( &mRef );
	if ( !node.IsDirectory() )
	{
#if DEBUG
		BEntry entry( &mRef );
		char label[B_FILE_NAME_LENGTH];
		entry.GetName( label );
		printf( "%s is not a folder\n", label );
#endif
		return;
	}

	folder.SetTo( &mRef );

	if ( folder.CountEntries() == 0 )
	{
		BMenuItem* empty = new BMenuItem( "Empty Folder", NULL );
		empty->SetEnabled( false );
		SetTriggersEnabled( false );
		AddItem( empty );
	}
	else
	{
		while ( folder.GetNextRef( &itemRef ) != B_ENTRY_NOT_FOUND )
		{
			node.SetTo( &itemRef );
			BMessage* msg = NULL;
			if ( Superitem()->Message() )
			{
				msg = new BMessage( Superitem()->Message() );
				msg->RemoveName( "refs" );
				msg->AddRef( "refs", &itemRef );
			}
			if ( node.IsDirectory() )
			{
				subMenu = new LTrackerMenu( &itemRef );
				subMenu->SetFont( be_plain_font );
				item = new LTrackerMenuItem( &itemRef, subMenu, msg );
			}
			else
			{
				item = new LTrackerMenuItem( &itemRef, msg );
			}
			AddItem( item );
		}
	}

	mIsValid = true;
}

// ----------------------------------------------------------------------------
//  Builds menu hierarchy and returns a top menu item
//  minimum error checking. only returns NULL if entry_ref is not a folder.
// ----------------------------------------------------------------------------
BMenuItem*
LTrackerMenu::BuildMenu( const entry_ref* ref, BMessage* message, BHandler* target )
{
	BNode		node;
	BEntry		entry;
	BDirectory	folder;

	node.SetTo( ref );
	if ( !node.IsDirectory() ) {
		return NULL;
	}

	LTrackerMenu* subMenu = new LTrackerMenu( (entry_ref*)ref );
	subMenu->SetFont( be_plain_font );
	BMessage* msg = NULL;
	if ( message )
	{
		msg = new BMessage( message );
		msg->AddRef( "refs", ref );
	}

	LTrackerMenuItem* item = new LTrackerMenuItem( ref, subMenu, msg );
	if ( target )
	{
		item->SetTarget( target );
	}

	delete message;

	return item;
}

// --------------------------------------------------------------------------
//  LTrackerMenuItem
// --------------------------------------------------------------------------
LTrackerMenuItem::LTrackerMenuItem(	const entry_ref*	ref,
									BMessage*			message,
									char				shortcut,
									uint32				modifiers )
:	BMenuItem( "", message, shortcut, modifiers )
{
	InitObject( ref );
}

LTrackerMenuItem::LTrackerMenuItem(	const entry_ref*	ref,
									BMenu*				subMenu,
									BMessage*			message )
	: BMenuItem( subMenu, message )
{
	InitObject( ref );
}

LTrackerMenuItem::~LTrackerMenuItem()
{
	delete mMiniIcon;
}

void
LTrackerMenuItem::InitObject( const entry_ref* ref )
{
	bool	needIcon = true;
	BRect	iconRect( 0, 0, B_MINI_ICON-1, B_MINI_ICON-1 );
	mMiniIcon = new BBitmap( iconRect, B_COLOR_8_BIT );

	// label
	BEntry	entry( ref, true );

	// if ref is a broken sym-link, the above init fails...
	if ( entry.InitCheck() < B_NO_ERROR ) {
		entry.SetTo( ref, false );
		// (BeOS PR2) at this point, entry should be okay. but for some reason, GetTrackerIcon locks up app_server...
		needIcon = false;
	}

	// Retrieve name.
	char	label[B_FILE_NAME_LENGTH];
	entry.GetName( label );
	SetLabel( label );

	// icon
	if ( needIcon ) {
		BNodeInfo::GetTrackerIcon( const_cast<entry_ref*>(ref), mMiniIcon, B_MINI_ICON );
	}
}

void
LTrackerMenuItem::GetContentSize( float* width, float* height )
{
	BMenuItem::GetContentSize( width, height );
	*width += 20;
	*height = 16;
}

void
LTrackerMenuItem::DrawContent( void )
{
	// icon
	drawing_mode drawingMode = Menu()->DrawingMode();
	Menu()->SetDrawingMode( B_OP_OVER );
	Menu()->MovePenTo( ContentLocation() );
	Menu()->DrawBitmapAsync( mMiniIcon );
	Menu()->SetDrawingMode( drawingMode );

	// label
	Menu()->MovePenBy( 20, 12 );
	if ( IsSelected() ) {	// Highlighted...
		rgb_color	lowColor = Menu()->LowColor();
		Menu()->SetLowColor( 152, 152, 152, 255 );
		Menu()->DrawString( Label() );
		Menu()->SetLowColor( lowColor );
	} else {
		Menu()->DrawString( Label() );
	}
}

void
LTrackerMenuItem::Highlight( bool flag )
{
	BMenuItem::Highlight( flag );
}

// ----------------------------------------------------------------------------
//  BuildNavMenu - helper function to build menu hierarchy
//  LTrackerMenu::BuildMenu does the same.
// ----------------------------------------------------------------------------
#if 0
status_t BuildNavMenu( BMenu* top, const entry_ref* ref, BMessage* message, BHandler* target )
{
	BNode		node;
	BEntry		entry;
	BDirectory	folder;

	node.SetTo( ref );
	if ( !node.IsDirectory() ) {
		return B_ERROR;
	}

	LTrackerMenu* subMenu = new LTrackerMenu( (entry_ref*)ref );
	subMenu->SetFont( be_plain_font );
	BMessage* msg = new BMessage( message );
	msg->AddRef( "refs", ref );
	LTrackerMenuItem* item = new LTrackerMenuItem( ref, subMenu, msg );
	item->SetTarget( target );
	top->AddItem( item );

	return B_NO_ERROR;
}
#endif

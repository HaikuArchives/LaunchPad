// $Id: PrefHandler.cpp,v 1.2 2000/04/14 07:10:20 hiro Exp $
// PrefHandler.cpp
//

#include "PrefHandler.h"
#include "LaunchPadDefs.h"
#include "LaunchPadWindow.h"
#include "DockPane.h"
#include <Alert.h>
#include <stdio.h>
#include <memory.h>

enum {
	kHorizontalPaneLayout,
	kVerticalPaneLayout
};

PrefHandler::PrefHandler()
	:	mPref( LAUNCH_PAD_SIGNATURE_MINOR ),
		mPrefSet( mPref, "settings", true )
{
	if ( mPref.InitCheck() ) {
		ErrorDialog( "Cannot initialize Preferences" );
		return;
	}
	if ( mPrefSet.InitCheck() ) {
		ErrorDialog( "Cannot initialize PreferenceSet" );
		return;
	}

	PRINT(( "Loading settings\n" ));
	Load();
}

PrefHandler::~PrefHandler()
{
	delete mSettings;
}

status_t
PrefHandler::Save( BMessage* settings )
{
	version_info	version;
	::GetAppVersion( &version );

	// Save the settings
	mPrefSet.Delete( "Version" );
	mPrefSet.Delete( "LaunchPadSettings" );
	if ( mPrefSet.SetData( "Version", &version, sizeof(version_info), B_RAW_TYPE ) )
	{
		ErrorDialog( "Cannot set Version setting" );
	}
	else if ( SetMessage( "LaunchPadSettings", settings ) )
	{
		ErrorDialog( "Cannot set settings" );
	}
	else if ( mPrefSet.Save() )
	{
		ErrorDialog( "Cannot save setting" );
	}

#if DEBUG
	BMessage msg;
	GetMessage( "LaunchPadSettings", &msg );
	PRINT(( "Checking if the preference setting message was set\n" ));
	msg.PrintToStream();
#endif
	return B_OK;
}

status_t
PrefHandler::Load( void )
{
	GetVersion();
	PRINT(( "Loading Version %d.%d.%d setting..\n", Version().major, Version().middle, Version().minor ));
	if ( Version().major == 1 && Version().middle <= 1 ) {
		LoadObsoleteSetting();
	} else {
		mSettings = new BMessage();
		if ( GetMessage( "LaunchPadSettings", mSettings ) != B_OK ) {
			puts( "Default setting" );
			GetDefaultSettings( mSettings );
		}
	}

	return B_OK;
}

status_t
PrefHandler::LoadObsoleteSetting( void )
{
	PRINT(( "Loading pre-1.1 LaunchPad settings" ));
	BAlert* alert = new BAlert(	"", "A settings file "
								"created probably by the pre-1.1 version "
								"of LaunchPad was found. I'll convert the settings "
								"to the format suitable for the current version. "
								"If you encounter problems, you may need to "
								"throw away the old settings and start over.",
								"OK" );
	alert->Go();

	// pre-1.1.0 version has only one pad
	mSettings = new BMessage( kMsgSettings );
	mSettings->AddInt32( "NumberOfPads", 1 );
	
	// Placeholder for this pad's setting
	BMessage	padSetting( kMsgPadSetting );

	BRect		frame;
	uint32		numberOfPanes;
	uint32		paneAppearance;
	uint32		paneLayout;
	entry_ref	ref;
	const void*	data;
	ssize_t		size;
	uint32		type = '    ';

	// Retrieve frame location
	if ( mPrefSet.GetData( "frame", data, size, type ) || type != B_RECT_TYPE ) {
		frame.Set( 10, 50, 100, 300 );
	} else {
		memcpy( &frame, data, size );
	}
	padSetting.AddRect( "Frame", frame );

	// Retrieve the number of panes
	if ( mPrefSet.GetData( "nPanes", data, size, type ) ) {
		numberOfPanes = 8;
	} else {
		memcpy( &numberOfPanes, data, size );
	}
	padSetting.AddInt32( "NumberOfPanes", numberOfPanes );

	// Retrieve pane appearance
	if ( mPrefSet.GetData( "DisplayMode", data, size, type ) ) {
		paneAppearance = kLargeIconWithoutCaption;
	} else {
		memcpy( &paneAppearance, data, size );
	}
	padSetting.AddInt32( "PaneAppearance", paneAppearance );

	// Retrieve pane layout
	if ( mPrefSet.GetData( "PaneLayout", data, size, type ) ) {
		paneLayout = kHorizontalPaneLayout;
	} else {
		memcpy( &paneLayout, data, size );
	}
	if ( paneLayout == kHorizontalPaneLayout ) {
		paneLayout = B_ITEMS_IN_COLUMN;
	} else {
		paneLayout = B_ITEMS_IN_ROW;
	}
	padSetting.AddInt32( "PaneLayout", paneLayout );

	// Retrieve pane contents
	PaneInfo *panes = new PaneInfo[ numberOfPanes ];

	if ( mPrefSet.GetData( "panes", data, size, type ) ) {
		printf( "Preference not found. (trying to get panes)\n" );
		return B_OK;
	} else {
		memcpy( panes, data, size );
	}

	BEntry	entry;
	for ( int i = 0; i < numberOfPanes; i++ ) {
		BMessage paneSetting( kMsgPaneSetting );
		if ( panes[i].hasItem ) {
			entry.SetTo( panes[i].path );
			entry.GetRef( &ref );
			paneSetting.AddInt32( "PaneContentType", kPaneTypeEntryRef );
			paneSetting.AddRef( "EntryRefs", &ref );
#ifdef DEBUG
			BPath path;
			entry.GetPath( &path );
			printf( "Reading Prefsrence... pane %d has ref %s\n", i, path.Path() );
#endif
		} else {
			paneSetting.AddInt32( "PaneContentType", kPaneTypeEmpty );
#ifdef DEBUG
			printf( "Readgin Preference... pane %d has no ref\n", i );
#endif
		}
		padSetting.AddMessage( "PaneSetting", &paneSetting );
	}

	// Store the retrieved pad setting as a BMessage
	mSettings->AddMessage( "PadSetting", &padSetting );

	return B_OK;
}

// Set BMessage object in setting
status_t
PrefHandler::SetMessage( const char* name, BMessage* msg )
{
	return (mPrefSet.GetMessage()).AddMessage( name, msg );
}

// Retrieve BMessage object from setting
status_t
PrefHandler::GetMessage( const char* name, BMessage* msg )
{
	return (mPrefSet.GetMessage()).FindMessage( name, msg );
}


// Returns the version of LaunchPad that created the current setting.
status_t
PrefHandler::GetVersion( void )
{
	const void*	data;
	ssize_t		size;
	uint32		type = '    ';
	if ( mPrefSet.GetData( "Version", data, size, type) || type != B_RAW_TYPE ) {
		if ( mPrefSet.GetData( "frame", data, size, type ) || type != B_RECT_TYPE ) {
			mVersion.major = 0;
			mVersion.middle = 0;
			mVersion.minor = 0;
		} else {
			// pre-1.1 version doesn't store the version info.
			mVersion.major = 1;
			mVersion.middle = 1;
			mVersion.minor = 0;
		}
		return B_OK;
	} else {
		memcpy( &mVersion, data, size );
		PRINT(( "Version info retrieved, %d.%d.%d size=%d\n", mVersion.major, mVersion.middle, mVersion.minor, size ));
		return B_OK;
	}
}

void
PrefHandler::GetDefaultSettings( BMessage* settings )
{
	settings->what = kMsgSettings;
	settings->AddInt32( "NumberOfPads", 1 );

	const uint32	kDefaultNumberOfPanes = 8;
	BMessage		padSetting( kMsgPadSetting );

	padSetting.AddRect( "Frame", BRect( 100, 100, 100, 100 ) );
	padSetting.AddInt32( "NumberOfPanes", kDefaultNumberOfPanes );
	for ( int i = 0; i < kDefaultNumberOfPanes; i++ ) {
		BMessage	paneSetting( kMsgPaneSetting );
		paneSetting.AddInt32( "PaneContentType", kPaneTypeEmpty );
		padSetting.AddMessage( "PaneSetting", &paneSetting );
	}
	padSetting.AddInt32( "PaneAppearance", kLargeIconWithoutCaption );
	padSetting.AddInt32( "PaneLayout", B_ITEMS_IN_ROW );

	settings->AddMessage( "PadSetting", &padSetting );
}

void
PrefHandler::ErrorDialog( const char* errorMessage )
{
	BAlert*	dialog = new BAlert( "PrefHandler", errorMessage, "Ignore", "Quit" );
	switch ( dialog->Go() ) {
	case 0:
		return;
		break;
	case 1:
		be_app->PostMessage( B_QUIT_REQUESTED );
		break;
	default:
		return;
		break;
	}
}

// $Id$
// LaunchPadApp.cpp
//

#include "LaunchPadApp.h"
#include "LaunchPadDefs.h"
#include "PrefHandler.h"
#include <be/interface/Alert.h>
#include <be/app/Roster.h>
#include <stdio.h>

LaunchPadApp::LaunchPadApp()
:	BApplication( LAUNCH_PAD_SIGNATURE ),
	mPreference( new BMessage( kMsgSettings ) )
{
}

LaunchPadApp::~LaunchPadApp()
{
	delete mPreference;
}

void
LaunchPadApp::AboutRequested( void )
{
	version_info version;
	::GetAppVersion( &version );

	char	versionStr[32];
	if ( version.internal > 0 )
	{
		sprintf( versionStr,	"%d.%d.%dd%d",
								version.major,
								version.middle,
								version.minor,
								version.internal );
	}
	else
	{
		sprintf( versionStr,	"%d.%d.%d",
								version.major,
								version.middle,
								version.minor );
	}

	char	label[256];
	sprintf(	label,
				"LaunchPad" B_UTF8_ELLIPSIS "\n"
				"\t Version %s\n"
				"\t by Hiromasa Kato" "\n\n\n"
				"PalleteWindow" B_UTF8_ELLIPSIS "\n"
				"\t by Steve Sprang \n"
				"\t Copyright" B_UTF8_COPYRIGHT " 1997 Digital Dungeon Software",
				versionStr );

	BAlert *about = new BAlert( "", label, "OK" );
	about->Go();
}

void
LaunchPadApp::ReadyToRun( void )
{
	mPref = new PrefHandler();

	// Run window
	int		numberOfPads = mPref->Settings()->FindInt32( "NumberOfPads" );
	if ( numberOfPads < 1 )
	{
		CreateNewPad();
	}
	for ( int i = 0; i < numberOfPads; i++ )
	{
		BMessage	padSetting;
		mPref->Settings()->FindMessage( "PadSetting", i, &padSetting );

		LaunchPadWindow*	launchPadWindow;
		uint32				tabOrientation;
		if ( padSetting.FindInt32( "PaneLayout" ) == B_ITEMS_IN_COLUMN )
		{
			tabOrientation = B_VERTICAL;
		}
		else
		{
			tabOrientation = B_HORIZONTAL;
		}
		char	name[30];
		sprintf( name, "PadWindow%d", i+1 );
		launchPadWindow = new LaunchPadWindow(	padSetting.FindRect( "Frame" ),
												name,
												tabOrientation,
												&padSetting );
		launchPadWindow->Show();
	}
}

void
LaunchPadApp::MessageReceived( BMessage* message )
{
	switch ( message->what )
	{
		case kMsgCreateNewPad:
			CreateNewPad();
			break;
		case kMsgSaveSettings:
		{
			BMessage padSetting;
			message->FindMessage( "PadSetting", &padSetting );
			SaveSettings( &padSetting, false );
			break;
		}
		default:
			BApplication::MessageReceived( message );
			break;
	}
}

void
LaunchPadApp::RefsReceived( BMessage* )
{
}

bool
LaunchPadApp::QuitRequested( void )
{
	// Summon all the windows to check if the settings need to be saved.
	for ( int i = 0; i < CountWindows(); i++ )
	{
		LaunchPadWindow*	pad = dynamic_cast<LaunchPadWindow*>(WindowAt(i));
		if ( pad != NULL && pad->SaveNeeded() )
		{
			PRINT(( "Saving settings for %s\n", pad->Name() ));
			BMessage	padSetting( kMsgPadSetting );
			pad->GetSetting( &padSetting );
			SaveSettings( &padSetting, false );
			// writing to file is done after this loop.
		}
	}

	// Then write the settings to the disk. (forceWrite = true)
	SaveSettings( NULL, true );

	// Quit the preference handler. (libprefs)
	delete	mPref;

	return true;
}

// ------------------------------------------------------------------------
//  Construct a kMsgPadSetting BMessage that encapsulates all the default
//  configuration. Used when instantiating a new pad.
// ------------------------------------------------------------------------
void
LaunchPadApp::CreateNewPad( void )
{
	const uint32	kDefaultNumberOfPanes = 8;
	BMessage		padSetting( kMsgPadSetting );

	padSetting.AddRect( "Frame", BRect( 100, 100, 100, 100 ) );
	padSetting.AddInt32( "NumberOfPanes", kDefaultNumberOfPanes );
	for ( int i = 0; i < kDefaultNumberOfPanes; i++ )
	{
		BMessage	paneSetting( kMsgPaneSetting );
		paneSetting.AddInt32( "PaneContentType", kPaneTypeEmpty );
		padSetting.AddMessage( "PaneSetting", &paneSetting );
	}
	padSetting.AddInt32( "PaneAppearance", kLargeIconWithoutCaption );
	padSetting.AddInt32( "PaneLayout", B_ITEMS_IN_ROW );

	LaunchPadWindow*	launchPadWindow;
	launchPadWindow = new LaunchPadWindow(	padSetting.FindRect( "Frame" ),
											"LaunchPadWindow",
											padSetting.FindInt32( "PaneLayout" ),
											&padSetting );
	launchPadWindow->Show();
}

status_t
LaunchPadApp::SaveSettings( BMessage* padSetting, bool forceWrite )
{
	if ( Lock() )
	{
		if ( padSetting != NULL )
		{
			mPreference->AddMessage( "PadSetting", padSetting );
		}
		if ( forceWrite )
		{
			int32		padCount = 0;
			type_code	typeCode;
			if ( mPreference->GetInfo( "PadSetting", &typeCode, &padCount ) ==
				 B_NO_ERROR )
			{
				mPreference->AddInt32( "NumberOfPads", padCount );
#if DEBUG
				PRINT(( "Saving preference:\n" ));
				mPreference->PrintToStream();
#endif
				mPref->Save( mPreference );
			}
			else
			{
				// no pad to save, but to override the previously-saved setting
				// just save empty setting.
				mPref->Save( mPreference );
			}
		}
		Unlock();
		return B_NO_ERROR;
	}
	else
	{
		BAlert*	errorDialog = new BAlert(	"",
											"Couldn't lock window",
											"", "", "Continue",
											B_WIDTH_AS_USUAL, B_WARNING_ALERT );
		errorDialog->Go();
		return B_ERROR;
	}
}

int
main( void )
{
	// Ready to launch!
	LaunchPadApp *app = new LaunchPadApp();
	app->Run();

	delete app;

	return 0;
}

void
GetAppVersion( version_info* version )
{
	// Get version_info for be_app
	app_info		ai;
	BFile			file;
	BAppFileInfo	afi;
	be_app->GetAppInfo(&ai); 
	file.SetTo(&ai.ref, B_READ_WRITE); 
	afi.SetTo(&file);
	afi.GetVersionInfo( version, B_APP_VERSION_KIND );
}

// vi: set ts=4:

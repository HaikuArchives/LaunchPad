//
// PrefHandler.h
// Class that takes care of all the preference(setting) handling, retrieval, archiving.
//
#ifndef _PREF_HANDLER_H
#define _PREF_HANDLER_H

#include "LaunchPadDefs.h"
#include "Preferences.h"	// libprefs
#include <AppFileInfo.h>
#include <Archivable.h>
#include <List.h>

// Obsolete data used in pre-1.1.0 version.
struct PaneInfo { 
	bool	hasItem;
	char	path[300];
};

// PrefHandler
class PrefHandler
{
	public:
							PrefHandler();
		virtual				~PrefHandler();
		status_t			Save( BMessage* settings );
		status_t			GetMessage( const char* name, BMessage* msg );
		status_t			SetMessage( const char* name, BMessage* msg );
		const version_info&	Version( void ) const
								{ return mVersion; }
		BMessage*			Settings( void ) const
								{ return mSettings; }
		void				ErrorDialog( const char* errorMessage );
		void				GetDefaultSettings( BMessage* settings );

	protected:
		status_t			Load( void );
		status_t			LoadObsoleteSetting( void );
		status_t			GetVersion( void );	// which version of LaunchPad created the setting?
		const uint32		GetPaneAppearance( void );

	private:
		Preferences			mPref;
		PreferenceSet		mPrefSet;
		version_info		mVersion;
		BMessage*			mSettings;
};

#endif // _PREF_HANDLER_H

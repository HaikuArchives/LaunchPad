//
// LaunchPadApp.h
//
#ifndef _LAUNCH_PAD_H
#define _LAUNCH_PAD_H

#include <Application.h>
#include "LaunchPadWindow.h"
#include "PrefHandler.h"

class LaunchPadApp : public BApplication
{
public:
						LaunchPadApp();
	virtual				~LaunchPadApp();
	virtual void		AboutRequested( void );
	virtual void		ReadyToRun( void );
	virtual void		MessageReceived( BMessage* message );
	virtual void		RefsReceived( BMessage* message );
	virtual bool		QuitRequested( void );
	void				CreateNewPad( void );
	status_t			SaveSettings( BMessage* padSetting, bool forceWrite );

protected:
	PrefHandler*		mPref;
	BMessage*			mPreference;
};

#endif // _LAUNCH_PAD_H

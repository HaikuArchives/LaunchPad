//
// LaunchPadWindow.h
//
#ifndef _LAUNCH_PAD_WINDOW_H
#define _LAUNCH_PAD_WINDOW_H

#include <Window.h>
#include <List.h>
#include "PaletteWindow.h"
#include "DockPane.h"
#include "GUIRowColumn.h"
#include "LaunchPadView.h"

class LaunchPadWindow : public PaletteWindow
{
public:
						LaunchPadWindow(	BRect		frame,
											const char*	name,
											uint32		tabOrientation,
											BMessage*	padSetting	);
						~LaunchPadWindow();
	virtual void		MessageReceived( BMessage* message );
	virtual bool		QuitRequested( void );
	virtual void		WorkspaceActivated(	int32		workspace,
											bool		active );
	status_t			GetSetting( BMessage* padSetting );
	bool				SaveNeeded( void ) const
							{ return mSaveNeeded; }

protected:

private:
	void				RelocateMyselfIfNeeded( void );

	LaunchPadView*		mPadView;
	BRect				mScreenRect;
	bool				mSaveNeeded;
};

#endif // _LAUNCH_PAD_WINDOW_H

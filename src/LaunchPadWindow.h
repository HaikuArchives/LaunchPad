// $Id$
#ifndef _LAUNCH_PAD_WINDOW_H
#define _LAUNCH_PAD_WINDOW_H

#include <be/interface/Window.h>

class LaunchPadWindow : public BWindow
{
public:
						LaunchPadWindow( BRect frame, const char* title );
	virtual				~LaunchPadWindow();
	virtual bool		QuitRequested( void );

protected:

private:
};

#endif // _LAUNCH_PAD_WINDOW_H
// vi: set ts=4:

// $Id$
#ifndef _WINDOW_TAB_H
#define _WINDOW_TAB_H

#include "WindowDragger.h"

class WindowTab : public WindowDragger
{
public:
						WindowTab(
								BRect		frame,
								const char*	name,
								uint32		resizingMode,
								uint32		flags
								);
	virtual				~WindowTab();
	virtual void		AttachedToWindow( void );
	virtual void		Draw( BRect updateRect );

protected:

private:
};

#endif // _WINDOW_TAB_H

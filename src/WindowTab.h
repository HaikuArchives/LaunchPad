// $Id$
#ifndef _WINDOW_TAB_H
#define _WINDOW_TAB_H

#include "WindowDragger.h"

class WindowTab : public WindowDragger
{
public:
	enum windowtab_orientation { HORIZONTAL, VERTICAL };

public:
							WindowTab(
									BRect					frame,
									const char*				name,
									windowtab_orientation	orientation,
									uint32					resizingMode,
									uint32					flags
									);
	virtual					~WindowTab();
	virtual void			AttachedToWindow( void );
	virtual void			Draw( BRect updateRect );

	void					SetTabOrientation( windowtab_orientation o );

protected:

private:
	static unsigned char	CLOSE_BUTTON_BITS_OFF[];
	static unsigned char	CLOSE_BUTTON_BITS_ON[];

	windowtab_orientation	mOrientation;
};

#endif // _WINDOW_TAB_H

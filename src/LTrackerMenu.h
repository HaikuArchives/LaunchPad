// $Id$
// LTrackerMenu.h
//
#ifndef _L_TRACKER_MENU_H
#define _L_TRACKER_MENU_H

#include <be/interface/Menu.h>
#include <interface/MenuItem.h>

class BBitmap;

class LTrackerMenu : public BMenu
{
public:
						LTrackerMenu(	entry_ref*		ref,
										menu_layout		layout = B_ITEMS_IN_COLUMN);
	virtual				~LTrackerMenu();
	virtual void		AttachedToWindow( void );
	void				ScanFolder( void );
	const bool			IsValid( void ) const
							{ return mIsValid; }
	static BMenuItem*	BuildMenu(	const entry_ref*	ref,
									BMessage*			message,
									BHandler*			handler );

protected:

private:
	bool				mIsValid;
	entry_ref			mRef;
};

class LTrackerMenuItem : public BMenuItem
{
public:
						LTrackerMenuItem(	const entry_ref*	ref,
											BMessage*			message,
											char				shortcut = 0,
											uint32				modifiers = 0 );
						LTrackerMenuItem(	const entry_ref*	ref,
											BMenu*				subMenu,
											BMessage*			message = NULL );
						LTrackerMenuItem(	BMessage*			data );
	virtual				~LTrackerMenuItem();

protected:
	virtual void		GetContentSize( float* width, float* height );
	virtual void		DrawContent( void );
	virtual void		Highlight( bool flag );

private:
	void				InitObject( const entry_ref* ref );

	BBitmap*			mMiniIcon;
};

#if 0
status_t BuildNavMenu( BMenu* top, const entry_ref* ref, BMessage* message, BHandler* target );
#endif

#endif // _L_TRACKER_MENU_H

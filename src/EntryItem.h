// $Id$
#ifndef _ENTRY_ITEM_H
#define _ENTRY_ITEM_H

#include "DockItem.h"
#include <be/storage/Entry.h>

class EntryItem : public DockItem
{
public:
						EntryItem( const char* path );
						EntryItem( entry_ref* ref );
	virtual				~EntryItem();
	virtual status_t	BuildMenu( BMenu* menu );

protected:
	status_t			FinalizeInit( void );

private:
	entry_ref			mRef;
	bool				mIsDirectory;
};

// vi: set ts=4:
#endif // _ENTRY_ITEM_H

// $Id$
#ifndef _ENTRY_ITEM_H
#define _ENTRY_ITEM_H

#include "DockItem.h"

class EntryItem : public DockItem
{
public:
						EntryItem( const char* path );
						EntryItem( entry_ref* ref );
	virtual				~EntryItem();

protected:
	status_t			FinalizeInit( void );

private:
	entry_ref			mRef;
};

// vi: set ts=4:
#endif // _ENTRY_ITEM_H

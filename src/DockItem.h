//
// DockItem.h
//
#ifndef _DOCK_ITEM_H
#define _DOCK_ITEM_H

#include <Bitmap.h>
#include <StorageKit.h>

// Node type const
enum {
	kFolder,
	kFile,
	kSymLink
};

class DockItem
{
	public:
							DockItem();
							DockItem( entry_ref *ref );
		virtual 			~DockItem();

		void				SetTo( const entry_ref *ref );
		status_t			GetRef( entry_ref *ref ) const;
//		const int32			Index( void ) const
//								{ return mIndex; }
		const char*			Type( void ) const
								{ return mMimeType; }
		const BBitmap*		SmallIcon( void ) const
								{ return mSmallIcon; }
		const BBitmap*		LargeIcon( void ) const
								{ return mLargeIcon; }
		const BBitmap*		SmallIconHi( void ) const
								{ return mSmallIconHi; }
		const BBitmap*		LargeIconHi( void ) const
								{ return mLargeIconHi; }
		const char*			Caption( void ) const
								{ return mCaption; }
		bool				HasRef( void ) const
								{ return mHasRef; }
		bool				IsFolder( void ) const
								{ return (mNodeType==kFolder)?true:false; }
		bool				IsFile( void ) const
								{ return (mNodeType==kFile)?true:false; }

	private:
		void				InitObject( void );
		void				GetGenericIcon( void );
		void				MakeHighlitIcon( void );

//		int32				mIndex;
		entry_ref			mRef;
		uint32				mNodeType;
		char				mMimeType[B_MIME_TYPE_LENGTH];
		BBitmap*			mLargeIcon;
		BBitmap*			mSmallIcon;
		BBitmap*			mLargeIconHi;
		BBitmap*			mSmallIconHi;
		char				mCaption[B_FILE_NAME_LENGTH];
		bool				mHighlight;
		bool				mHasRef;
};

#endif // _DOCK_ITEM_H

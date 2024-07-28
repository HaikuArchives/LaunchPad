//
// LClickFilter.h
//
#ifndef _L_CLICK_FILTER_H
#define _L_CLICK_FILTER_H

#include <MessageFilter.h>

class LClickFilter : public BMessageFilter
{
public:
							LClickFilter();
	virtual					~LClickFilter();
	virtual filter_result	Filter( BMessage* message, BHandler** target );

	static const uint32		kDrag;
	static const uint32		kDoubleClick;
	static const uint32		kSingleClick;
	static const uint32		kSingleClickHold;

protected:

private:
	BHandler*				mAssociatedTarget;
	bigtime_t				mLastMouseDown;
	uint32					mLastMouseButton;
};

#endif // _L_CLICK_FILTER_H

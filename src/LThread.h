// $Id$
//
//
#ifndef _L_THREAD_H
#define _L_THREAD_H

#include <be/kernel/OS.h>
#include <be/support/SupportDefs.h>
#include <be/support/String.h>
#include <be/support/Locker.h>


class LThread
{
public:
					LThread( const char* name );
	virtual			~LThread();
	status_t		Create( thread_func hookFunc, void* arg );
	status_t		Resume( void );
	status_t		Join( status_t* exit = NULL );
	status_t		Kill( void );

protected:

private:
	static status_t	_InternalThreadEntry( void* arg );
	status_t		InternalThreadEntry( void );

	BString			mName;
	thread_id		mThread;
	thread_func		mHookFunc;
	void*			mArg;
	BLocker			mLock;
};

#endif // _L_THREAD_H

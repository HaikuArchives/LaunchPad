// $Id$
//
//

#include <stdio.h>
#include <be/support/Autolock.h>
#include <be/kernel/OS.h>
#include "LThread.h"

LThread::LThread( const char* name )
:	mName( name ),
	mThread( 0 ),
	mHookFunc( NULL ),
	mArg( NULL )
{
}

LThread::~LThread()
{
}

status_t
LThread::Create( thread_func hookFunc, void* arg )
{
	BAutolock	locker( mLock );

	if ( mThread != 0 )
	{
		perror( "LThread::Create:another thread already assigned" );
	}

	mHookFunc = hookFunc;
	mArg = arg;

	mThread = spawn_thread( _InternalThreadEntry, mName.String(), B_NORMAL_PRIORITY, this );

	return B_NO_ERROR;
}

status_t
LThread::Resume( void )
{
	return resume_thread( mThread );
}

status_t
LThread::Join( status_t* exit )
{
	status_t	exitStatus;
	status_t	err;

	err = wait_for_thread( mThread, &exitStatus );
	if ( err == B_NO_ERROR )
	{
		mThread = 0;
		mHookFunc = NULL;
		mArg = NULL;
	}

	if ( exit != NULL ) *exit = exitStatus;
	return err;
}

status_t
LThread::Kill( void )
{
	status_t	err;

	err = kill_thread( mThread );
	if ( err == B_NO_ERROR )
	{
		mThread = 0;
		mHookFunc = NULL;
		mArg = NULL;
	}

	return err;
}

status_t
LThread::_InternalThreadEntry( void* arg )
{
	return ((LThread*)arg)->InternalThreadEntry();
}

status_t
LThread::InternalThreadEntry( void )
{
	status_t err = mHookFunc( mArg );

	// clean up
	mThread = 0;
	mHookFunc = NULL;
	mArg = NULL;

	return err;
}

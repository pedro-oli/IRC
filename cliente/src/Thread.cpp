#include "Thread.h"

Thread::Thread() : _threadId(0) {}

Thread::~Thread()
{
#ifndef _WIN32
    pthread_exit(NULL);
#endif
}

bool Thread::Start(ThreadFunction callback, void* param)
{
	if (pthread_create(&_threadId, NULL, *callback, param) == 0)
		return true;

	return false;
}
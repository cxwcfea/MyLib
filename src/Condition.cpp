/*
 * Condition.cpp
 *
 *  Created on: Feb 22, 2014
 *      Author: xcheng
 */

#include <sys/time.h>
#include <cerrno>
#include "Condition.h"

namespace cxwcfea {

auto Condition::waitForSeconds(int seconds) -> bool {
	struct timespec abstime;
#ifdef CLOCK_REALTIME
	clock_gettime(CLOCK_REALTIME, &abstime);
#else
	struct timeval tv;
	gettimeofday(&tv, nullptr);
	abstime.tv_sec = tv.tv_sec;
	abstime.tv_nsec = tv.tv_usec * 1000;
#endif
	abstime.tv_sec += seconds;
	Mutex::UnassignGuard ug(lock_);
	return ETIMEDOUT == pthread_cond_timedwait(&pcond_, lock_.getPthreadMutex(), &abstime);
}


} /* namespace cxwcfea */


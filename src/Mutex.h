/*
 * Mutex.h
 *
 *  Created on: Feb 21, 2014
 *      Author: xcheng
 */

#ifndef CXWCFEA_MUTEX_H_
#define CXWCFEA_MUTEX_H_

#include <pthread.h>
#include "Precompiled.h"

namespace cxwcfea {

class Mutex {
	pthread_mutex_t mutex_;
public:
	Mutex() {
		VERIFY(pthread_mutex_init(&mutex_, nullptr));
	}

	Mutex(const Mutex &) = delete;
	auto operator=(const Mutex &) -> Mutex & = delete;

	~Mutex() {
		VERIFY(pthread_mutex_destroy(&mutex_));
	}

	auto lock() noexcept -> void {
		VERIFY(pthread_mutex_lock(&mutex_));
	}

	auto unlock() noexcept -> void {
		VERIFY(pthread_mutex_unlock(&mutex_));
	}

	auto getPthreadMutex() noexcept -> pthread_mutex_t * {
		return &mutex_;
	}
};

class MutexLockGuard {
private:
	Mutex &mutex_;
public:
	explicit MutexLockGuard(Mutex &mutex) :
			mutex_(mutex) {
		mutex_.lock();
	}

	~MutexLockGuard() {
		mutex_.unlock();
	}

	MutexLockGuard(const MutexLockGuard &) = delete;
	auto operator=(const MutexLockGuard &) -> MutexLockGuard & = delete;
};

} /* namespace cxwcfea */

#endif /* CXWCFEA_MUTEX_H_ */

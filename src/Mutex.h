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
#include "CurrentThread.h"

namespace cxwcfea {

class Mutex {
public:
	Mutex() : holder_(0) {
		VERIFY(pthread_mutex_init(&mutex_, nullptr) == 0);
	}

	Mutex(const Mutex &) = delete;
	auto operator=(const Mutex &) -> Mutex & = delete;

	~Mutex() {
		ASSERT(holder_ == 0);
		VERIFY(pthread_mutex_destroy(&mutex_) == 0);
	}

	auto lock() noexcept -> void {
		VERIFY(pthread_mutex_lock(&mutex_) == 0);
		assignHolder();
	}

	auto unlock() noexcept -> void {
		unassignHolder();
		VERIFY(pthread_mutex_unlock(&mutex_) == 0);
	}

	auto getPthreadMutex() noexcept -> pthread_mutex_t * {
		return &mutex_;
	}

	auto isLockedByThisThread() const -> bool {
		return holder_ == CurrentThread::tid();
	}

	auto assertLocked() const -> void {
		ASSERT(isLockedByThisThread());
	}

	auto unassignHolder() noexcept -> void {
		holder_ = 0;
	}

	auto assignHolder() -> void {
		holder_ = CurrentThread::tid();
	}
private:
	friend class Condition;

	class UnassignGuard {
	public:
		UnassignGuard(Mutex &owner) : owner_(owner) {
			owner_.unassignHolder();
		}

		~UnassignGuard() {
			owner_.assignHolder();
		}

	private:
		Mutex &owner_;
	};


	pthread_mutex_t mutex_;
	pid_t holder_;
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

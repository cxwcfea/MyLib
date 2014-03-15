/*
 * BlockingQueue.h
 *
 *  Created on: Mar 14, 2014
 *      Author: xcheng
 */

#ifndef CXWCFEA_BLOCKINGQUEUE_H_
#define CXWCFEA_BLOCKINGQUEUE_H_

#include <queue>
#include "Precompiled.h"
#include "Mutex.h"
#include "Condition.h"

namespace cxwcfea {

template<typename T>
class BlockingQueue {
public:
	BlockingQueue() :
		mutex_(),
		notEmpty_(mutex_),
		queue_() {
	}

	auto put(const T &value) -> void {
		MutexLockGuard lock(mutex_);
		queue_.push_back(value);
		notEmpty_.notify();
	}

	auto take() -> T {
		MutexLockGuard lock(mutex_);
		while (queue_.empty()) {
			notEmpty_.wait();
		}
		ASSERT(!queue_.empty());
		T front { queue_.front() };
		queue_.pop_front();
		return front;
	}

	auto size() const -> size_t {
		MutexLockGuard lock(mutex_);
		return queue_.size();
	}

private:
	Mutex mutex_;
	Condition notEmpty_;
	std::deque<T> queue_;
};

}  // namespace cxwcfea

#endif /* CXWCFEA_BLOCKINGQUEUE_H_ */

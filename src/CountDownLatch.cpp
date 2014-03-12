/*
 * CountDownLatch.cpp
 *
 *  Created on: Mar 6, 2014
 *      Author: xcheng
 */

#include "CountDownLatch.h"

namespace cxwcfea {

CountDownLatch::CountDownLatch(int count) :
	mutex_(),
	condition_(mutex_),
	count_(count) {
}

CountDownLatch::~CountDownLatch() {
}

auto CountDownLatch::wait() -> void {
	MutexLockGuard lock(mutex_);
	while (count_ > 0) {
		condition_.wait();
	}
}

auto CountDownLatch::countDown() -> void {
	MutexLockGuard lock(mutex_);
	--count_;
	if (count_ == 0) {
		condition_.notifyAll();
	}
}

auto CountDownLatch::getCount() const -> int {
	MutexLockGuard lock(mutex_);
	return count_;
}

} /* namespace cxwcfea */

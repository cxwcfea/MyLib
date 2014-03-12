/*
 * Condition.h
 *
 *  Created on: Feb 22, 2014
 *      Author: xcheng
 */

#ifndef CXWCFEA_CONDITION_H_
#define CXWCFEA_CONDITION_H_

#include <pthread.h>
#include "Mutex.h"

namespace cxwcfea {

class Condition {
public:
	explicit Condition(Mutex &lock) :
			lock_(lock) {
		VERIFY(pthread_cond_init(&pcond_, nullptr) == 0);
	}

	~Condition() {
		VERIFY(pthread_cond_destroy(&pcond_) == 0);
	}

	Condition(const Condition &) = delete;
	auto operator=(const Condition &) -> Condition & = delete;

	auto wait() -> void {
		VERIFY(pthread_cond_wait(&pcond_, lock_.getPthreadMutex()) == 0);
	}

	auto notify() -> void {
		VERIFY(pthread_cond_signal(&pcond_) == 0);
	}

	auto notifyAll() -> void {
		VERIFY(pthread_cond_broadcast(&pcond_) == 0);
	}

	auto waitForSeconds(int seconds) -> bool;

private:
	Mutex &lock_;
	pthread_cond_t pcond_;
};

} /* namespace cxwcfea */

#endif /* CXWCFEA_CONDITION_H_ */

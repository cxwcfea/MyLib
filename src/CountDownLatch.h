/*
 * CountDownLatch.h
 *
 *  Created on: Mar 6, 2014
 *      Author: xcheng
 */

#ifndef CXWCFEA_COUNTDOWNLATCH_H_
#define CXWCFEA_COUNTDOWNLATCH_H_

#include "Mutex.h"
#include "Condition.h"
#include "Precompiled.h"

namespace cxwcfea {

class CountDownLatch {
public:
	explicit CountDownLatch(int count);
	~CountDownLatch();

	CountDownLatch(const CountDownLatch &) = delete;
	auto operator=(const CountDownLatch &) -> CountDownLatch = delete;

	auto wait() -> void;
	auto countDown() -> void;
	auto getCount() const -> int;

private:
	mutable Mutex mutex_;
	Condition condition_;
	int count_;
};

} /* namespace cxwcfea */

#endif /* CXWCFEA_COUNTDOWNLATCH_H_ */

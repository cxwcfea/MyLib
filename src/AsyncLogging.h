/*
 * AsyncLogging.h
 *
 *  Created on: Mar 10, 2014
 *      Author: xcheng
 */

#ifndef CXWCFEA_ASYNCLOGGING_H_
#define CXWCFEA_ASYNCLOGGING_H_

#include "Thread.h"
#include "Mutex.h"
#include "Condition.h"
#include "CountDownLatch.h"
#include "LogStream.h"
#include "Precompiled.h"

namespace cxwcfea {

class AsyncLogging {
public:
	AsyncLogging(const string &basename, size_t rollSize, int flushIntervale = 3);
	~AsyncLogging() {
		if (running_) {
			stop();
		}
	}

	AsyncLogging(const AsyncLogging &) = delete;
	auto operator=(const AsyncLogging &) -> AsyncLogging & = delete;

	auto start() -> void {
		running_ = true;
		thread_.start();
		latch_.wait();
	}

	auto append(const char *logLine, int len) -> void;

	auto stop() -> void {
		running_ = false;
		condition_.notify();
		thread_.join();
	}
private:
	auto threadFunc() -> void;

	using Buffer = detail::FixedBuffer<detail::kLargeBuffer>;
	using BufferVector = std::vector<unique_ptr<Buffer>>;
	using BufferPtr = BufferVector::value_type;

	const int flushInterval_;
	bool running_;
	string basename_;
	size_t rollSize_;
	Thread thread_;
	CountDownLatch latch_;
	Mutex mutex_;
	Condition condition_;
	BufferPtr currentBuffer_;
	BufferPtr nextBuffer_;
	BufferVector buffers_;
};

} /* namespace cxwcfea */

#endif /* ASYNCLOGGING_H_ */

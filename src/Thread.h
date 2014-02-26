/*
 * Thread.h
 *
 *  Created on: Feb 23, 2014
 *      Author: xcheng
 */

#ifndef CXWCFEA_THREAD_H_
#define CXWCFEA_THREAD_H_

#include <atomic>
#include <functional>
#include <pthread.h>
#include "Precompiled.h"
#include "CurrentThread.h"

namespace cxwcfea {

class Thread {
public:
	using ThreadFunc = std::function<void()>;

	explicit Thread(const ThreadFunc &func, const string &name = string());
	explicit Thread(ThreadFunc &&func, const string &name = string());

	Thread(const Thread &) = delete;
	auto operator=(const Thread &) -> Thread & = delete;

	~Thread();

	auto start() -> void;
	auto join() -> int;

	auto started() const noexcept -> bool {
		return started_;
	}

	auto getName() const noexcept -> const string & {
		return name_;
	}

	auto tid() const noexcept -> pid_t {
		return *tid_;
	}

	static auto numCreated() noexcept -> int {
		return numCreated_;
	}

private:
	bool started_;
	bool joined_;
	pthread_t threadId_;
	shared_ptr<pid_t> tid_;
	ThreadFunc func_;
	string name_;

	static std::atomic<std::int32_t> numCreated_;
};

} /* namespace cxwcfea */

#endif /* CXWCFEA_THREAD_H_ */

/*
 * ThreadPool.h
 *
 *  Created on: Mar 13, 2014
 *      Author: xcheng
 */

#ifndef THREADPOOL_H_
#define THREADPOOL_H_

#include <functional>
#include <queue>
#include "Precompiled.h"
#include "Mutex.h"
#include "Condition.h"
#include "Thread.h"

namespace cxwcfea {

class ThreadPool {
public:
	explicit ThreadPool(const string &name = string());
	~ThreadPool();

	ThreadPool(const ThreadPool &) = delete;
	auto operator=(const ThreadPool &) -> ThreadPool & = delete;

	using Task = std::function<void()>;

	auto start(int numThreads) -> void;
	auto stop() -> void;

	auto run(const Task &) -> void;
	auto run(Task &&) -> void;

	auto setMaxQueueSize(int maxSize) -> void { maxQueueSize_= maxSize; }

private:
	auto isFull() const -> bool;
	auto runInThread() -> void;
	auto take() -> Task;

	Mutex mutex_;
	Condition notEmpty_;
	Condition notFull_;
	string name_;
	std::vector<std::unique_ptr<Thread>> threads_;
	std::deque<Task> queue_;
	size_t maxQueueSize_;
	bool running_;
};

} /* namespace cxwcfea */

#endif /* THREADPOOL_H_ */

/*
 * ThreadPool.cpp
 *
 *  Created on: Mar 13, 2014
 *      Author: xcheng
 */

#include "ThreadPool.h"
#include "Exception.h"

namespace cxwcfea {

ThreadPool::ThreadPool(const string &name)
	: mutex_(),
	  notEmpty_(mutex_),
	  notFull_(mutex_),
	  name_(name),
	  maxQueueSize_(0),
	  running_(false) {
}

ThreadPool::~ThreadPool() {
	if (running_) {
		stop();
	}
}

auto ThreadPool::start(int numThreads) -> void {
	ASSERT(threads_.empty());
	running_ = true;
	threads_.reserve(numThreads);
	for (int i = 0; i < numThreads; ++i) {
        char id[32];
        snprintf(id, sizeof id, "%d", i);
        threads_.emplace_back(new Thread(std::bind(&ThreadPool::runInThread, this), name_ + id));
        threads_[i]->start();
	}
}

auto ThreadPool::stop() -> void {
	{
		MutexLockGuard lock(mutex_);
		running_ = false;
		notEmpty_.notifyAll();
	}
	for (auto &thread : threads_) {
		thread->join();
	}
}

auto ThreadPool::isFull() const -> bool {
	mutex_.assertLocked();
	return maxQueueSize_ > 0 && queue_.size() >= maxQueueSize_;
}

auto ThreadPool::run(const Task &task) -> void {
	if (threads_.empty()) {
		task();
	} else {
		MutexLockGuard lock(mutex_);
		if (isFull()) {
			notFull_.wait();
		}
		ASSERT(!isFull());

		queue_.emplace_back(task);
		notEmpty_.notify();
	}
}

auto ThreadPool::run(Task &&task) -> void {
	if (threads_.empty()) {
		task();
	} else {
		MutexLockGuard lock(mutex_);
		if (isFull()) {
			notFull_.wait();
		}
		ASSERT(!isFull());

		queue_.emplace_back(task);
		notEmpty_.notify();
	}
}

auto ThreadPool::take() -> ThreadPool::Task {
	MutexLockGuard lock(mutex_);
	while (queue_.empty() && running_) {
		notEmpty_.wait();
	}
	Task task;
	if (!queue_.empty()) {
		task = queue_.front();
		queue_.pop_front();
		if (maxQueueSize_ > 0) {
			notFull_.notify();
		}
	}
	return task;
}

auto ThreadPool::runInThread() -> void {
	try {
		while (running_) {
			Task task { take() };
			if (task) {
				task();
			}
		}
	} catch (const Exception &ex) {
		fprintf(stderr, "exception caught in ThreadPool %s\n", name_.c_str());
		fprintf(stderr, "reason: %s\n", ex.what());
		fprintf(stderr, "stack trace: %s\n", ex.stackTrace());
		abort();
	} catch (const std::exception &ex) {
		fprintf(stderr, "exception caught in ThreadPool %s\n", name_.c_str());
		fprintf(stderr, "reason: %s\n", ex.what());
		abort();
	} catch (...) {
		fprintf(stderr, "unknown exception caught in ThreadPool %s\n",
				name_.c_str());
		throw; // rethrow
	}
}

} /* namespace cxwcfea */

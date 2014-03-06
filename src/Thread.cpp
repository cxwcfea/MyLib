/*
 * Thread.cpp
 *
 *  Created on: Feb 23, 2014
 *      Author: xcheng
 */

#include <unistd.h>
#include <sys/syscall.h>
#include <sys/types.h>
#ifndef __MACH__
#include <linux/unistd.h>
#endif
#include "Timestamp.h"
#include "Thread.h"
#include "Exception.h"
#include "Logging.h"

namespace cxwcfea {

namespace CurrentThread {
__thread int t_cachedTid = 0;
__thread char t_tidString[32];
__thread const char *t_threadName = "unknown";
const bool sameType = std::is_same<int, pid_t>::value;
static_assert(sameType, "int and pid_t are not same type");
}

namespace detail {

#ifndef __MACH__
auto gettid() -> pid_t {
	return static_cast<pid_t>(::syscall(SYS_gettid));
}
#else
auto gettid() -> pid_t {
	return pthread_mach_thread_np(pthread_self());
}
#endif

auto afterfork() -> void {
	CurrentThread::t_cachedTid = 0;
	CurrentThread::t_threadName = "main";
	CurrentThread::tid();
}

struct ThreadNameInitializer {
	ThreadNameInitializer() {
		CurrentThread::t_threadName = "main";
		CurrentThread::tid();
		pthread_atfork(nullptr, nullptr, &afterfork);
	}
};

ThreadNameInitializer init;

struct ThreadData {
	using ThreadFunc = Thread::ThreadFunc;
	ThreadFunc func_;
	string name_;
	weak_ptr<pid_t> wkTid_;

	ThreadData(const ThreadFunc &func, const string &name,
			const shared_ptr<pid_t> &tid) :
			func_(func), name_(name), wkTid_(tid) {
	}

	auto runInThread() -> void {
		pid_t tid = CurrentThread::tid();

		shared_ptr<pid_t> ptid = wkTid_.lock();
		if (ptid) {
			*ptid = tid;
			ptid.reset();
		}

		CurrentThread::t_threadName = name_.c_str();
		try {
			func_();
			CurrentThread::t_threadName = "finished";
		} catch (const Exception &ex) {
			CurrentThread::t_threadName = "crashed";
			std::cerr << "exception caught in Thread " << name_ << std::endl;
			std::cerr << "reason: " << ex.what() << std::endl;
			std::cerr << "stack trace: " << ex.stackTrace() << std::endl;
			abort();
		} catch (const std::exception &ex) {
			CurrentThread::t_threadName = "crashed";
			std::cerr << "exception caught in Thread " << name_ << std::endl;
			std::cerr << "reason: " << ex.what() << std::endl;
			abort();
		} catch (...) {
			CurrentThread::t_threadName = "crashed";
			std::cerr << "unknown exception caught in Thread " << name_
					<< std::endl;
			throw;
		}
	}
};

auto startThread(void *obj) -> void * {
	ThreadData *data = static_cast<ThreadData *>(obj);
	data->runInThread();
	delete data;
	return nullptr;
}
}

namespace CurrentThread {
auto cacheTid() -> void {
	if (t_cachedTid == 0) {
		t_cachedTid = detail::gettid();
		int n = snprintf(t_tidString, sizeof(t_tidString), "%5d ", t_cachedTid);
		ASSERT(n == 6);
	}
}

auto isMainThread() -> bool {
	return tid() == getpid();
}

auto sleepUsec(int64_t usec) -> void {
	struct timespec time { 0, 0 };
	time.tv_sec = static_cast<time_t>(usec / Timestamp::kMicroSecondsPerSecond);
	time.tv_nsec = static_cast<long>(usec % Timestamp::kMicroSecondsPerSecond
			* 1000);
	nanosleep(&time, nullptr);
}
}

std::atomic<int32_t> Thread::numCreated_;

Thread::Thread(const ThreadFunc &func, const string &name) :
		started_(false), joined_(false), threadId_(0), tid_(new pid_t(0)), func_(
				func), name_(name) {
	numCreated_.fetch_add(1);
}

Thread::Thread(ThreadFunc &&func, const string &name) :
		started_(false), joined_(false), threadId_(0), tid_(new pid_t(0)), func_(
				func), name_(name) {
	numCreated_.fetch_add(1);
}

Thread::~Thread() {
	if (started_ && !joined_) {
		pthread_detach(threadId_);
	}
}

auto Thread::start() -> void {
	ASSERT(!started_);
	started_ = true;
	detail::ThreadData *data = new detail::ThreadData(func_, name_, tid_);
	if (pthread_create(&threadId_, nullptr, &detail::startThread, data)) {
		started_ = false;
		delete data;
		//TODO: use log func
		LOG_SYSFATAL << "Failed in pthread_create";
	}
}

auto Thread::join() -> int {
	ASSERT(started_);
	ASSERT(!joined_);
	joined_ = true;
	return pthread_join(threadId_, nullptr);
}

} /* namespace cxwcfea */


/*
 * CurrentThread.h
 *
 *  Created on: Feb 23, 2014
 *      Author: xcheng
 */

#ifndef CXWCFEA_CURRENTTHREAD_H_
#define CXWCFEA_CURRENTTHREAD_H_

namespace cxwcfea {

namespace CurrentThread {
	extern __thread int t_cachedTid;
	extern __thread char t_tidString[32];
	extern __thread const char *t_threadName;
	auto cacheTid() -> void;

	inline auto tid() -> int {
		if (__builtin_expect(t_cachedTid == 0, 0)) {
			cacheTid();
		}
		return t_cachedTid;
	}

	inline auto tidString() noexcept -> const char * {
		return t_tidString;
	}

	inline auto name() noexcept -> const char * {
		return t_threadName;
	}

	auto isMainThread() -> bool;

	auto sleepUsec(int64_t usec) -> void;

}  // namespace CurrentThread


}  // namespace cxwcfea



#endif /* CXWCFEA_CURRENTTHREAD_H_ */

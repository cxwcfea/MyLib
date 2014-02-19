/*
 * Precompiled.h
 *
 *  Created on: Feb 17, 2014
 *      Author: xcheng
 */

#ifndef CXWCFEA_PRECOMPILED_H_
#define CXWCFEA_PRECOMPILED_H_

#include <cstdio>
#include <cstdint>
#include <cassert>
#include <cerrno>
#include <iostream>
#include <string>
#include <algorithm>
#include <utility>
#include <vector>
#include <type_traits>

#define ASSERT assert

using std::string;

struct Tracer {
	const char *file_name_;
	unsigned line_num_;

	static const int BUF_SIZE = 256;

	Tracer(const char *file_name, unsigned line_num) :
			file_name_(file_name), line_num_(line_num) {

	}

	auto operator()(const char *format, ...) const -> void {
		char buf[BUF_SIZE];
		auto count = snprintf(buf, BUF_SIZE, "%s(%d): ", file_name_, line_num_);
		ASSERT(-1 != count);
		va_list args;
		va_start(args, format);
		ASSERT(-1 != vsnprintf(buf+count, BUF_SIZE-count, format, args));
		std::cout << buf;
		va_end(args);
	}
};

#ifdef NDEBUG
#define VERIFY(expression) (expression)
#define TRACE
#else
#define VERIFY ASSERT
#define TRACE Tracer(__FILE__, __LINE__)
#endif

template<class T, class ... Ts>
std::unique_ptr<T> make_unique(Ts&&... ts) {
	return std::unique_ptr<T>(new T(std::forward<Ts>(ts)...));
}

#endif /* CXWCFEA_PRECOMPILED_H_ */

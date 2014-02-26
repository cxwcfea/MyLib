/*
 * Exception.cpp
 *
 *  Created on: Feb 24, 2014
 *      Author: xcheng
 */

#include <execinfo.h>
#include <iterator>
#include "Exception.h"

using namespace std;

namespace cxwcfea {

Exception::Exception(const char *msg) : message_(msg) {
	fillStackTrace();
}

Exception::Exception(const string& msg) :
		Exception(msg.c_str()) {
}

Exception::~Exception() {
}

auto Exception::what() const noexcept -> const char * {
	return message_.c_str();
}

auto Exception::stackTrace() const noexcept -> const char * {
	return stack_.c_str();
}

auto Exception::fillStackTrace() -> void {
	constexpr int len = 200;
	void *buffer[len];
	int frames = backtrace(buffer, len);
	char **strings = backtrace_symbols(buffer, frames);
	if (strings) {
		for (int i = 0; i < frames; ++i) {
			stack_.append(strings[i]);
			stack_.push_back('\n');
		}
		free(strings);
	}
}

} /* namespace cxwcfea */

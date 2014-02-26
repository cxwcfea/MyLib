/*
 * Exception.h
 *
 *  Created on: Feb 24, 2014
 *      Author: xcheng
 */

#ifndef EXCEPTION_H_
#define EXCEPTION_H_

#include <exception>
#include "Precompiled.h"

namespace cxwcfea {

class Exception: public std::exception {
public:
	Exception(const char *what);
	Exception(const string &what);
	virtual ~Exception();
	virtual auto what() const noexcept -> const char *;
	auto stackTrace() const noexcept -> const char *;
private:
	auto fillStackTrace() -> void;

	string message_;
	string stack_;
};

} /* namespace cxwcfea */

#endif /* EXCEPTION_H_ */

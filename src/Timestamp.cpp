/*
 * Timestamp.cpp
 *
 *  Created on: Feb 17, 2014
 *      Author: xcheng
 */

#include <cinttypes>
#include <sys/time.h>
#include "Timestamp.h"

namespace cxwcfea {

static_assert(sizeof(Timestamp) == sizeof(int64_t), "Timestamp size not equal to 8 byte");

Timestamp::Timestamp(int64_t microseconds) :
		microsecondsSinceEpoch_(microseconds) {

}

string Timestamp::toString() {
	char buf[32] = { '\0' };
	int64_t seconds = microsecondsSinceEpoch_ / kMicroSecondsPerSecond;
	int64_t microseconds = microsecondsSinceEpoch_ % kMicroSecondsPerSecond;
	snprintf(buf, sizeof(buf), "%" PRId64 ".%06" PRId64, seconds, microseconds);
	return buf;
}

string Timestamp::toFormattedString() {
	char buf[32] = { '\0' };
	std::time_t seconds = static_cast<std::time_t>(microsecondsSinceEpoch_
			/ kMicroSecondsPerSecond);
	int64_t microseconds = microsecondsSinceEpoch_ % kMicroSecondsPerSecond;
	std::tm result;
	gmtime_r(&seconds, &result);

	snprintf(buf, sizeof(buf), "%04d%02d%02d %02d:%02d:%02d.%06" PRId64,
			result.tm_year + 1900, result.tm_mon + 1, result.tm_mday,
			result.tm_hour, result.tm_min, result.tm_sec, microseconds);

	return buf;
}

Timestamp Timestamp::now() {
	struct timeval result;
	gettimeofday(&result, nullptr);
	return Timestamp(result.tv_sec * kMicroSecondsPerSecond + result.tv_usec);
}

Timestamp Timestamp::invalid() {
	return Timestamp();
}

Timestamp::~Timestamp() {
}

} /* namespace cxwcfea */

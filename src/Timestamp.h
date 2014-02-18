/*
 * Timestamp.h
 *
 *  Created on: Feb 17, 2014
 *      Author: xcheng
 */

#ifndef CXWCFEA_TIMESTAMP_H_
#define CXWCFEA_TIMESTAMP_H_

#include <ctime>
#include "Precompiled.h"

namespace cxwcfea {

class Timestamp {
	int64_t microsecondsSinceEpoch_;
public:
	explicit Timestamp(int64_t microseconds = 0);
	~Timestamp();

	static constexpr int kMicroSecondsPerSecond = 1000 * 1000;

	string toString();
	string toFormattedString();
	static Timestamp now();
	static Timestamp invalid();

	bool valid() const noexcept {
		return microsecondsSinceEpoch_ > 0;
	}
	int64_t microsecondsSinceEpoch() const noexcept {
		return microsecondsSinceEpoch_;
	}
	std::time_t secondsSinceEpoch() const noexcept {
		return static_cast<time_t>(microsecondsSinceEpoch_
				/ kMicroSecondsPerSecond);
	}
};

inline bool operator<(Timestamp left, Timestamp right) {
	return left.microsecondsSinceEpoch() < left.microsecondsSinceEpoch();
}

inline bool operator==(Timestamp left, Timestamp right) {
	return left.microsecondsSinceEpoch() == left.microsecondsSinceEpoch();
}

inline bool operator<=(Timestamp left, Timestamp right) {
	return left.microsecondsSinceEpoch() <= left.microsecondsSinceEpoch();
}

inline bool operator>=(Timestamp left, Timestamp right) {
	return left.microsecondsSinceEpoch() >= left.microsecondsSinceEpoch();
}

inline bool operator!=(Timestamp left, Timestamp right) {
	return left.microsecondsSinceEpoch() != left.microsecondsSinceEpoch();
}

inline double timeDifferenceInSeconds(Timestamp left, Timestamp right) {
	int64_t diff = left.microsecondsSinceEpoch()
			- right.microsecondsSinceEpoch();
	return static_cast<double>(diff) / Timestamp::kMicroSecondsPerSecond;
}

inline Timestamp addTimeBySeconds(Timestamp timestamp, double seconds) {
	int64_t delta = static_cast<int64_t>(seconds
			* Timestamp::kMicroSecondsPerSecond);
	return Timestamp(timestamp.microsecondsSinceEpoch() + delta);
}

} /* namespace cxwcfea */

#endif /* CXWCFEA_TIMESTAMP_H_ */

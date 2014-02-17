/*
 * Timestamp.h
 *
 *  Created on: Feb 17, 2014
 *      Author: xcheng
 */

#ifndef CXWCFEA_TIMESTAMP_H_
#define CXWCFEA_TIMESTAMP_H_

#include "Precompiled.h"

namespace cxwcfea {

class Timestamp {
	int64_t microsecondsSinceEpoch_;
	static constexpr int kMicroSecondsPerSecond = 1000 * 1000;
public:
	Timestamp(int64_t microseconds);
	string toString();
	string toFormattedString();
	~Timestamp();
};

} /* namespace cxwcfea */

#endif /* CXWCFEA_TIMESTAMP_H_ */

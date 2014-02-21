/*
 * Date.h
 *
 *  Created on: Feb 20, 2014
 *      Author: xcheng
 */

#ifndef CXWCFEA_DATE_H_
#define CXWCFEA_DATE_H_

#include "Precompiled.h"

namespace cxwcfea {

class Date {
public:

	struct YearMonthDay {
		int year;
		int month;
		int day;
	};

	static constexpr int kDaysPerWeek = 7;
	static const int kJulianDayOf1970_01_01;

	Date() :
			julianDayNumber_(0) {
	}

	Date(int year, int month, int day);

	explicit Date(int julianDayNumber) :
			julianDayNumber_(julianDayNumber) {
	}

	explicit Date(struct tm &);

	auto invalid() const noexcept -> bool {
		return julianDayNumber_ > 0;
	}

	auto toIsoString() const noexcept -> string;

	auto yearMonthDay() const noexcept -> YearMonthDay;

	auto year() const noexcept -> int {
		return yearMonthDay().year;
	}

	auto month() const noexcept -> int {
		return yearMonthDay().month;
	}

	auto day() const noexcept -> int {
		return yearMonthDay().day;
	}

	auto weekday() const noexcept -> int {
		return (julianDayNumber_ + 1) % kDaysPerWeek;
	}

	auto julianDayNumber() const noexcept -> int {
		return julianDayNumber_;
	}

private:
	int julianDayNumber_;
}
;

} /* namespace cxwcfea */

#endif /* CXWCFEA_DATE_H_ */

/*
 * Date.cpp
 *
 *  Created on: Feb 20, 2014
 *      Author: xcheng
 */

#include "Date.h"

namespace cxwcfea {

namespace detail {
static_assert(sizeof(int) >= sizeof(int32_t), "require_32_bit_integer_at_least");

// algorithm and explanation see:
// http://www.faqs.org/faqs/calendars/faq/part2/
// http://blog.csdn.net/Solstice

int getJulianDayNumber(int year, int month, int day) {
	int a = (14 - month) / 12;
	int y = year + 4800 - a;
	int m = month + 12 * a - 3;
	return day + (153 * m + 2) / 5 + y * 365 + y / 4 - y / 100 + y / 400 - 32045;
}

struct Date::YearMonthDay getYearMonthDay(int julianDayNumber) {
	int a = julianDayNumber + 32044;
	int b = (4 * a + 3) / 146097;
	int c = a - ((b * 146097) / 4);
	int d = (4 * c + 3) / 1461;
	int e = c - ((1461 * d) / 4);
	int m = (5 * e + 2) / 153;
	Date::YearMonthDay ymd;
	ymd.day = e - ((153 * m + 2) / 5) + 1;
	ymd.month = m + 3 - 12 * (m / 10);
	ymd.year = b * 100 + d - 4800 + (m / 10);
	return ymd;
}
}

using namespace detail;

const int Date::kJulianDayOf1970_01_01 = detail::getJulianDayNumber(1970, 1, 1);

Date::Date(int year, int month, int day) :
		julianDayNumber_(getJulianDayNumber(year, month, day)) {
}

Date::Date(struct tm& date) :
		julianDayNumber_(
				getJulianDayNumber(date.tm_year + 1900, date.tm_mon + 1,
						date.tm_mday)) {
}

auto Date::toIsoString() const noexcept -> string {
	char buf[32];
	YearMonthDay ymd(yearMonthDay());
	snprintf(buf, 32, "%4d-%02d-%02d", ymd.year, ymd.month, ymd.day);
	return buf;
}

auto Date::yearMonthDay() const noexcept -> YearMonthDay {
	return getYearMonthDay(julianDayNumber_);
}

} /* namespace cxwcfea */

/*
 * LogFile.cpp
 *
 *  Created on: Feb 27, 2014
 *      Author: xcheng
 */

#include <ctime>
#include "LogFile.h"
#include "ProcessInfo.h"

namespace cxwcfea {

LogFile::LogFile(const string &basename, size_t rollSize, bool threadSafe,
		int flushInterval) :
		basename_("basename"), rollSize_(rollSize), flushInterval_(
				flushInterval), count_(0), mutex_(
				threadSafe ? new Mutex : nullptr), startOfPeriod_(0), lastRoll_(
				0), lastFlush_(0) {
	ASSERT(basename.find('/') == string::npos);
	rollFile();
}

LogFile::~LogFile() {
}

auto LogFile::append(const char *logLine, int len) -> void {
	if (mutex_) {
		MutexLockGuard lock(*mutex_);
		append_unlocked(logLine, len);
	} else {
		append_unlocked(logLine, len);
	}
}

auto LogFile::flush() -> void {
	if (mutex_) {
		MutexLockGuard lock(*mutex_);
		file_->flush();
	} else {
		file_->flush();
	}
}

auto LogFile::append_unlocked(const char *logLine, int len) -> void {
	file_->append(logLine, len);

	if (file_->writtenBytes() > rollSize_) {
		rollFile();
	} else {
		if (count_ > kCheckTimeRoll_) {
			count_ = 0;
			time_t now = time(nullptr);
			time_t thisPeriod = now / kRollPerSeconds_ * kRollPerSeconds_;
			if (thisPeriod > startOfPeriod_) {
				rollFile();
			} else if (now - lastFlush_ > flushInterval_) {
				lastFlush_ = now;
				file_->flush();
			}
		} else {
			++count_;
		}
	}
}

auto LogFile::rollFile() -> void {
	time_t now = 0;
	string filename = getLogFileName(basename_, &now);
	time_t start = now / kRollPerSeconds_ * kRollPerSeconds_;

	if (now > lastRoll_) {
		lastRoll_ = now;
		lastFlush_ = now;
		startOfPeriod_ = start;
		file_.reset(new AppendFile(filename));
	}
}

auto LogFile::getLogFileName(const string& basename, time_t* now) -> string {
	string filename;
	filename.reserve(basename.size() + 64);
	filename = basename;

	char timebuf[32];
	*now = time(nullptr);
	struct tm tm;
	gmtime_r(now, &tm);
	strftime(timebuf, sizeof(timebuf) - 1, ".%Y%m%d-%H%M%S.", &tm);
	filename += timebuf;

	filename += ProcessInfo::hostName();

	char pidbuf[32];
	snprintf(pidbuf, sizeof pidbuf, ".%d", ProcessInfo::pid());
	filename += pidbuf;

	filename += ".log";

	return filename;
}

} /* namespace cxwcfea */

/*
 * LogFile.h
 *
 *  Created on: Feb 27, 2014
 *      Author: xcheng
 */

#ifndef CXWCFEA_LOGFILE_H_
#define CXWCFEA_LOGFILE_H_

#include "Precompiled.h"
#include "FileUtil.h"
#include "Mutex.h"

namespace cxwcfea {

class LogFile {
public:
	LogFile(const string &basename, size_t rollSize, bool threadSafe = true,
			int flushInterval = 3);
	~LogFile();

	LogFile(const LogFile &) = delete;
	auto operator=(const LogFile &) -> LogFile & = delete;

	auto append(const char *logline, int len) -> void;
	auto flush() -> void;

private:
	auto append_unlocked(const char *logline, int len) -> void;

	static auto getLogFileName(const string &basename, time_t *now) -> string;
	auto rollFile() -> void;

	const string &basename_;
	const size_t rollSize_;
	const int flushInterval_;

	int count_;

	shared_ptr<Mutex> mutex_;
	shared_ptr<AppendFile> file_;
	time_t startOfPeriod_;
	time_t lastRoll_;
	time_t lastFlush_;

	constexpr static int kCheckTimeRoll_ = 1024;
	constexpr static int kRollPerSeconds_ = 60 * 60 * 24;
};

} /* namespace cxwcfea */

#endif /* CXWCFEA_LOGFILE_H_ */

/*
 * Logging.cpp
 *
 *  Created on: Mar 3, 2014
 *      Author: xcheng
 */

#include "Logging.h"
#include "CurrentThread.h"

namespace cxwcfea {

__thread char t_errnobuf[512];
__thread char t_time[32];
__thread time_t t_lastSecond;

Logger::LogLevel initLogLevel() {
	if (::getenv("CX_LOG_TRACE")) {
		return Logger::TRACE;
	} else if (::getenv("CX_LOG_DEBUG")) {
		return Logger::DEBUG;
	} else {
		return Logger::INFO;
	}
}

Logger::LogLevel g_logLevel = initLogLevel();

const char *LogLevelName[Logger::NUM_LOG_LEVELS] = {
	"TRACE ",
	"DEBUG ",
	"INFO  ",
	"WARN  ",
	"ERROR ",
	"FATAL ",
};

class T {
public:
	T(const char *str, unsigned len) :
		str_(str),
		len_(len) {
		ASSERT(strlen(str) == len);
	}
	const char *str_;
	const unsigned len_;
};

inline LogStream &operator<<(LogStream &s, T v) {
	s.append(v.str_, v.len_);
	return s;
}

inline LogStream &operator<<(LogStream &s, const Logger::SourceFile &v) {
	s.append(v.data_, v.size_);
	return s;
}

auto Logger::Impl::formatTime() -> void {
	t_lastSecond = time_.secondsSinceEpoch();
	strncpy(t_time, time_.toFormattedString().c_str(), sizeof t_time);
	stream_ << T(t_time, 24);
}

auto Logger::Impl::finish() -> void {
	stream_ << " - " << baseName_ << ":" << line_ << "\n";
}

auto defaultOutput(const char *msg, int len) -> void {
	fwrite(msg, 1, len, stdout);
}

auto defaultFlush() -> void {
	fflush(stdout);
}

Logger::OutputFunc g_output = defaultOutput;
Logger::FlushFunc g_flush = defaultFlush;

Logger::Impl::Impl(LogLevel level, int savedErrno, const SourceFile &baseName, int line) :
	time_(Timestamp::now()),
	stream_(),
	level_(level),
	line_(line),
	baseName_(baseName) {
	formatTime();
	CurrentThread::tid();
	stream_ << T(CurrentThread::tidString(), 6);
	stream_ << T(LogLevelName[level], 6);
	if (savedErrno != 0) {
		stream_ << strerror_tl(savedErrno) << " (errno=" << savedErrno << ") ";
	}
}

Logger::Logger(SourceFile file, int line, LogLevel level) :
	impl_(level, 0, file, line) {
}

Logger::Logger(SourceFile file, int line, LogLevel level, const char *func)
	: Logger(file, line, level) {
	impl_.stream_ << func << ' ';
}

Logger::Logger(SourceFile file, int line, bool toAbort)
	: impl_(toAbort?FATAL:ERROR, errno, file, line) {
}

auto strerror_tl(int savedErrno) -> const char * {
#ifndef __MACH__
	return strerror_r(savedErrno, t_errnobuf, sizeof t_errnobuf);
#else
	strerror_r(savedErrno, t_errnobuf, sizeof t_errnobuf);
	return t_errnobuf;
#endif
}

auto Logger::setLogLevel(LogLevel level) -> void {
	g_logLevel = level;
}

inline Logger::~Logger() {
	impl_.finish();
	const LogStream::Buffer &buf(stream().buffer());
	g_output(buf.data(), buf.length());
	if (impl_.level_ == FATAL) {
		g_flush();
		abort();
	}
}

auto Logger::setOutput(OutputFunc out) -> void {
	g_output = out;
}

auto Logger::setFlush(FlushFunc flush) -> void {
	g_flush = flush;
}

}  // namespace cxwcfea





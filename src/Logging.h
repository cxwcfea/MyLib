/*
 * Logging.h
 *
 *  Created on: Mar 3, 2014
 *      Author: xcheng
 */

#ifndef CXWCFEA_LOGGING_H_
#define CXWCFEA_LOGGING_H_

#include <functional>
#include "Precompiled.h"
#include "LogStream.h"
#include "Timestamp.h"

namespace cxwcfea {

class Logger {
public:
	enum LogLevel { TRACE, DEBUG, INFO, WARN, ERROR, FATAL, NUM_LOG_LEVELS, };

	class SourceFile {
	public:
		template<int N>
		inline SourceFile(const char (&arr)[N]) :
			data_(arr), size_(N - 1) {
			const char *slash = strrchr(data_, '/');
			if (slash) {
				data_ = slash + 1;
				size_ -= static_cast<int>(data_ - arr);
			}
		}

		explicit SourceFile(const char *fileName) :
			data_(fileName) {
			const char *slash = strrchr(data_, '/');
			if (slash) {
				data_ = slash + 1;
			}
			size_ = static_cast<int>(strlen(data_));
		}

		const char *data_;
		int size_;
	};

	Logger(SourceFile file, int line, LogLevel level = INFO);
	Logger(SourceFile file, int line, LogLevel level, const char *func);
	Logger(SourceFile file, int line, bool toAbort);
	~Logger();

	auto stream() noexcept -> LogStream & {
		return impl_.stream_;
	}

	static auto logLevel() -> LogLevel;
	static auto setLogLevel(LogLevel level) -> void;

	using OutputFunc = std::function<void(const char *, int)>;
	using FlushFunc = std::function<void()>;
	static auto setOutput(OutputFunc) -> void;
	static auto setFlush(FlushFunc) -> void;

private:
	class Impl {
	public:
		Impl(LogLevel level, int savedErrno, const SourceFile &baseName, int line);
		auto formatTime() -> void;
		auto finish() -> void;

		Timestamp time_;
		LogStream stream_;
		LogLevel level_;
		int line_;
		SourceFile baseName_;
	};

	Impl impl_;
};

extern Logger::LogLevel g_logLevel;

inline Logger::LogLevel Logger::logLevel() {
	return g_logLevel;
}

#define LOG_TRACE if (cxwcfea::Logger::logLevel() <= cxwcfea::Logger::TRACE) \
	cxwcfea::Logger(__FILE__, __LINE__, cxwcfea::Logger::TRACE, __func__).stream()
#define LOG_DEBUG if (cxwcfea::Logger::logLevel() <= cxwcfea::Logger::DEBUG) \
	cxwcfea::Logger(__FILE__, __LINE__, cxwcfea::Logger::DEBUG, __func__).stream()
#define LOG_INFO if (cxwcfea::Logger::logLevel() <= cxwcfea::Logger::INFO) \
	cxwcfea::Logger(__FILE__, __LINE__).stream()
#define LOG_WARN cxwcfea::Logger(__FILE__, __LINE__, cxwcfea::Logger::WARN).stream()
#define LOG_ERROR cxwcfea::Logger(__FILE__, __LINE__, cxwcfea::Logger::ERROR).stream()
#define LOG_FATAL cxwcfea::Logger(__FILE__, __LINE__, cxwcfea::Logger::FATAL).stream()
#define LOG_SYSERR cxwcfea::Logger(__FILE__, __LINE__, false).stream()
#define LOG_SYSFATAL cxwcfea::Logger(__FILE__, __LINE__, true).stream()

auto strerror_tl(int savedErrno) -> const char *;

#define CHECK_NOTNULL(val) \
	cxwcfea::CheckNotNull(__FILE__, __LINE__, "'" #val "' Must be non nullptr", (val))

template<typename T>
T *CheckNotNull(Logger::SourceFile file, int line, const char *name, T *ptr) {
	if (ptr == nullptr) {
		Logger(file, line, Logger::FATAL).stream() << name;
	}
	return ptr;
}

}  // namespace cxwcfea

#endif /* CXWCFEA_LOGGING_H_ */

/*
 * LogStream.h
 *
 *  Created on: Mar 2, 2014
 *      Author: xcheng
 */

#ifndef CXWCFEA_LOGSTREAM_H_
#define CXWCFEA_LOGSTREAM_H_

#include <cstring>
#include "Precompiled.h"

namespace cxwcfea {

namespace detail {

constexpr int kSmallBuffer = 4000;
constexpr int kLargeBuffer = 4000 * 1000;

template<int SIZE>
class FixedBuffer {
public:
	FixedBuffer() :
		cur_(data_) {
	}
	~FixedBuffer() = default;

	FixedBuffer(const FixedBuffer &) = delete;
	auto operator=(const FixedBuffer &) -> FixedBuffer & = delete;

	auto append(const char *buf, size_t len) -> void {
		if (static_cast<size_t>(avail()) > len) {
			memcpy(cur_, buf, len);
			cur_ += len;
		}
	}

	auto data() const noexcept -> const char * {
		return data_;
	}
	auto length() const noexcept -> int {
		return static_cast<int>(cur_ - data_);
	}

	auto current() const noexcept -> char * {
		return cur_;
	}
	auto avail() const noexcept -> int {
		return static_cast<int>(end() - cur_);
	}
	auto add(size_t len) noexcept -> void {
		cur_ += len;
	}

	auto reset() noexcept -> void {
		cur_ = data_;
	}
	auto bzero() noexcept -> void {
		::bzero(data_, sizeof data_);
	}

	auto debugString() noexcept -> const char *;
	auto asString() const noexcept -> string {
		return string(data_, length());
	}
private:
	auto end() const noexcept -> const char * {
		return data_ + sizeof data_;
	}
	char data_[SIZE];
	char *cur_;
};

}  // namespace detail

class LogStream {
public:
	using Buffer = detail::FixedBuffer<detail::kSmallBuffer>;
	LogStream() = default;
	~LogStream() = default;

	LogStream(const LogStream&) = delete;
	auto operator=(const LogStream&) -> LogStream & = delete;

	LogStream &operator<<(bool v) {
		buffer_.append(v ? "1" : "0", 1);
		return *this;
	}

	LogStream &operator<<(short);
	LogStream &operator<<(unsigned short);
	LogStream &operator<<(int);
	LogStream &operator<<(unsigned int);
	LogStream &operator<<(long);
	LogStream &operator<<(unsigned long);
	LogStream &operator<<(long long);
	LogStream &operator<<(unsigned long long);

	LogStream &operator<<(const void *);

	LogStream &operator<<(float v) {
		*this << static_cast<double>(v);
		return *this;
	}
	LogStream &operator<<(double);

	LogStream &operator<<(char v) {
		buffer_.append(&v, 1);
		return *this;
	}
	LogStream &operator<<(const string &v) {
		buffer_.append(v.c_str(), v.size());
		return *this;
	}
	LogStream &operator<<(const char *v) {
//		std::cout << "log output const char *" << std::endl;
		buffer_.append(v, strlen(v));
		return *this;
	}

	auto append(const char *data, int len) -> void {
		buffer_.append(data, len);
	}
	auto buffer() -> const Buffer & {
		return buffer_;
	}
	auto resetBuffer() noexcept -> void {
		buffer_.reset();
	}
private:
	auto staticCheck() -> void;
	template<typename T>
	auto formatIntegar(T) -> void;

	Buffer buffer_;
	static constexpr int kMaxNumericSize = 32;
};

class Fmt {
public:
	template<typename T>
	Fmt(const char *fmt, T val);

	auto buf() const noexcept -> const char * {
		return buf_;
	}

	auto length() const noexcept -> int {
		return length_;
	}

private:
	char buf_[32];
	int length_;
};

inline auto operator<<(LogStream &s, const Fmt &fmt) -> LogStream & {
	s.append(fmt.buf(), fmt.length());
	return s;
}


} /* namespace cxwcfea */

#endif /* CXWCFEA_LOGSTREAM_H_ */

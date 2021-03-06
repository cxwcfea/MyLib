/*
 * FileUtil.h
 *
 *  Created on: Feb 18, 2014
 *      Author: xcheng
 */

#ifndef CXWCFEA_FILEUTIL_H_
#define CXWCFEA_FILEUTIL_H_

#include "Precompiled.h"
#include <fstream>

namespace cxwcfea {

class ReadSmallFile {
public:
	ReadSmallFile(string file_name);
	~ReadSmallFile();

	ReadSmallFile(const ReadSmallFile &) = delete;
	auto operator=(const ReadSmallFile &) -> ReadSmallFile & = delete;

	auto readToString(int maxSize, string *content, int64_t *fileSize = nullptr,
			int64_t *modifyTime = nullptr, int64_t *createTime = nullptr) noexcept -> int;

	auto readToBuffer(int *size) noexcept -> int;

	auto buffer() const noexcept -> const char * {
		return buf_;
	}

	static constexpr int kBufferSize = 64 * 1024;

private:
	int fd_;
	int err_;
	char buf_[kBufferSize];
};

auto readFile(string fileName, int maxSize, string *content, int64_t *fileSize = nullptr,
		int64_t *modifyTime = nullptr, int64_t *createTime = nullptr) noexcept -> int;

class AppendFile {
public:
	AppendFile(string &fileName);
	~AppendFile();

	AppendFile(const AppendFile &) = delete;
	auto operator=(const AppendFile &) -> AppendFile & = delete;

	auto append(const char *logline, const size_t len) -> void;

	auto flush() -> void;

	auto writtenBytes() const noexcept -> int {
		return writtenBytes_;
	}

private:
	std::ofstream fs_;
	int writtenBytes_;
};

} /* namespace cxwcfea */

#endif /* CXWCFEA_FILEUTIL_H_*/

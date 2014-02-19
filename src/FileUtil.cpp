/*
 * FileUtil.cpp
 *
 *  Created on: Feb 18, 2014
 *      Author: xcheng
 */

#include <fcntl.h>
#include <unistd.h>
#include <sys/uio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include "FileUtil.h"

namespace cxwcfea {

ReadSmallFile::ReadSmallFile(string file_name) :
		fd_(open(file_name.c_str(), O_RDONLY | O_CLOEXEC)), err_(0) {
	buf_[0] = '\0';
	if (fd_ == -1) {
		err_ = errno;
	}
}

ReadSmallFile::~ReadSmallFile() {
	if (fd_ > -1) {
		close(fd_);
	}
}

auto ReadSmallFile::readToString(int maxSize, string* content,
		int64_t* fileSize, int64_t* modifyTime, int64_t* createTime) noexcept -> int {
	static_assert(sizeof(off_t) == 8, "off_t not equal to 8 byte");
	ASSERT(content != nullptr);
	ASSERT(maxSize >= 0);

	int err = err_;

	if (fd_ > -1) {
		content->clear();
		if (fileSize) {
			struct stat state;
			if (fstat(fd_, &state) == 0) {
				if (S_ISREG(state.st_mode)) {
					*fileSize = state.st_size;
					content->reserve(static_cast<string::size_type>(std::min(static_cast<int64_t>(maxSize), *fileSize)));
				} else if (S_ISDIR(state.st_mode)) {
					err = EISDIR;
				}
				if (modifyTime) {
					*modifyTime = state.st_mtime;
				}
				if (createTime) {
					*createTime = state.st_ctime;
				}
			} else {
				err = errno;
			}
		}

		while (content->size() < maxSize) {
			size_t to_read = std::min(maxSize - content->size(), sizeof(buf_));
			ssize_t n = read(fd_, buf_, to_read);
			if (n > 0) {
				content->append(buf_, n);
			} else {
				if (n < 0) {
					err = errno;
				}
				break;
			}
		}
	}
	return err;
}

auto ReadSmallFile::readToBuffer(int *size) noexcept -> int {
	int err = err_;
	if (fd_ > -1) {
		ssize_t n = pread(fd_, buf_, sizeof(buf_) - 1, 0);
		if (n >= 0) {
			if (size) {
				*size = static_cast<int>(n);
			}
			buf_[n] = '\0';
		} else {
			err = errno;
		}
	}
	return err;
}

auto readFile(string fileName, int maxSize, string *content, int64_t *fileSize,
		int64_t *modifyTime, int64_t *createTime) noexcept -> int {
	ReadSmallFile file(fileName);
	return file.readToString(maxSize, content, fileSize, modifyTime, createTime);
}

} /* namespace cxwcfea */

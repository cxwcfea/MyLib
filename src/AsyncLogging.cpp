/*
 * AsyncLogging.cpp
 *
 *  Created on: Mar 10, 2014
 *      Author: xcheng
 */

#include "AsyncLogging.h"
#include "LogFile.h"
#include "Timestamp.h"

namespace cxwcfea {

AsyncLogging::AsyncLogging(const string &basename, size_t rollSize, int flushInterval) :
	flushInterval_(flushInterval),
	running_(false),
	basename_(basename),
	rollSize_(rollSize),
	thread_(std::bind(&AsyncLogging::threadFunc, this), "Logging"),
	latch_(1),
	mutex_(),
	condition_(mutex_),
	currentBuffer_(new Buffer),
	nextBuffer_(new Buffer),
	buffers_() {
	currentBuffer_->bzero();
	nextBuffer_->bzero();
	buffers_.reserve(16);
}

auto AsyncLogging::append(const char *logLine, int len) -> void {
	MutexLockGuard lock(mutex_);
	if (currentBuffer_->avail() >= len) {
		currentBuffer_->append(logLine, len);
	} else {
		buffers_.emplace_back(std::move(currentBuffer_));
		if (nextBuffer_) {
			currentBuffer_  = std::move(nextBuffer_);
		} else {
			currentBuffer_.reset(new Buffer);
		}
		currentBuffer_->append(logLine, len);
        condition_.notify();
	}
}

auto AsyncLogging::threadFunc() -> void {
	ASSERT(running_ == true);
	latch_.countDown();
	LogFile output { basename_, rollSize_, false };
	BufferPtr newBuffer1 = make_unique<Buffer>();
	BufferPtr newBuffer2 = make_unique<Buffer>();
	newBuffer1->bzero();
	newBuffer2->bzero();
	BufferVector buffersToWrite;
	buffersToWrite.reserve(16);
	while (running_) {
		ASSERT(newBuffer1 && newBuffer1->length() == 0);
		ASSERT(newBuffer2 && newBuffer2->length() == 0);
		ASSERT(buffersToWrite.empty());

		{
			MutexLockGuard lock(mutex_);
			if (buffers_.empty()) {
				condition_.waitForSeconds(flushInterval_);
			}
			buffers_.emplace_back(std::move(currentBuffer_));
			currentBuffer_ = std::move(newBuffer1);
			buffersToWrite.swap(buffers_);
			if (!nextBuffer_) {
				nextBuffer_ = std::move(newBuffer2);
			}
		}

		ASSERT(!buffersToWrite.empty());
		if (buffersToWrite.size() > 25) {
			char buf[256];
			snprintf(buf, sizeof buf, "Dropped log messages at %s, %zd larger buffers\n",
					Timestamp::now().toFormattedString().c_str(), buffersToWrite.size() - 2);
			fputs(buf, stderr);
			output.append(buf, static_cast<int>(strlen(buf)));
			buffersToWrite.erase(buffersToWrite.begin()+2, buffersToWrite.end());
		}
		std::for_each(begin(buffersToWrite), end(buffersToWrite), [&output](BufferPtr &buffer) {
			output.append(buffer->data(), buffer->length());
		});
		if (buffersToWrite.size() > 2) {
			buffersToWrite.resize(2);
		}
		if (!newBuffer1) {
			ASSERT(!buffersToWrite.empty());
			newBuffer1 = std::move(buffersToWrite.back());
//			buffersToWrite.back();
			newBuffer1.reset();
		}
		if (!newBuffer2) {
			ASSERT(!buffersToWrite.empty());
			newBuffer1 = std::move(buffersToWrite.back());
//			buffersToWrite.back();
			newBuffer1.reset();
		}
		buffersToWrite.clear();
		output.flush();
	}
	output.flush();
}

} /* namespace cxwcfea */

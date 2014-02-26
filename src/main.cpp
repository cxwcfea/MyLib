/*
 * main.cpp
 *
 *  Created on: Feb 17, 2014
 *      Author: xcheng
 */

#include "Precompiled.h"
#include "Timestamp.h"
#include "FileUtil.h"
#include "Date.h"
#include "Exception.h"
#include "Thread.h"

using namespace std;
using namespace cxwcfea;

class Incomplete;

struct MoveTest {
	int a;
public:
	MoveTest() = default;
	MoveTest(const MoveTest &) = delete;
	MoveTest &operator=(const MoveTest &) = delete;

	MoveTest(MoveTest &&other) {
		a = other.a;
	}
	MoveTest &operator=(MoveTest &&other) {
		a = other.a;
		return *this;
	}
};

auto badFunc() -> void {
	try {
		cout << "throw" << endl;
		throw Exception("test");
	} catch (const Exception &ex) {
		cout << "Exception:" << endl;
		cout << "what:" << ex.what() << endl;
		cout << "stack trace:" << ex.stackTrace() << endl;
	}
}

auto threadFunc() -> void {
	cout << "running in Thread:" << CurrentThread::t_tidString << endl;
	cout << "end" << endl;
}

int main(int argc, char **argv) {
	auto a = 9;
	TRACE("Start %d\n", a);
	auto b = make_unique<int>(8);
	TRACE("%d\n", *b);
	cout << Timestamp::now().toString() << " " << Timestamp::now().toFormattedString() << endl;
	string c;
	int64_t fileSize;
	int64_t modifyTime;
	if (int err = readFile("/Users/xcheng/hls_url.txt", 1024, &c, &fileSize, &modifyTime)) {
		TRACE("read file fail:%s\n", strerror(err));
	} else {
		cout << "file content:" << endl;
		cout << c << endl;
		cout << "size:" << fileSize << " modifyTime:" << Timestamp(modifyTime*Timestamp::kMicroSecondsPerSecond).toFormattedString() << endl;
	}
//	string fileName = "/Users/xcheng/test.txt";
//	AppendFile file_(fileName);
//	char buf[] = "ok, I am writing something";
//	file_.append(buf, sizeof(buf));
//	cout << "write " << file_.writtenBytes() << " bytes" << endl;
//	Date d { 1981, 8, 4 };
//	cout << d.toIsoString() << endl;

//	vector<Date> dateV;
//	cout << dateV.capacity() << endl;
//	dateV.push_back(Date());
//	cout << dateV.capacity() << endl;
//	dateV.push_back(Date());
//	cout << dateV.capacity() << endl;
//	Date d1;
//	Date d2 { 123 };
//	swap(d1, d2);
//	vector<Date> dateV { Date(), Date(), Date(), Date(), Date() };
	Date dateV[10];
	std::reverse(begin(dateV), end(dateV));
	MoveTest mt;
	MoveTest mt2(std::move(mt));
//	badFunc();
	Thread thread1(threadFunc, "thread1");
	thread1.start();
	thread1.join();
	cout << CurrentThread::t_tidString << " end" << endl;
}

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
#include "dirent.h"
#include "ProcessInfo.h"
#include "LogFile.h"
#include "Logging.h"

using namespace std;
using namespace cxwcfea;

class Incomplete;

class MyClass {
public:
	MyClass() {
		cout << "constructor" << endl;
	}
	~MyClass() {
		cout << "destructor" << endl;
	}
	MyClass(const MyClass &other) {
		cout << "copy constructor" << endl;
	}
	MyClass &operator=(const MyClass &other) {
		cout << "copy assignment" << endl;
		return *this;
	}
	MyClass(MyClass &&other) {
		cout << "move constructor" << endl;
	}
	MyClass &operator=(MyClass &&other) {
		cout << "move assignment" << endl;
		return *this;
	}
};

MyClass getMyClass(int value) {
	if (value == 1) {
		MyClass local = MyClass();
		return local;
	} else {
		MyClass local2 = MyClass();
		return local2;
	}
}

class noncopyable {
protected:
	noncopyable() {
	}
	;
private:
	noncopyable(const noncopyable &) = delete;
	void operator=(const noncopyable &) = delete;
};

class testnoncopyable: noncopyable {
public:
	testnoncopyable() = default;
	testnoncopyable(testnoncopyable &&other) {
	}
};

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

auto stringTestFunc(const char *name) -> void {
	cout << name << endl;
}

auto threadFunc() -> void {
	cout << "running in Thread:" << CurrentThread::t_tidString << endl;
	cout << "end" << endl;
}

template<int N>
auto templateTest(const char (&arr)[N]) -> void {
	cout << arr << endl;
}

enum LLevel {
	A,
	B,
	C,
	D,
};

int main(int argc, char **argv) {
	auto a = 9;
	TRACEOUT("Start %d\n", a);
	auto b = make_unique<int>(8);
	TRACEOUT("%d\n", *b);
	cout << Timestamp::now().toString() << " "
			<< Timestamp::now().toFormattedString() << endl;
	string c;
	int64_t fileSize;
	int64_t modifyTime;
	if (int err = readFile("/Users/xcheng/hls_url.txt", 1024, &c, &fileSize,
			&modifyTime)) {
		TRACEOUT("read file fail:%s\n", strerror(err));
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
//	DIR *dir = opendir("/Users/xcheng/Downloads");
//	struct dirent *de;
//	while ((de = readdir(dir)) != nullptr) {
//		cout << de->d_name << endl;
//	}
	string name1 = "abcd";
	stringTestFunc(name1.c_str());
	cout << "pid:" << ProcessInfo::pidString() << endl;
	cout << "uid:" << ProcessInfo::uid() << endl;
	cout << "username:" << ProcessInfo::userName() << endl;
	cout << "euid:" << ProcessInfo::euid() << endl;
	cout << "startTime:" << ProcessInfo::startTime().toFormattedString()
			<< endl;
	cout << "procName:" << ProcessInfo::procName() << endl;
	cout << "hostName:" << ProcessInfo::hostName() << endl;
	testnoncopyable na;
	testnoncopyable nb(move(na));
	LogFile lf("cxwcfea", 300);
	int kRPS = 24 * 60 * 60;
	time_t now = time(nullptr);
	time_t start = now / kRPS * kRPS;
	cout << "now:" << now << " \nstart:" << start << endl;
	MyClass inMain = getMyClass(1);
	templateTest("templatetest");
	LOG_INFO << "ok, this is a info";
}

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
#include "AsyncLogging.h"
#ifdef __MACH__
#include <libgen.h>  // basename()
#endif

using namespace std;
using namespace cxwcfea;

class Incomplete;

class MyClass {
	static int time;
	int a;
public:
	MyClass() {
		a = ++time;
		cout << "MyClass constructor " << a << endl;
	}
	~MyClass() {
		cout << "MyClass destructor " << a << endl;
	}
	MyClass(const MyClass &other) {
		a = ++time;
		cout << "MyClass copy constructor" << a << endl;
	}
	MyClass &operator=(const MyClass &other) {
		a = ++time;
		cout << "MyClass copy assignment" << a << endl;
		return *this;
	}
	MyClass(MyClass &&other) {
		a = other.a;
		cout << "MyClass move constructor" << a << endl;
	}
	MyClass &operator=(MyClass &&other) {
		a = other.a;
		cout << "MyClass move assignment" << a << endl;
		return *this;
	}
};

int MyClass::time = 0;

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
	//shared_ptr<char> buf;
public:
	MoveTest() : a(0) {
	}
	MoveTest(const MoveTest &) = delete;
	MoveTest &operator=(const MoveTest &) = delete;

	MoveTest(MoveTest &&other) {
		a = other.a;
	}
	MoveTest &operator=(MoveTest &&other) {
		a = other.a;
		return *this;
	}
	auto push(char c) -> void {
	//	buf[a++] = c;
	}
	auto print() -> void {
	//	for_each(begin(buf), end(buf), [](char e) { cout << e << ' '; });
	//	cout << endl;
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

AsyncLogging *g_asyncLog = nullptr;

auto asynOutput(const char *msg, int len) -> void {
	//cout << "asynOutput" << endl;
	g_asyncLog->append(msg, len);
}

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
	mt.push('a');
	mt.push('e');
	mt.push('c');
	mt.push('i');
	MoveTest mt2(std::move(mt));
	mt2.print();
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
	//LogFile lf("cxwcfea", 300);
	int kRPS = 24 * 60 * 60;
	time_t now = time(nullptr);
	time_t start = now / kRPS * kRPS;
	cout << "now:" << now << " \nstart:" << start << endl;
	MyClass inMain = getMyClass(1);
	templateTest("templatetest");
	LOG_INFO << "ok, this is a info";
	unique_ptr<MyClass> umy1 = make_unique<MyClass>();
	if (umy1) {
		cout << "create umy1 sucess" << endl;
	}
	unique_ptr<MyClass> umy2 = move(umy1);
	if (umy1) {
		cout << "umy1 still not null" << endl;
	} else {
		cout << "umy1 is null" << endl;
	}
	if (umy2) {
		cout << "umy2 is not null" << endl;
	}
	umy2.reset();
	if (umy2) {
		cout << "after reset umy2 still not null" << endl;
	} else {
		cout << "after reset umy2 is null" << endl;
	}
	std::vector<char> letters {'o', 'm', 'g', 'w', 't', 'f'};

    if (!letters.empty()) {
        std::cout << "The last character is: " << letters.back() << '\n';
    }
    if (!letters.empty()) {
        std::cout << "The last character is: " << letters.back() << '\n';
    }
    char filename[256];
    strncpy(filename, argv[0], 256);
    cxwcfea::AsyncLogging logger { basename(filename), 500*1000*1000, };
    logger.start();
    g_asyncLog = &logger;
    Logger::setOutput(asynOutput);
    cout << "loglevel:" << Logger::logLevel() << endl;
    int cnt = 0;
    string empty = " ";
	for (int i = 0; i < 1000; ++i) {
		LOG_INFO << "Hello 0123456789" << " abcdefghijklmnopqrstuvwxyz "
				<< empty << cnt;
		++cnt;
	}
	cout << "end" << endl;
}

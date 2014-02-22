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

using namespace std;
using namespace cxwcfea;

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
}

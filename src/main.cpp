/*
 * main.cpp
 *
 *  Created on: Feb 17, 2014
 *      Author: xcheng
 */

#include "Precompiled.h"
#include "Timestamp.h"
#include "FileUtil.h"

using namespace std;
using namespace cxwcfea;

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
		TRACE("read file fail:%s", strerror(err));
	} else {
		cout << "file content:" << endl;
		cout << c << endl;
		cout << "size:" << fileSize << " modifyTime:" << Timestamp(modifyTime*Timestamp::kMicroSecondsPerSecond).toFormattedString() << endl;
	}
}




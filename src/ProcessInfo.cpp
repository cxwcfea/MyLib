/*
 * ProcessInfo.cpp
 *
 *  Created on: Feb 26, 2014
 *      Author: xcheng
 */

#include <cctype>
#include <cstdlib>
#include <sys/types.h>
#include <sys/resource.h>
#include <dirent.h>
#include <unistd.h>
#include <pwd.h>
#include "ProcessInfo.h"
#include "FileUtil.h"

namespace cxwcfea {

namespace detail {

__thread int t_numOpenedFiles;
auto fdDirFilter = [](const struct dirent *d) -> int {
	if (::isdigit(d->d_name[0])) {
		++t_numOpenedFiles;
	}
	return 0;
};

__thread std::vector<pid_t> *t_pids = nullptr;
auto taskDirFilter = [](const struct dirent *d) -> int {
	if (::isdigit(d->d_name[0])) {
		t_pids->emplace_back(atoi(d->d_name));
	}
	return 0;
};

auto scanDir(const char *dirpath, int (*filter)(const struct dirent *)) -> int {
	struct dirent **namelist = nullptr;
	int result = ::scandir(dirpath, &namelist, filter, alphasort);
	ASSERT(namelist == nullptr);
	return result;
}

Timestamp g_startTime = Timestamp::now();

}  // namespace detail

}  // namespace cxwcfea

using namespace std;
using namespace cxwcfea;
using namespace cxwcfea::detail;

auto ProcessInfo::pid() -> pid_t {
	return ::getpid();
}

auto ProcessInfo::pidString() -> string {
	char buf[32];
	snprintf(buf, sizeof buf, "%d", pid());
	return buf;
}

auto ProcessInfo::uid() -> uid_t {
	return ::getuid();
}

auto ProcessInfo::userName() -> string {
	struct passwd pwd;
	struct passwd *result = nullptr;
	char buf[8192];
	const char *name = "unknownuser";

	::getpwuid_r(uid(), &pwd, buf, sizeof buf, &result);
	if (result) {
		name = pwd.pw_name;
	}
	return name;
}

auto ProcessInfo::euid() -> uid_t {
	return ::geteuid();
}

auto ProcessInfo::startTime() -> Timestamp {
	return g_startTime;
}

auto ProcessInfo::hostName() -> string {
	char buf[64] = "unknownhostname";
	buf[sizeof(buf) - 1] = '\0';
	::gethostname(buf, sizeof buf);
	return buf;
}

auto ProcessInfo::procName() -> string {
	string name;
	string stat = procStat();
	size_t lp = stat.find('(');
	size_t rp = stat.find(')');
	if (lp != string::npos && rp != string::npos) {
		name = stat.substr(lp + 1, rp - lp - 1);
	}
	return name;
}

auto ProcessInfo::procStatus() -> string {
	string result;
	readFile("/proc/self/status", 65536, &result);
	return result;
}

auto ProcessInfo::procStat() -> string {
	string result;
	readFile("/proc/self/stat", 65536, &result);
	return result;
}

auto ProcessInfo::exePath() -> string {
	string result;
	char buf[1024];
	ssize_t n = ::readlink("/proc/self/exe", buf, sizeof buf);
	if (n > 0) {
		result.assign(buf, n);
	}
	return result;
}

auto ProcessInfo::openedFiles() -> int {
	t_numOpenedFiles = 0;
	scanDir("/proc/self/fd", fdDirFilter);
	return t_numOpenedFiles;
}

auto ProcessInfo::maxOpenFiles() -> int {
	struct rlimit rl;
	if (::getrlimit(RLIMIT_NOFILE, &rl)) {
		return openedFiles();
	} else {
		return static_cast<int>(rl.rlim_cur);
	}
}

auto ProcessInfo::numThreads() -> int {
	int result = 0;
	string status = procStatus();
	size_t pos = status.find("Threads:");
	if (pos != string::npos) {
		result = ::atoi(status.c_str() + pos + 8);
	}
	return result;
}

auto ProcessInfo::threads() -> vector<pid_t> {
	std::vector<pid_t> result;
	t_pids = &result;
	scanDir("/proc/self/task", taskDirFilter);
	t_pids = nullptr;
	std::sort(begin(result), end(result));
	return result;
}


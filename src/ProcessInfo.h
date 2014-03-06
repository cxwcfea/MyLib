/*
 * ProcessInfo.h
 *
 *  Created on: Feb 26, 2014
 *      Author: xcheng
 */

#ifndef CXWCFEA_PROCESSINFO_H_
#define CXWCFEA_PROCESSINFO_H_

#include "Precompiled.h"
#include "Timestamp.h"

namespace cxwcfea {

namespace ProcessInfo {

auto pid() -> pid_t;
auto pidString() -> string;
auto uid() -> uid_t;
auto userName() -> string;
auto euid() -> uid_t;
auto startTime() -> Timestamp;

auto hostName() -> string;
auto procName() -> string;
auto procStatus() -> string;
auto procStat() -> string;
auto exePath() -> string;

auto openedFiles() -> int;
auto maxOpenFiles() -> int;

auto numThreads() -> int;
auto threads() -> std::vector<pid_t>;

}  // namespace ProcessInfo

}  // namespace cxwcfea

#endif /* PROCESSINFO_H_ */

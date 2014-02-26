/*
 * Singleton.h
 *
 *  Created on: Feb 26, 2014
 *      Author: xcheng
 */

#ifndef CXWCFEA_SINGLETON_H_
#define CXWCFEA_SINGLETON_H_

#include <pthread.h>
#include <stdlib.h>

namespace cxwcfea {
template<class T>
class Singleton {
public:
	static auto getInstance() -> T & {
		pthread_once(&ponce_, &Singleton::init);
		return *value_;
	}
private:
	Singleton();
	~Singleton();

	Singleton(const Singleton &) = delete;
	auto operator=(const Singleton &) -> Singleton & = delete;

	static auto init() -> void {
		value_ = new T();
		atexit(destroy);
	}

	static auto destroy() -> void {
		typedef char T_must_be_complete_type[sizeof(T) == 0 ? -1 : 1];
		T_must_be_complete_type dummy;
		(void) dummy;

		delete value_;
	}

	static pthread_once_t ponce_;
	static T *value_;
};

template<class T>
pthread_once_t Singleton<T>::ponce_ = PTHREAD_ONCE_INIT;

template<class T>
T *Singleton<T>::value_ = nullptr;

} /* namespace cxwcfea */

#endif /* CXWCFEA_SINGLETON_H_ */

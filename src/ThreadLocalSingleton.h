/*
 * ThreadLocalSingleton.h
 *
 *  Created on: Mar 13, 2014
 *      Author: xcheng
 */

#ifndef CXWCFEA_THREADLOCALSINGLETON_H_
#define CXWCFEA_THREADLOCALSINGLETON_H_

#include <pthread.h>
#include "Precompiled.h"

template<typename T>
class ThreadLocalSingleton {
public:
	static T &instance() {
		if (!value_) {
			value_ = new T();
			deleter_.set(value_);
		}
		return *value_;
	}

	static T *pointer() noexcept {
		return value_;
	}

private:
	ThreadLocalSingleton() = delete;
	~ThreadLocalSingleton() = delete;

	static void destructor(void *obj) {
		ASSERT(obj == value_);
        typedef char T_must_be_complete_type[sizeof(T) == 0 ? -1 : 1];
        T_must_be_complete_type dummy; (void) dummy;
        delete obj;
        value_ = 0;
	}

	class Deleter {
	public:
		Deleter() {
			pthread_key_create(&pkey_, &ThreadLocalSingleton::destructor);
		}

		~Deleter() {
			pthread_key_delete(pkey_);
		}

		void set(T *value) {
			ASSERT(pthread_getspecific(pkey_) == nullptr);
			pthread_setspecific(pkey_, value);
		}

		pthread_key_t pkey_;
	};


	static __thread T *value_;
	static Deleter deleter_;
};

template<typename T>
__thread T *ThreadLocalSingleton<T>::value_ = 0;

template<typename T>
typename ThreadLocalSingleton<T>::Deleter ThreadLocalSingleton<T>::deleter_;

#endif /* CXWCFEA_THREADLOCALSINGLETON_H_ */

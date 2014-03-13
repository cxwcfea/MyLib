/*
 * ThreadLocal.h
 *
 *  Created on: Mar 12, 2014
 *      Author: xcheng
 */

#ifndef CXWCFEA_THREADLOCAL_H_
#define CXWCFEA_THREADLOCAL_H_

#include <pthread.h>

namespace cxwcfea {

template<typename T>
class ThreadLocal {
public:
	ThreadLocal() {
		pthread_key_create(&pkey_, &ThreadLocal::destructor);
	}
	~ThreadLocal() {
		pthread_key_delete(pkey_);
	}

	auto value() -> T& {
		T* perThreadValue = static_cast<T*>(pthread_getspecific(pkey_));
		if (!perThreadValue) {
			T *newObject = new T();
			pthread_setspecific(pkey_, newObject);
			perThreadValue = newObject;
		}
		return *perThreadValue;
	}

private:

	static auto destructor(void *x) -> void {
		T* obj = static_cast<T*>(x);
		typedef char T_must_be_complete_type[sizeof(T) == 0 ? -1 : 1];
		T_must_be_complete_type dummy; (void) dummy;
		delete obj;
	}

	pthread_key_t pkey_;
};

} /* namespace cxwcfea */

#endif /* CXWCFEA_THREADLOCAL_H_ */

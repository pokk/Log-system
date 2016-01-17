/*
 * common.c
 *
 *  Created on: 2013/10/15
 *      Author: Jieyi
 */

#include <stdio.h>
#include <pthread.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>

#include "../../header/Common/common.h"
#include "../../header/common_header.h"

// ==========   open function   ==========
// non-busy waiting delay
inline int delay(const int sec, const int micro_sec)
{
    struct timeval delay;

    // set delay time
    delay.tv_sec = sec;
    delay.tv_usec = micro_sec;

    // delay
    int res = select(0, NULL, NULL, NULL, &delay);

    return res;
}

// create a thread function
int create_thread(void **func_pointer, void *arg)
{
	AVER_ENTER;

	pthread_t thread;

	// create a thread service
	pthread_create(&thread, NULL, func_pointer, arg);
	// set thread attribute is detach
	pthread_detach(thread);

	AVER_LEAVE;
	return SUCCESS;
}

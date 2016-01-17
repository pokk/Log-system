/*
 * common.h
 *
 *  Created on: 2013/10/15
 *      Author: Jieyi
 */

inline int delay(const int sec, const int micro_sec);			// non-busy waiting function
int create_thread(void **func_pointer, void *arg);				// create a thread function

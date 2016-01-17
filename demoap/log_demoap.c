/*
 * log_demoap.c
 *
 *  Created on: 2013/10/7
 *      Author: Jieyi
 */

#include <pthread.h>

#include "../header/avt_log.h"
#include "../header/common_header.h"

#define PROC_TAG "MY-PROCESS"

void *log_thread_one(void *arg)
{
	int i = 0;

	for (i = 0; i < 5000; i++)
	{
		AVT_log(LOG_LEVEL_DEBUG, "THREAD1", "every body go %d~~~~", i);
		delay(0, 100);
	}
}

void *log_thread_two(void *arg)
{
	int i = -10;

	for (i = -10; i < 4000; i++)
	{
		AVT_log(LOG_LEVEL_INFO, "222THREAD222", "so good bad boy %d~~~~", i);
		delay(0, 50);
	}
}

void sequence_log()
{
	int i = 0;
	//	avt_set_log_output_format_setting(LOG_ATTR_IGN_TAG | LOG_ATTR_IGN_FUN_NAME);
	//	avt_set_log_output_format_setting(LOG_ATTR_IGN_DATE | LOG_ATTR_IGN_PID);

	for (i = 0; i < 10; i++)
	{
		AVT_log(LOG_LEVEL_ERROR, PROC_TAG, "test word : %d", i);
		delay(0, 10);
	}

	log("finish...........");
}

void competition_log()
{
	void *ret1 = NULL, *ret2 = NULL;
	// declare 2 thread, one is push data in the queue; the other is pop data from the queue
	pthread_t t_enque, t_deque;

//	avt_set_log_output_format_setting(LOG_ATTR_IGN_TAG | LOG_ATTR_IGN_FUN_NAME);
	avt_set_log_level(LOG_LEVEL_WARNING);

	pthread_create(&t_enque, NULL, &log_thread_one, NULL);
	pthread_create(&t_deque, NULL, &log_thread_two, NULL);

	pthread_join(t_enque, &ret1);
	pthread_join(t_deque, &ret2);

	log("return value from t_enque = %s", (char *)ret1);
	log("return value from t_deque = %s", (char *)ret2);
}

void filter_log()
{
	avt_set_filter(FILTER_OP_DROP, "A;B;C;D;E;F;D");

	AVT_log(LOG_LEVEL_ERROR, "a", "test word : gfds");
	AVT_log(LOG_LEVEL_ERROR, "B", "test word : gfsdsss");
	AVT_log(LOG_LEVEL_ERROR, "c", "test word : vvvv");
	AVT_log(LOG_LEVEL_ERROR, "D", "test word : h32f34");
	AVT_log(LOG_LEVEL_ERROR, "e", "test word : f4qfd");
	AVT_log(LOG_LEVEL_ERROR, "F", "test word : bgfbgsf");
	AVT_log(LOG_LEVEL_ERROR, "G", "test word : gfgsd");
	AVT_log(LOG_LEVEL_ERROR, "a", "test word : gfdsgfdsfgs");
	AVT_log(LOG_LEVEL_ERROR, "b", "test word : f43q2fihvffvffffff");
	AVT_log(LOG_LEVEL_ERROR, "C", "test word : fegb241f1");
	AVT_log(LOG_LEVEL_ERROR, "d", "test word : 134f134");
	AVT_log(LOG_LEVEL_ERROR, "E", "test word : 4333f4444");
	AVT_log(LOG_LEVEL_ERROR, "f", "test word : f423f24");
	AVT_log(LOG_LEVEL_ERROR, "g", "test word : gfa9f43f");

}


// =========================== main demo ===========================
void log_demoap()
{
//	sequence_log();
	filter_log();
//	competition_log();
	sleep(5);
}

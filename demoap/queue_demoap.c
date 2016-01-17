/*
 * queue_demoap.c
 *
 *  Created on: 2013/10/7
 *      Author: Jieyi
 */

#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>

#include "../header/queue_model.h"
#include "../header/common_header.h"

static void normal_test();
static void *push_queue_thread(void *arg);
static void *pop_quque_thread(void *arg);
static void competition_test();

m_q_data g_tmp_data;

// normal processes, those just are initial, enqueue, dequeue and uninitial
static void normal_test()
{
	msg_queue *q = NULL;
	long int max_size = 20, i = 0;
	char *data = NULL, msg[50] = {0};

	memset(msg, 0 , sizeof(msg));
	memset(&g_tmp_data, 0, sizeof(m_q_data));

	// initial
	init_queue(&q, max_size);

	// push in queue
	for (i = 0; i < max_size + 3; i++)
	{
		sprintf(msg, "ABCD  %ld", i);

		g_tmp_data.fun_name = "normal_test";
		g_tmp_data.level = LOG_LEVEL_WARNING;
		g_tmp_data.line = __LINE__;
		g_tmp_data.tag = "((GG-TAG))";
		g_tmp_data.time = time(NULL);
		g_tmp_data.msg = msg;

		op_enqueue(q, &g_tmp_data);
	}

	// pop from queue
	for (i = 0; i < 5; i++)
	{
		if ((data = op_dequeue(q)) == NULL)
			continue;

		log("data : %s", data);
	}

	// uninitial
	uninit_queue(&q);
}

// competition process, those are initial, uninitial and thread competition enqueue and dequeue
static void competition_test()
{
	msg_queue *q = NULL;
	void *ret1 = NULL, *ret2 = NULL;
	int max_size = 2000;
	// declare 2 thread, one is push data in the queue; the other is pop data from the queue
	pthread_t t_enque, t_deque;

	// initial
	init_queue(&q, max_size);

	pthread_create(&t_enque, NULL, &push_queue_thread, q);
	pthread_create(&t_deque, NULL, &pop_quque_thread, q);

	pthread_join(t_enque, &ret1);
	pthread_join(t_deque, &ret2);

	log("return value from t_enque = %s", (char *)ret1);
	log("return value from t_deque = %s", (char *)ret2);

	// uninitial
	uninit_queue(&q);
}

static void *push_queue_thread(void *arg)
{
	msg_queue *q = (msg_queue *)arg;
	int i = 0, max = 5000;
	char str[50] = {0};

	for (i = 0; i < max; i++)
	{
		memset(str, 0, sizeof(str));

		snprintf(str, sizeof(str), "String : %d", i);

		g_tmp_data.fun_name = "normal_test";
		g_tmp_data.level = LOG_LEVEL_WARNING;
		g_tmp_data.line = __LINE__;
		g_tmp_data.tag = "((GG-TAG))";
		g_tmp_data.time = time(NULL);
		g_tmp_data.msg = str;

		op_enqueue(q, &g_tmp_data);
	}

	pthread_exit((void *)"push finish");
}

static void *pop_quque_thread(void *arg)
{
	msg_queue *q = (msg_queue *)arg;
	int i = 0, max = 5000;

	for (i = 0; i < max; i++)
	{
		op_dequeue(q);
	}

	pthread_exit((void *)"pop finish");
}

// =========================== main demo ===========================
void queue_demoap()
{
	normal_test();

//	competition_test();
}

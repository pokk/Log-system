/*
 * avt_log.c
 *
 *  Created on: 2013/10/2
 *      Author: Jieyi
 */

#include <stdio.h>
#include <stdbool.h>
#include <stdarg.h>
#include <string.h>
#include <pthread.h>
#include <time.h>

#include "../header/queue_model.h"
#include "../header/avt_log.h"
#include "../header/common_header.h"

// ==========   define   ==========

// ==========   static global variable   ==========
static pthread_mutex_t g_msg_queue_mutex = PTHREAD_MUTEX_INITIALIZER; // first step, protect to push log to queue
// Queue
static msg_queue *g_msg_queue = NULL;					// create message queue to store message

// to judge the log queue flag
static bool g_init_fir_sec_thread_flag = false;			// only initial once flag

// ==========   open function   ==========
void avt_set_file_setting(avt_file_setting setting)
{
	AVER_ENTER;

	set_file_log_setting(setting);

	AVER_LEAVE;
}

void avt_set_udp_network(avt_network_setting setting)
{
	AVER_ENTER;
	// TODO : 設定 UDP 網路
	AVER_LEAVE;
}

void avt_set_output_setting(int io_attr_flag)
{
	AVER_ENTER;

	set_log_device_setting(io_attr_flag);

	AVER_LEAVE;
}

void avt_set_log_output_format_setting(int log_attr_flag)
{
	AVER_ENTER;

	set_log_output_setting(log_attr_flag);

	AVER_LEAVE;
}

void avt_set_log_level(avt_log_level level)
{
	AVER_ENTER;

	set_log_level(level);

	AVER_LEAVE;
}

void avt_set_filter(avt_filter_op operate, const char *tag)
{
	AVER_ENTER;

	set_tag_filter(operate, tag);

	AVER_LEAVE;
}

void avt_set_log_queue_size(avt_queue_op operate, int queue_size)
{
	AVER_ENTER;

	// check the global queue was created, if it was not created, create it now, set queue size
	if (!g_msg_queue)
		init_queue(&g_msg_queue, queue_size);

	// setting dropping message mechanism
	op_set_drop_mech(g_msg_queue, operate);

	AVER_LEAVE;
}

void avt_log(avt_log_level level, char *fun_name, int line, char *tag, char *format, ...)
{
	AVER_ENTER;

	pthread_mutex_lock(&g_msg_queue_mutex); // lock mutex flag for message queue

	// first queue initial
	que_manager_init(&g_msg_queue);

	// declare ap for storing the log
	// declare log_data for
	// declare tmp_log for
	va_list ap;
	m_q_data log_data;
	char tmp_log[512] = {'/0',};

	// initial
	memset(tmp_log, 0, 512);
	memset(&log_data, 0, sizeof(m_q_data));

	// get the log from container
	va_start(ap, format);
	vsnprintf(tmp_log, sizeof(tmp_log), format, ap);
	va_end(ap);

	// assign the print log information
	log_data.time = time(NULL);
	log_data.level = level;
	log_data.fun_name = fun_name;
	log_data.tag = tag;
	log_data.line = line;
	log_data.msg = tmp_log;

	// put the logs in queue
	op_enqueue(g_msg_queue, &log_data);

	// start the log_selector process, check the log agent
	if (!g_init_fir_sec_thread_flag)
	{
		// start the selector service to combine and filter the condition
		create_log_selector_service(g_msg_queue);

		// start the output service to get the second queue and distribute the output device
		create_log_destribute_service();

		// change the flag, it already started
		g_init_fir_sec_thread_flag = true;
	}

	pthread_mutex_unlock(&g_msg_queue_mutex); // lock mutex flag for message queue

	AVER_LEAVE;
}

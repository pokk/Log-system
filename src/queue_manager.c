/*
 * queue_manager.c
 *
 *  Created on: 2013/10/11
 *      Author: Jieyi
 */

#include <pthread.h>
#include <unistd.h>

#include "../header/queue_model.h"
#include "../header/common_header.h"

// ==========   define   ==========
// queue setting default value
#define DEFAULT_QUEUE_SIZE			5000				// default queue size
// IO output setting default value
#define DEFAULT_IO_ATTR_FLAG		0x00000				// default IO attribute

// ==========   static global variable   ==========
//static int g_io_attr_flag = DEFAULT_IO_ATTR_FLAG;		// save log attribute setting and initialized
static int g_io_attr_flag = LOG_ATTR_IGN_DATE;  // 測試用的啦
// Queue
static msg_queue *g_output_queue = NULL;				// create message queue to store message

// ==========   open function prototype   ==========
int que_manager_init(msg_queue **queue);
void create_log_selector_service(msg_queue *msg_queue);
void create_log_destribute_service();
// 測試用的啦

// ==========   static function prototype   ==========
static void *log_selector_service(void *arg);
static void *log_distribute_service(void *arg);
static bool chk_file_setting(int attribute_name);

// ==========   open function   ==========
// user setting "or" default setting
void set_log_device_setting(int io_attr_flag)
{
	AVER_ENTER;

	g_io_attr_flag |= io_attr_flag;

	AVER_LEAVE;
}

// for initial queue function
int que_manager_init(msg_queue **queue)
{
	AVER_ENTER;

	int ret = BEEN_INIT;

	// check the global queue was created, if it was not created, create it now, set queue size
	if (!(*queue))
	{
		// do initial queue
		init_queue(queue, DEFAULT_QUEUE_SIZE);
		ret = INIT;
	}

	AVER_LEAVE;
	return ret;
}

// start the log selector service by thread
void create_log_selector_service(msg_queue *msg_queue)
{
	AVER_ENTER;

	// second queue initial
	que_manager_init(&g_output_queue);
//	init_queue(&g_output_queue, 100000);    // ------- test code, protect the second queue don't be overflow

	// create a thread service
	create_thread(&log_selector_service, msg_queue);
	log("start selector log service thread...");

	AVER_LEAVE;
}

// start the log selector service by thread
void create_log_destribute_service()
{
	AVER_ENTER;

	// create a thread service
	create_thread(&log_distribute_service, NULL);
	log("start distribute log service thread...");

	AVER_LEAVE;
}


// ==========   static function prototype   ==========
// do the log selector service to do something and flow, this is thread function
static void *log_selector_service(void *arg)
{
	AVER_ENTER;

	// casting the parameter to message queue
	msg_queue *first_q = (msg_queue *)arg;
	int result = NOT_NULL_POTINER;
	// 測試用的啦
	// always get message from the first queue
	while (1)
	{
		// if the output queue was already null, and rest a 1 second
		if ((result = log_combinding(first_q, g_output_queue)) == NULL_POINTER)
			delay(1, 0);
		// normal get the message from queue, the speeding is about 30 microseconds
		delay(0, 30);
	}

	AVER_LEAVE;
	return NULL;
}

// get the data from the output queue and assign the output device, this is thread function
static void *log_distribute_service(void *arg)
{
	AVER_ENTER;

	m_q_data *output_data = NULL;

	// always get the log data from the output(the second) queue
	while (1)
	{
		// get the log data from the queue
		if ((output_data = op_dequeue(g_output_queue)) == NULL)
			delay(1, 0);
		else
		{
			// check all option
			if (g_io_attr_flag != IO_ATTR_DEFA)
			{
				// print to the monitor
				if (chk_file_setting(IO_ATTR_CONS))
					printf("哥印的啦 -> %s\n", output_data->msg);

				// write to the file
				if (chk_file_setting(IO_ATTR_FILE))
					file_management_distribute(output_data);

				// send to the remote server by UDP multi-cast
				if (chk_file_setting(IO_ATTR_UDP_MULTICAST))
				{
					// TODO : 分配進去UDP群播的地方
				}
				// send to the remote server by UDP
				if (chk_file_setting(IO_ATTR_UDP))
				{
					// TODO : 分配進去的地方
				}
			}

			// normal get the message from queue, the speeding is about 30 microseconds
			delay(0, 30);
		}
	}

	AVER_LEAVE;
	return NULL;
}

// decode file setting attribute
static bool chk_file_setting(int attribute_name)
{
	AVER_ENTER;

	// decode attribute flag
	if ((g_io_attr_flag & attribute_name) == attribute_name)
	{
		AVER_LEAVE;
		return true;
	}

	AVER_LEAVE;
	return false;
}

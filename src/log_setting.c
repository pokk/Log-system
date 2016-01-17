/*
 * log_setting.c
 *
 *  Created on: 2013/10/2
 *      Author: Jieyi
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <stdarg.h>
#include <unistd.h>
#include <time.h>

#include "../header/avt_log.h"
#include "../header/queue_model.h"
#include "../header/common_header.h"

// ==========   define   ==========
// log setting default value
#define LOG_SIZE_HEADER				128					// maximum header log size
#define LOG_SIZE_CONTENT			512					// maximum content log size

#define DEFAULT_LOG_ATTR_FLAG		0x00000				// default log output attribute


// ==========   static global variable   ==========
// default flag
static int g_log_attr_flag = DEFAULT_LOG_ATTR_FLAG;	// save log output format setting and initial


// ==========   open function prototype   ==========
int set_log_output_setting(const int log_attr_flag);
int log_combinding(msg_queue *log_queue, msg_queue *output_queue);


// ==========   static function prototype   ==========
static bool chk_log_setting(const int attribute_name);
static void log_format_add_date(const char *log_str, const time_t t);
static void log_format_add_tag(const char *log_str, const char *tag);
static void log_format_add_proc_pid(const char *log_str);
static void log_format_add_fun_name(const char *log_str, const char *fun_name, const int line);
static void log_format_write(avt_log_level level, const char *fun_name, const time_t t, const int line, const char *tag, const char *log_str);


// ==========   open function   ==========
// set log output format, there are "time", "PID" ... of the attributes.
int set_log_output_setting(const int log_attr_flag)
{
	AVER_ENTER;

	// special case is input LOG_ATTR_DEFA
	if (log_attr_flag == LOG_ATTR_DEFA)
	{
		// user setting is LOG_ATTR_DEFA, then setting change to LOG_ATTR_DEFA
		g_log_attr_flag = log_attr_flag;
	}
	else
	{
		// user setting "or" default setting
		g_log_attr_flag |= log_attr_flag;
	}

	AVER_LEAVE;
	return SUCCESS;
}

// print log to output device.
int log_combinding(msg_queue *log_queue, msg_queue *output_queue)
{
	AVER_ENTER;

	// declare ret is return code
	// declare log_head save log header
	// declare log_context save log header and user log
	// declare log_data is first queue
	int ret = NOT_NULL_POTINER;
	char log_head[LOG_SIZE_HEADER], log_context[LOG_SIZE_CONTENT];
	m_q_data *log_data = NULL;

	// initial variable
	memset(log_head, 0, sizeof(log_head));
	memset(log_context, 0, sizeof(log_context));

	do
	{
		// get the data from the log queue
		if ((log_data = op_dequeue(log_queue)) == NULL)
		{
			// the queue is null, can not get data anymore
			ret = NULL_POINTER;
			break;
		}
		// check the log whether it is filtered or not
		if (filter_log_level(log_data->level))
		{
			log("log filter lo~~~~~~~~~~~~~~~~~because level filter");
			// the log message will be filtered
			break;
		}
		// check the tag whether it is filtered or not
		if (filter_tag(log_data->tag))
		{
			log("log filter lo~~~~~~~~~~~~~~~~~because tag filter");
			// the log message will be filtered
			break;
		}

		// decide to what information write to log header
		log_format_write(log_data->level, log_data->fun_name, log_data->time, log_data->line, log_data->tag, log_head);

		// combine the header and the container to the context array
		snprintf(log_context, sizeof(log_context) - 1,"%s %s", log_head, log_data->msg);

		// 測試用的啦
//		log("log : %s", log_context);

		// assign the pointer to the other queue
		snprintf(log_data->msg, sizeof(log_context), "%s", log_context);

		// push the data in the output queue
		op_enqueue(output_queue, log_data);
	} while(0);

	AVER_LEAVE;
	return ret;
}


// ==========   static function   ==========
// decode log setting attribute.
static bool chk_log_setting(int attribute_name)
{
	AVER_ENTER;

	// decode attribute flag
	if ((g_log_attr_flag & attribute_name) == attribute_name)
	{
		AVER_LEAVE;
		return true;
	}

	AVER_LEAVE;
	return false;
}

// add date information.
static void log_format_add_date(const char *log_str, const time_t t)
{
	AVER_ENTER;

	struct tm *local;

	// get local time from unix time
	local = localtime(&t);

	// write date information in the log
	sprintf(log_str, "[%d/%d/%d %d:%d:%d]", local->tm_year + 1900, local->tm_mon + 1, local->tm_mday, local->tm_hour, local->tm_min, local->tm_sec);

	AVER_LEAVE;
}

// add tag information.
static void log_format_add_tag(const char *log_str, const char *tag)
{
	AVER_ENTER;

	// write date information in the log
	sprintf(log_str, "%s[%s]", log_str, tag);

	AVER_LEAVE;
}

// add process PID information.
static void log_format_add_proc_pid(const char *log_str)
{
	AVER_ENTER;

	// write PID information in the log
	sprintf(log_str, "%s(%d)", log_str, getpid());

	AVER_LEAVE;
}

// add function name information.
static void log_format_add_fun_name(const char *log_str, const char *fun_name, const int line)
{
	AVER_ENTER;

	// write function name and line information in the log
	sprintf(log_str, "%s[%s:%d]", log_str, fun_name, line);

	AVER_LEAVE;
}

// decide to log information which should write to the log.
static void log_format_write(avt_log_level level, const char *fun_name, const time_t t, const int line, const char *tag, const char *log_str)
{
	AVER_ENTER;

	// initial variable
	memset(log_str, 0, sizeof(log_str));

	// check ignore information
	if (!chk_log_setting(LOG_ATTR_IGN_DATE))			// date information
	{
		// add date information in the log
		log_format_add_date(log_str, t);
	}
	if (!chk_log_setting(LOG_ATTR_IGN_TAG))			// tag information
	{
		// add tag information in the log
		log_format_add_tag(log_str, tag);
	}
	if (!chk_log_setting(LOG_ATTR_IGN_PID))			// process PID information
	{
		// add process PID information in the log
		log_format_add_proc_pid(log_str);
	}
	if (!chk_log_setting(LOG_ATTR_IGN_FUN_NAME))		// function name information
	{
		// add function name information in the log
		log_format_add_fun_name(log_str, fun_name, line);
	}

	AVER_LEAVE;
}


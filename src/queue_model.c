/*
 * queue_model.c
 *
 *  Created on: 2013/10/4
 *      Author: Jieyi
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>

#include "../header/queue_model.h"
#include "../header/common_header.h"

// ==========   define   ==========
static const int G_MSG_SIZE = 512;		// maximum content log size
static const int G_FUN_NAME_SIZE = 50;  // maximum function name size
static const int G_TAG_SIZE = 25;		// maximum tag size

#define MAX_SIZEOF(buff, max_size) ((sizeof(buff) / sizeof(char)) <= max_size) ? strlen(buff) : max_size

// ==========   structure prototype   ==========
typedef struct m_queue_info	// all of the information about the queue
{
	pthread_mutex_t mutex;		// queue's synchronize flag
	avt_queue_op drop_mech;		// queue drop mechanism
	long int queue_size;		// queue maximum size
	int front;					// queue's head
	int rear;					// queue's tail
} m_q_info;
typedef struct message_queue	// split the data and the information structure
{
	m_q_data *q;				// the queue's data
	m_q_info info;				// the queue's information
} msg_queue;

// ==========   static function prototype   ==========
static bool chk_que_is_exist(msg_queue *queue);
static int set_que_size(msg_queue *queue, const long int size);
static int get_que_next_index(const int index, const int size);


// ==========   open function   ==========
// initial create queue and queue container
int init_queue(msg_queue **queue, const long int size)
{
	AVER_ENTER;

	// check whether is parameter queue null pointer or not
	if (chk_que_is_exist(*queue))
	{
		AVER_LEAVE;
		return NOT_NULL_POTINER;
	}

	int ret = SUCCESS;  // declare return code

	do
	{
		// create a queue object
		if ((*queue = malloc(sizeof(msg_queue))) == NULL)
		{
			log("create queue structure fail");
			ret = MALLOC_FAIL;
			break;
		}

		// initial
		memset(*queue, 0, sizeof(msg_queue));
		(*queue)->info.drop_mech = QUEUE_OP_DROP_NEW_DATA;
		pthread_mutex_init(&((*queue)->info.mutex), NULL);

		// set queue size and allocate a space for queue
		if ((ret = set_que_size(*queue, size)) == MALLOC_FAIL)
		{
			log("create queue size fail");
			break;
		}

		log("finish init queue");
	} while(0);

	// if memory allocate is fail, it must be release
	if (ret == MALLOC_FAIL)
	{
		if (queue)
		{
			free(queue);
			queue = NULL;
		}
	}
	AVER_LEAVE;
	return ret;
}

// uninitial queue and queue container
int uninit_queue(msg_queue **queue)
{
	AVER_ENTER;

	if (*queue)
	{
		// first free queue data
		if ((*queue)->q)
		{
			free((*queue)->q);
			(*queue)->q = NULL;
		}

		// second free queue
		free(*queue);
		*queue = NULL;
	}

	AVER_LEAVE;
	return SUCCESS;
}

// set the operate. if the queue is full, using what is the mechanism
void op_set_drop_mech(msg_queue *queue, avt_queue_op op)
{
	AVER_ENTER;

	// check whether is parameter queue null pointer or not
	if (!chk_que_is_exist(queue))
	{
		AVER_LEAVE;
		return;
	}

	// assign the setting
	queue->info.drop_mech = op;

	AVER_LEAVE;
}

// push data in rear of the queue
int op_enqueue(msg_queue *queue, m_q_data *msg_data)
{
	AVER_ENTER;

	// check whether is parameter queue null pointer or not
	if (!chk_que_is_exist(queue))
	{
		AVER_LEAVE;
		return NULL_POINTER;
	}

	int ret = SUCCESS;  // declare return code

	pthread_mutex_lock(&(queue->info.mutex));  // lock mutex flag
	do
	{
		// checking the queue is full or not
		if (op_is_full(queue))
		{
			static int drop_count = 0;

			log("queue is full!!  drop msg : %d", ++drop_count);

			// choose drop mechanism
			if (queue->info.drop_mech == QUEUE_OP_DROP_OLD_DATA)
			{
				// get next rear index, drop the old message
				queue->info.front = get_que_next_index(queue->info.front, queue->info.queue_size);
			}
			else if (queue->info.drop_mech == QUEUE_OP_DROP_NEW_DATA)
			{
				// drop the new message
				ret = FAIL;
				break;
			}
		}

		// assign data
		queue->q[queue->info.rear].level = msg_data->level;  // assign message level
		queue->q[queue->info.rear].time = msg_data->time;	// assign message time
		if (msg_data->msg)  // assign message log
		{
			memset(queue->q[queue->info.rear].msg, 0, G_MSG_SIZE);
			memcpy(queue->q[queue->info.rear].msg, msg_data->msg, MAX_SIZEOF(msg_data->msg, G_MSG_SIZE));
		}
		if (msg_data->fun_name)  // assign function name
		{
			memset(queue->q[queue->info.rear].fun_name, 0, G_FUN_NAME_SIZE);
			memcpy(queue->q[queue->info.rear].fun_name, msg_data->fun_name, MAX_SIZEOF(msg_data->fun_name, G_FUN_NAME_SIZE));
		}
		if (msg_data->line)  // assign function line number
			queue->q[queue->info.rear].line = msg_data->line;
		if (msg_data->tag)  // assign function tag
		{
			memset(queue->q[queue->info.rear].tag, 0, G_TAG_SIZE);
			memcpy(queue->q[queue->info.rear].tag, msg_data->tag, MAX_SIZEOF(msg_data->tag, G_TAG_SIZE));
		}

		// 測試用的啦
//		log("111111 front : %d, rear : %d\nmsg : %s\nfunction name : %s\ntag : %s  line %d\n",
//				queue->info.front, queue->info.rear, queue->q[queue->info.rear].msg,
//				queue->q[queue->info.rear].fun_name, queue->q[queue->info.rear].tag, queue->q[queue->info.rear].line);

		// get next rear index
		queue->info.rear = get_que_next_index(queue->info.rear, queue->info.queue_size);
	} while(0);
	pthread_mutex_unlock(&(queue->info.mutex));  // unlock mutex flag

	AVER_LEAVE;
	return ret;
}

// get data from front of the queue
m_q_data *op_dequeue(msg_queue *queue)
{
	AVER_ENTER;

	// check whether is parameter queue null pointer or not
	if (!chk_que_is_exist(queue))
	{
		AVER_LEAVE;
		return NULL;
	}

	m_q_data *ret = NULL;  // declare return value

	pthread_mutex_lock(&(queue->info.mutex));  // lock mutex flag
	do
	{
		// checking the queue is empty or not
		if (op_is_empty(queue))
		{
			log("queue is empty!!");
			ret = NULL;
			break;
		}

		// get data
		ret = &(queue->q[queue->info.front]);

		// 測試用的啦
//		log("222222 front : %d, rear : %d\nmsg : %s\nfunction name : %s\ntag : %s  line %d\n",
//						queue->info.front, queue->info.rear, queue->q[queue->info.front].msg,
//						queue->q[queue->info.front].fun_name, queue->q[queue->info.front].tag, queue->q[queue->info.front].line);

		// get next front index
		queue->info.front = get_que_next_index(queue->info.front, queue->info.queue_size);
	} while(0);
	pthread_mutex_unlock(&(queue->info.mutex));  // unlock mutex flag

	AVER_LEAVE;
	return ret;
}

// clear all of the queue data
int op_clear(msg_queue *queue)
{
	AVER_ENTER;

	// check whether is parameter queue null pointer or not
	if (!chk_que_is_exist(queue))
	{
		AVER_LEAVE;
		return NULL_POINTER;
	}

	int ret = SUCCESS;  // declare return code

	// clear data from front to rear
	while (!op_is_empty(queue))
	{
		// clear data
		queue->q[queue->info.front].msg = NULL;

		// get next index
		queue->info.front = get_que_next_index(queue->info.front, queue->info.queue_size);
	}

	AVER_LEAVE;
	return ret;
}

// check whether is the queue full or not
bool op_is_full(msg_queue *queue)
{
	AVER_ENTER;

	// check whether is parameter queue null pointer or not
	if (!chk_que_is_exist(queue))
	{
		AVER_LEAVE;
		return NULL_POINTER;
	}

	AVER_LEAVE;
	return (queue->info.rear + 1) % queue->info.queue_size == queue->info.front;
}

// check whether is the queue empty or not
bool op_is_empty(msg_queue *queue)
{
	AVER_ENTER;

	// check whether is parameter queue null pointer or not
	if (!chk_que_is_exist(queue))
	{
		AVER_LEAVE;
		return NULL_POINTER;
	}

	AVER_LEAVE;
	return queue->info.front == queue->info.rear;
}

// NOTE : it's not regular function
// get queue's rear message pointer, NOTE : it's convenient to store directly for reducing CPU efficacy
m_q_data *get_que_rear_msg_item(msg_queue *queue)
{
	AVER_ENTER;
	AVER_LEAVE;
	return &(queue->q[queue->info.rear]);
}

// ==========   static function prototype   ==========
// check whether is the queue NULL or not
static bool chk_que_is_exist(msg_queue *queue)
{
	AVER_ENTER;

	bool ret = true;  // declare return code

	//check whether is the queue allocated or not
	if (!queue)
	{
		log("parameter queue is NULL");
		ret = false;
	}

	AVER_LEAVE;
	return ret;
}

// setting queue size
static int set_que_size(msg_queue *queue, const long int size)
{
	AVER_ENTER;

	// declare ret is return code
	// declare tmp_que_count is temporal counting queue index
	// declare i is counting index
	int ret = SUCCESS, i = 0;
	int tmp_que_count = 0;

	// set queue size
	queue->info.queue_size = size;

	do
	{
		// create queue space, check whether did it be allocated was success.
		if ((queue->q = malloc(sizeof(m_q_data) * size)) == NULL)
		{
			log("queue size memory allocate fail");
			ret = MALLOC_FAIL;
			break;
		}
		memset(queue->q, 0, sizeof(m_q_data) * size);  // initial memory space

		// then create space of the every queue
		for (i = tmp_que_count = 0;i < queue->info.queue_size; i++, tmp_que_count++)
		{
			// memory allocate a space for message
			if ((queue->q[i].msg = malloc(sizeof(char) * G_MSG_SIZE)) == NULL)
			{
				log("queue msg size memory allocate fail on %d", i);
				ret = MALLOC_FAIL;
				break;
			}
			memset(queue->q[i].msg, 0, sizeof(char) * G_MSG_SIZE);  // initial memory space

			// memory allocate a space for function name
			if ((queue->q[i].fun_name = malloc(sizeof(char) * G_FUN_NAME_SIZE)) == NULL)
			{
				log("queue function name size memory allocate fail on %d", i);
				ret = MALLOC_FAIL;
				break;
			}
			memset(queue->q[i].fun_name, 0, sizeof(char) * G_FUN_NAME_SIZE);  // initial memory space

			// memory allocate a space for tag
			if ((queue->q[i].tag = malloc(sizeof(char) * G_TAG_SIZE)) == NULL)
			{
				log("queue tag size memory allocate fail on %d", i);
				ret = MALLOC_FAIL;
				break;
			}
			memset(queue->q[i].tag, 0, sizeof(char) * G_TAG_SIZE);  // initial memory space
		}
	} while(0);

	// if memory allocate fail
	if (ret == MALLOC_FAIL)
	{
		// free the space has been allocated
		for (i = 0; i <= tmp_que_count; i++)
		{
			if (queue->q[i].msg)
			{
				free(queue->q[i].msg);
				queue->q[i].msg = NULL;
			}
			if (queue->q[i].fun_name)
			{
				free(queue->q[i].fun_name);
				queue->q[i].fun_name = NULL;
			}
			if (queue->q[i].tag)
			{
				free(queue->q[i].tag);
				queue->q[i].tag = NULL;
			}
		}

		// free the queue
		if (queue->q)
		{
			free(queue->q);
			queue->q = NULL;
		}
	}

	AVER_LEAVE;
	return ret;
}

// get queue's rear next index
static int get_que_next_index(const int index,const int size)
{
	AVER_ENTER;
	AVER_LEAVE;
	return (index + 1) % size;
}

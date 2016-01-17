/*
 * queue_model.h
 *
 *  Created on: 2013/10/4
 *      Author: Jieyi
 */

#ifndef QUEUE_MODEL_H_
#define QUEUE_MODEL_H_

#include <stdbool.h>
#include <time.h>

#include "../header/avt_log.h"

typedef struct m_queue										// queue's data storage
{
	avt_log_level level;									// message level
	time_t time;											// message print time
	char *msg;												// message data
	char *fun_name;											// process function name
	char *tag;												// process function tag
	int line;												// process call the avt_log line number
} m_q_data;
typedef struct message_queue msg_queue;					// store log queue structure


int init_queue(msg_queue **queue, const long int size);	// initial create queue and queue container
int uninit_queue(msg_queue **queue);						// uninitial queue and queue container
void op_set_drop_mech(msg_queue *queue, avt_queue_op op);	// set the operate. if the queue is full, using what is the mechanism
int op_enqueue(msg_queue *queue, m_q_data *msg_data);		// push data in rear of the queue
m_q_data *op_dequeue(msg_queue *queue);					// get data from front of the queue
int op_clear(msg_queue *queue);							// clear all of the queue data
bool op_is_full(msg_queue *queue);							// check whether is the queue full or not
bool op_is_empty(msg_queue *queue);						// check whether is the queue empty or not

// NOTE : it's not regular function
m_q_data *get_que_rear_msg_item(msg_queue *queue);		// get queue's rear data pointer, NOTE : it's convenient to store directly
															// for reducing CPU efficacy
#endif /* QUEUE_MODEL_H_ */

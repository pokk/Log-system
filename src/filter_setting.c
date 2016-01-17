/*
 * filter_setting.c
 *
 *  Created on: 2013/10/2
 *      Author: Jieyi
 */

#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#include "../header/avt_log.h"
#include "../header/common_header.h"

// ==========   define   ==========
// log setting default value
#define DEFAULT_LOG_LEVEL				LOG_LEVEL_INFO	// default log output level
#define DEFAULT_TAG_LENGTH				50				// default tag length
#define DEFAULT_TAG_HASH_INDEX_SIZE		10				// default maximum hash table size


// ==========   structure prototype   ==========
struct level_name										// log level name structure
{
	char *c_name;										// log level name
	int	c_val;											// log level
};
typedef struct tag_hash_table							// tag hash table
{
	char tag[DEFAULT_TAG_LENGTH];						// save the tag
	struct tag_hash_table *next;						// next pointer
}tag_hash_table;


// ==========   static global variable   ==========
// default flag
static int g_log_level = DEFAULT_LOG_LEVEL;			// save log level setting flag and initial
static int g_filter_op = FILTER_OP_KEEP;				// save filter operation flag and initial
// tag hash table
static tag_hash_table g_tag[DEFAULT_TAG_HASH_INDEX_SIZE];  // save the tag information
//static struct level_name g_log_lv_name[] = {
//	{"INFO",	LOG_LEVEL_INFO},
//	{"DEBUG",	LOG_LEVEL_DEBUG},
//	{"WARN",	LOG_LEVEL_WARNING},
//	{"ERROR",	LOG_LEVEL_ERROR},
//};


// ==========   open function prototype   ==========
int set_log_level(avt_log_level level);
int set_tag_filter(avt_filter_op operate, char *tag);
bool filter_log_level(avt_log_level msg_lv);
bool filter_tag(char *tag);


// ==========   static function prototype   ==========
static void split_tag(char *tag);
static bool filter_keep_tag(char *tag);
static bool filter_drop_tag(char *tag);
static void reset_tag_hash_table();
static int hash_table_put_tag(char *tag);
static bool hash_table_check_tag(char *tag);
static int calculate_index_of_string(char *tag);

// ==========   open function   ==========
// set log level, if user log level lower than setting log level, it don't output; otherwise, it will output.
int set_log_level(avt_log_level level)
{
	AVER_ENTER;

	// assign setting
	g_log_level = level;

	AVER_LEAVE;
	return SUCCESS;
}

// set flag operation and split tag name
int set_tag_filter(avt_filter_op operate, char *tag)
{
	AVER_ENTER;

	// assign setting
	g_filter_op = operate;

	// initial hash table
	reset_tag_hash_table();

	// split tag name
	split_tag(tag);

	AVER_LEAVE;
	return SUCCESS;
}

// filter log level, the log level compare with the setting level, then the level lower will be filtered.
bool filter_log_level(avt_log_level msg_lv)
{
	AVER_ENTER;
	AVER_LEAVE;
	return msg_lv < g_log_level;
}

// filter tag, the tag compare with the saving tag or dropping tag
bool filter_tag(char *tag)
{
	AVER_ENTER;

	int ret = true;

	// choose the filter option
	switch (g_filter_op)
	{
		case FILTER_OP_DROP:
			ret = filter_drop_tag(tag);
			break;
		case FILTER_OP_KEEP:
			ret = filter_keep_tag(tag);
			break;
	}

	AVER_LEAVE;
	return ret;
}

// show the hash table all value, but it's a test function
void show_all_hash_table_value()
{
	int i = 0;
	tag_hash_table *ptr = NULL;

	for (i = 0; i < DEFAULT_TAG_HASH_INDEX_SIZE; i++)
	{
		printf("%d : %s  ", i, g_tag[i].tag);
		for (ptr = g_tag[i].next; ptr; ptr = ptr->next)
			printf("v.%s  ", ptr->tag);
		printf("\n");
	}
}


// ==========   static function   ==========
// split the input tag
static void split_tag(char *tag)
{
	AVER_ENTER;

	char buff[DEFAULT_TAG_LENGTH * DEFAULT_TAG_HASH_INDEX_SIZE * DEFAULT_TAG_HASH_INDEX_SIZE] = {0};
	char *tok = NULL;

	// initial
	memset(buff, 0, sizeof(buff));

	// copy input string in our buffer
	strcpy(buff, tag);

	// split the string
	tok = strtok(buff, ";");
	while (tok != NULL)
	{
		// put the tag in the hash table
		hash_table_put_tag(tok);
		tok = strtok(NULL, ";");
	}

	AVER_LEAVE;
}

// filter the tag which we want to keep
static bool filter_keep_tag(char *tag)
{
	AVER_ENTER;
	AVER_LEAVE;
	// if check tag, we will keep it. And return "False"
	return !hash_table_check_tag(tag);
}

// filter the tag which we want to drop
static bool filter_drop_tag(char *tag)
{
	AVER_ENTER;
	AVER_LEAVE;
	// if check tag, we will drop it. And return "True"
	return hash_table_check_tag(tag);
}

// reset and free the space from the tag hash table
static void reset_tag_hash_table()
{
	AVER_ENTER;

	int i = 0;
	tag_hash_table *ptr = NULL;

	// reset and free the space which was allocated
	for (i = 0; i < DEFAULT_TAG_HASH_INDEX_SIZE; i++)
	{
		if (g_tag[i].next)  // free all of the space which was allocated
		{
			tag_hash_table *prev = NULL;

			// take a temporary pointer to save the space of allocated
			prev = g_tag[i].next;
			ptr = prev->next;

			while (prev)
			{
				free(prev);
				prev = ptr;
				// if any free space remain, and ptr point the last pointer
				if (ptr)
					ptr = ptr->next;
			}
		}
		memset(&g_tag[i], 0, sizeof(tag_hash_table));
	}

	AVER_LEAVE;
}

// put tag in the hash table
static int hash_table_put_tag(char *tag)
{
	AVER_ENTER;

	// declare ret for return value
	// declare tag_index for calculate index
	// declare ptr for running to array
	int ret = SUCCESS;
	int tag_index = calculate_index_of_string(tag);
	tag_hash_table *ptr = g_tag[tag_index].next;

	do
	{
		// check it whether is in the hash table or not
		if (hash_table_check_tag(tag))
		{
			ret = FAIL;
			break;
		}

		// if the first space is empty
		if (!strcmp(g_tag[tag_index].tag, "\0"))
			memcpy(g_tag[tag_index].tag, tag, strlen(tag));
		else  // if the first space is not empty
		{
			bool head = true;

			// search this tag index's array to the last pointer
			while (ptr)
			{
				// stamp a flag to judge head
				head = false;
				if (!ptr->next)
					break;

				ptr = ptr->next;
			}

			// after head space is null
			if (head)
			{
				// allocate a free space
				g_tag[tag_index].next = malloc(sizeof(tag_hash_table));
				ptr = g_tag[tag_index].next;
			}
			else
			{
				// allocate a free space
				ptr->next = malloc(sizeof(tag_hash_table));
				ptr = ptr->next;
			}
			// reset the new space and copy data
			memset(ptr, 0, sizeof(tag_hash_table));
			memcpy(ptr->tag, tag, strlen(tag));
		}
	} while(0);

	AVER_LEAVE;
	return ret;
}

// check whether tag is in the hash table or not
static bool hash_table_check_tag(char *tag)
{
	AVER_ENTER;

	// declare ret for return value
	// declare tag_index for calculate index
	// declare ptr for running to array
	int ret = false;
	int tag_index = calculate_index_of_string(tag);
	tag_hash_table *ptr = NULL;

	// search this tag index's array
	for (ptr = &g_tag[tag_index]; ptr; ptr = ptr->next)
		if (!strcmp(tag, ptr->tag))  // find the same string in the hash table
		{
			ret = true;
			break;
		}

	AVER_LEAVE;
	return ret;
}

// calculate number of the input string
static int calculate_index_of_string(char *tag)
{
	AVER_ENTER;

	int i = 0, score = 0, index = 0;

	// calculate score
	for (i = 0; i < strlen(tag); i++)
		score += *(tag + i);

	// calculate the hash index
	index = score % DEFAULT_TAG_HASH_INDEX_SIZE;

	AVER_LEAVE;
	return index;
}

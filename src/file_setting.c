/*
 * file_setting.c
 *
 *  Created on: 2013/10/2
 *      Author: Jieyi
 */

#include <string.h>

#include "../header/avt_log.h"
#include "../header/queue_model.h"
#include "../header/file_setting.h"
#include "../header/common_header.h"

// ==========   define   ==========
// file setting default value
#define DEFAULT_FILE_PATH			"/tmp/"				// default file path
#define DEFAULT_FILE_NAME			"proc_log.txt"		// default file name
#define DEFAULT_FILE_SEPARATE		FILE_SEP_BY_SIZE	// default file separate mode
#define DEFAULT_FILE_ROTATE			1					// default file number of rotate
#define DEFAULT_FILE_MAX_TIME		3600				// default file max time
#define DEFAULT_FILE_MAX_SIZE		1024				// default file max size


// ==========   structure prototype   ==========
typedef struct file_path_with_pid						// save the each other file setting structure
{
	char path[100];										// save file path plus file name
	int pid_list[20];									// save PID list
	int sep_mode;										// save separate mode
	int num_rotate;										// save number of the rotate file
	int max_time;										// save what time to do the rotate
	int max_size;										// save what size to do the rotate
	struct file_path_with_pid *next;					// next pointer
}file_path_with_pid;


// ==========   static global variable   ==========
static file_path_with_pid *g_file_set = NULL;			// save file setting and initialized


// ==========   static function prototype   ==========
static int combine_file_path(char *path, char *name, char *file_path);
static int modify_file_path(char *path, char *out_path);
static int modify_file_name(char *name, char *out_name);
static char *search_file_path(int proc_pid);
static int open_log_file(char *file_path, char *file_name);
static int close_log_file(FILE *file_des);
static void write_log_file(char *log);

// ==========   open function   ==========
// set log file setting
void set_file_log_setting(avt_file_setting setting)
{
	AVER_ENTER;

	// declare path for save the file path and file name
	char path[100] = {0};
	file_path_with_pid *ptr = g_file_set;

	// initial the array
	memset(path, 0, sizeof(path));

	// normalize the file path
	combine_file_path(setting.file_path, setting.file_path, path);

	// allocate a new space to save the setting
	if (!ptr)
	{
		// trail to list's tail
		for (ptr = g_file_set; ptr; ptr = ptr->next)
		{
			// setting path compare with all of the list's path
			if (!strcmp(ptr->path, path))
			{
				log("there have been in the list...");
				AVER_LEAVE;
				return;
			}
		}
	}
	// if memory allocated fail
	if ((ptr = malloc(sizeof(file_path_with_pid))) == NULL)
		log("memory allocated fail...");

	// initial
	memset(ptr, 0, sizeof(file_path_with_pid));

	// copy all setting to global setting
	// copy the file path
	strcpy(ptr->path, path);
	// TODO : 要改變儲存的資訊, 若為 PID 的話會有一些問題, 根本無法分檔案
	// copy process id information
//	ptr->pid_list[0] =
	// copy separate mode
	ptr->sep_mode = (setting.separate_mode ? setting.separate_mode : DEFAULT_FILE_SEPARATE);
	// copy number of rotate file
	ptr->num_rotate = (setting.num_rotate_file ? setting.num_rotate_file : DEFAULT_FILE_ROTATE);
	// copy maximum time of file
	ptr->max_time = (setting.max_time ? setting.max_time : DEFAULT_FILE_MAX_TIME);
	// copy maximum size of file
	ptr->max_size = (setting.max_size ? setting.max_size : DEFAULT_FILE_MAX_SIZE);

	AVER_LEAVE;
}

// log information to the management to control
void file_management_distribute(m_q_data *data)
{
	AVER_ENTER;

	// find the path name belong to the PID

	// open file

	// write the log

	// close file


	AVER_LEAVE;
}

// ==========   static function   ==========
// organize the file path and the file name
static int combine_file_path(char *path, char *name, char *file_path)
{
	AVER_ENTER;

	int ret = SUCCESS;

	// First, modify the file path
	if (!path)  // if path is NULL
		strcpy(file_path, DEFAULT_FILE_PATH);  // assign the default value
	else
		modify_file_path(path, file_path);

	// Finally, modify the file name
	if (!name)  // if name if NULL
		strcpy(file_path, DEFAULT_FILE_NAME);  // assign the default value
	else
		modify_file_name(name, file_path);

	AVER_LEAVE;
	return ret;
}

// modify the file path to correct path which format
static int modify_file_path(char *path, char *out_path)
{
	AVER_ENTER;

	int ret = SUCCESS;

	// check the last character is '/' or not. if not, add the '/' in the last
	if (*(path + strlen(path) - 1) != '/')
		sprintf(out_path, "%s/", path);

	AVER_LEAVE;
	return ret;
}

// modify the file name to correct path which format
static int modify_file_name(char *name, char *out_name)
{
	AVER_ENTER;

	int ret = SUCCESS;

	// catch the file name directly in the last string
	if (*name == '/')
		strcat(out_name, (name + 1));

	AVER_LEAVE;
	return ret;
}

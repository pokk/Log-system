/**
 * @file        avt_log.h
 * @author      Jieyi
 * @brief       write log client
 * @version     0.0.1
 * @date        2013.09.13
 */

#ifndef AVT_LOG_H
#define	 AVT_LOG_H

#define AVT_log(level, tag, format, ...) avt_log(level, __func__, __LINE__, tag, format, ##__VA_ARGS__)

/**
 * @struct		set file setting
 * @brief		set file attribute and setting
 */
typedef struct file_setting
{
    /** @brief set file path. default file path is /tmp */
    char *file_path;
    /** @brief set file name. default file name is PID_log.txt */
    char *file_name;

    /** @brief rotate file by time or file size, default is by file size, reference "enum file_separate_mode" */
    int separate_mode;
    /** @brief number of rotate file, default is 1 */
    int num_rotate_file;
    /** @brief time, the unit is second. default is 3600s */
    int max_time;
    /** @brief file size, the unit is kilobytes. default is 1024 KBs */
    int max_size;
} avt_file_setting;

typedef struct network_setting
{
    /** @brief UDP multicast group IP */
    char *udp_multicast_ip;
    /** @brief UDP multicast group port */
    int udp_multicast_port;
} avt_network_setting;

/**
 * @enum		IO flag operation
 * @brief		IO attribute
 */
typedef enum io_attribute
{
	IO_ATTR_DEFA				=	0x00000,	/*!< default setting, close all output function */
	IO_ATTR_CONS				=	0x00001,	/*!< open output to the console */
	IO_ATTR_FILE				=	0x00002,	/*!< open output to the file */
	IO_ATTR_UDP_MULTICAST		=	0x00004,	/*!< open use UDP multicast*/
	IO_ATTR_UDP					=	0x00008,	/*!< open use UDP */
} avt_io_attr;

/**
 * @enum		log flag operation
 * @brief		log attribute
 */
typedef enum log_attribute
{
	LOG_ATTR_DEFA				=	0x00000,	/*!< default setting, open all output format information */
	LOG_ATTR_IGN_DATE			=	0x00001,	/*!< hide date in log information */
	LOG_ATTR_IGN_TAG			=	0x00002,	/*!< hide tag in log information */
	LOG_ATTR_IGN_PID			=	0x00004,	/*!< hide PID in log information */
	LOG_ATTR_IGN_FUN_NAME		=	0x00008,	/*!< hide function name in log information */
} avt_log_attr;

/**
 * @enum		file separate mode
 * @brief		separate method index
 */
typedef enum file_separate_mode
{
	FILE_SEP_BY_SIZE			=	0x00001,	/*!< separate file by file size */
	FILE_SEP_BY_TIME			=	0x00002,	/*!< separate file by time */
} avt_file_sep;

/**
 * @enum		log level
 * @brief		define log level 0 ~ 3
 */
typedef enum log_level
{
	LOG_LEVEL_INFO = 0,
	LOG_LEVEL_DEBUG,
	LOG_LEVEL_WARNING,
	LOG_LEVEL_ERROR,
} avt_log_level;

/**
 * @enum		filter operation
 * @brief		control filter or keep tag.
 */
typedef enum filter_operate
{
	FILTER_OP_DROP,								/*!< filter tag which we don't want */
	FILTER_OP_KEEP,								/*!< keep tag which we must save */
} avt_filter_op;

/**
 * @enum		queue operate
 * @brief		if queue is full, then it will do operation.
 */
typedef enum queue_operate
{
	QUEUE_OP_DROP_NEW_DATA,						/*!< dropping message which will put in the queue */
	QUEUE_OP_DROP_OLD_DATA						/*!< dropping message which is older in the queue */
} avt_queue_op;



/**
 * @brief		set file setting.
 * @details		If no set attribute, the settings is default.
 * @param[in]	setting         Describe file setting
 * @return		no return value
 */
void avt_set_file_setting(avt_file_setting setting);

/**
 * @brief       set UDP multicast network setting.
 * @param[in]   setting         Describe UDP network setting
 * @return      no return value
 */
void avt_set_udp_network(avt_network_setting setting);

/**
 * @brief       set file setting.
 * @details     If no set attribute, the settings is default.
 *              "io_attr_flag" default is close all function.
 * @param[in]   io_attr_flag    Set IO attribute(avm_io_attr), and using "or" input parameter.
 *              Ex : IO_ATTR_CONS | IO_ATTR_UDP_MULTICAST
 * @return      no return value
 */
void avt_set_output_setting(int io_attr_flag);

/**
 * @brief       set log output format setting.
 * @details     If no set attribute, the settings is default.
 *              "log_attr_flag" default is open all function.
 * @param[in]   log_attr_flag   Set log attribute(avm_log_attr), and using "or" input parameter.
 *              Ex : LOG_ATTR_IGN_DATE | LOG_ATTR_IGN_PID | LOG_ATTR_IGN_TAG
 * @return      no return value
 */
void avt_set_log_output_format_setting(int log_attr_flag);

/**
 * @brief       set filter level.
 * @details     If no set attribute, the settings is default.
 *              "level" default is "Info" level.
 * @param[in]   level           We want to filter log level.
 * @return      no return value
 */
void avt_set_log_level(avt_log_level level);

/**
 * @brief       set filter specific tag.
 * @details     If no set, default setting is no filter any tag.
 *              Tag format "moduleA;moduleB;moduleC", use ";" separate tag name.
 * @param[in]   operate         Drop or Keep operation.
 * @param[in]   tag             Log tag.
 * @return      no return value
 */
void avt_set_filter(avt_filter_op operate, const char *tag);

/**
 * @brief       set log queue size.
 * @details     If no set, the settings is default.
 * 				"operate" default is QUEUE_OP_DROP_NEW_DATA.
 * 				"queue_size" default is 5000, maximum is 10000.
 * @param[in]   operate         It operate queue dropping message.
 * @param[in]   queue_size		Queue size.
 * @return      no return value
 */
void avt_set_log_queue_size(avt_queue_op operate, int queue_size);

/**
 * @brief       write log to log daemon.
 * @param[in]   level           Setting log level.
 * @param[in]   fun_name        Function name.
 * @param[in]   tag             Tag name.
 * @param[in]   format          Log contents, NOTE : the maximum log size is 512 byes.
 * 								if log length is over than 512 bytes, the log which is over than 512 bytes will be ignored.
 * @param[in]   ...             Indefinite parameters.
 * @return      no return value
 */
void avt_log(avt_log_level level, char *fun_name, int line, char *tag, char *format, ...);

#endif	/* AVM_LOG_H */

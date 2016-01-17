/*
 * File:   main.c
 * Author: Jieyi
 *
 * Created on 2013年9月12日, 下午 5:56
 */

#include <stdlib.h>

#define __avt_log__
//#define __queue_model__

int main(int argc, char** argv)
{

#ifdef __avt_log__
	log_demoap();
	show_all_hash_table_value();
#endif

#ifdef __queue_model__
	queue_demoap();
#endif

    return (EXIT_SUCCESS);
}


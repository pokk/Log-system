/*
 * debug.h
 *
 *  Created on: 2013/10/3
 *      Author: Jieyi
 */

#ifndef DEBUG_H_
#define DEBUG_H_

#include <stdio.h>

// open debug switch
#define __DEBUG_LOG__
//#define __DEBUG_DETAL_LOG__


// show the normal debug message
#ifdef __DEBUG_LOG__
#define log(fmt, arg... )		printf("[%s], " fmt "\n", __FUNCTION__,  ##arg)
#else
#define log(...)				do{} while(0)
#endif


// show function "begin" position and "end" position
#ifdef __DEBUG_DETAL_LOG__
#define AVER_ENTER					printf("[Enter %s]\n", __FUNCTION__)
#define AVER_LEAVE					printf("[Leave %s]\n", __FUNCTION__)
#else
#define AVER_ENTER					do{} while(0)
#define AVER_LEAVE					do{} while(0)
#endif

#endif /* DEBUG_H_ */

/*
 * file_setting.h
 *
 *  Created on: 2013/10/23
 *      Author: Jieyi
 */

#ifndef FILE_SETTING_H_
#define FILE_SETTING_H_

#include "../header/avt_log.h"


void set_file_log_setting(avt_file_setting setting);		// set log file setting
void file_management_distribute(m_q_data *data);		// log information to the management to control


#endif /* FILE_SETTING_H_ */

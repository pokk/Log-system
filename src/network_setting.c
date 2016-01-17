/*
 * network_setting.c
 *
 *  Created on: 2013/10/2
 *      Author: Jieyi
 */

#include "../header/avt_log.h"
#include "../header/common_header.h"

// ==========   define   ==========
// network setting default value
/// TODO : setting the value now, I will modify this value.
#define DEFAULT_NETWORK_UDP_IP		"192.0.0.1"			// default UDP network IP, it's setting temporary IP
#define DEFAULT_NETWORK_UDP_PORT	3456				// default UDP network port, it's setting temporary port

// ==========   static global variable   ==========
static avt_network_setting g_network_set = {			// save network setting and initialized
	.udp_multicast_ip = DEFAULT_NETWORK_UDP_IP,
	.udp_multicast_port = DEFAULT_NETWORK_UDP_PORT
};

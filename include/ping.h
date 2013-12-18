/**\file ping.h
 * \author val
 * \brief ping functions (header)
 * \date December 14, 2013, 14:45 AM
 */

#ifndef __PING_H
#define __PING_H

#include <stdio.h>
#include <netinet/udp.h>
#include <linux/icmp.h>
#include <linux/tcp.h>
#include <linux/ip.h>
#include <ctype.h>
#include <unistd.h>
#include <signal.h>
//#include <netinet/ip_icmp.h>
//#include <netinet/ip.h>

#include "../include/common.h"
#include "../include/log.h"

#define LONGHDR_IP 20
#define LONGHDR_ICMP 8
#define IP_MAXPACKET 1024

/**\brief Show usage of the ping program
 */
void Usage(void);

/**\brief Launch the ping program
 * \param argc Number of args
 * \param argv List of args
 * \return 0
 */
int main_ping(int argc, char* argv[]);

/**\brief SIGINT handler
 * \param sig
 * \return void
 */
void handlerArret(int s);

#endif //__PING_H

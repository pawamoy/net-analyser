/**\file ping.h
 * \brief ping functions (header)
 */

#ifndef __PING_H
#define __PING_H

#include <netinet/udp.h>
#include <linux/icmp.h>
#include <linux/tcp.h>
#include <linux/ip.h>
#include <unistd.h>
#include <signal.h>
//#include <netinet/ip_icmp.h>
//#include <netinet/ip.h>

#include "../include/common.h"
#include "../include/log.h"

#define LONGHDR_IP 20
#define LONGHDR_ICMP 8
#define IP_MAXPACKET 1024

#define DELAY   1
#define LOSS    2
#define FAILURE 3

/**\brief Show usage of the ping program
 * \author val
 */
void UsagePing(void);

/**\brief Launch the ping program
 * \param address Address/domain to ping
 * \return 0
 * \author val
 */
int main_ping(char* address);

/**\brief SIGINT handler
 * \param sig
 * \return void
 * \author val
 */
void handlerArret(int s);

#endif //__PING_H

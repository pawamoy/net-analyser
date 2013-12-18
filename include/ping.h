/**\file ping.h
 * \author val
 * \brief ping functions (header)
 * \date December 14, 2013, 14:45 AM
 */

#ifndef __PING_H
#define __PING_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <ifaddrs.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <netinet/udp.h>
#include <linux/icmp.h>
#include <linux/tcp.h>
#include <linux/ip.h>
#include <ctype.h>
//#include <netinet/ip_icmp.h>
//#include <netinet/ip.h>
#include <unistd.h>

/**\brief Get IP from an hostname (ie. google.fr)
 * \param hostname String of domain name
 * \return NULL on error, a pointer to char
 */
char* GetIPFromHostname(const char *hostname);

/**\brief Get my own IP address
 * \return String (address)
 */
char* GetMyIP(void);

/**\brief Parse address to see if it is my address (not local)
 * \param addr Pointer to a string (address)
 * \return 1(yes), 0(no)
 */
int IsMyAddress(char* addr);

#endif //__PING_H

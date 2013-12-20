/**\file common.h
 * \brief ip relative functions
 * \date December 18, 2013, 17:07
 */

#ifndef __COMMON_H
#define __COMMON_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <netdb.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <ifaddrs.h>
#include <ctype.h>

#define MAX_ADDRESS 128

typedef struct sockaddr_in Sockin;
typedef int Socket;

/**\brief Get IP from an hostname (ie. google.fr)
 * \param hostname String of domain name
 * \return NULL on error, a pointer to char
 * \author tom
 */
char* GetIPFromHostname(const char *hostname);

/**\brief Get my own IP address
 * \return String (address)
 * \author tim
 */
char* GetMyIP(void);

/**\brief Parse address to see if it is my address (not local)
 * \param addr Pointer to a string (address)
 * \return 1(yes), 0(no)
 * \author tim
 */
int IsMyAddress(char* addr);

/**\brief Get host name from an IP address
 * \param ip The IP address
 * \return A string containing host name
 * \author tim
 */
char* GetHostNameFromIP(const char* ip);

#endif // __COMMON_H

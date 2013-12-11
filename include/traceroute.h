/**\file traceroute.h
 * \author tim
 * \brief traceroute main (header)
 * \date December 11, 2013, 15:39 AM
 */

#ifndef __TRACEROUTE_H
#define __TRACEROUTE_H

#include <stdio.h>
#include <stdlib.h>

#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <netinet/in.h>

#include <linux/ip.h>

#include <unistd.h>

typedef int Socket;

/**\brief Show usage of the program
 */
void Usage(void);

/**\brief Set TTL field (IP header) for socket 
 * \param s socket identifier
 * \param ttl time-to-live field (IP header)
 * \return 0(false) on failure, 1(true) on success
 */ 
int SetTTL(Socket s, int ttl);

/**\brief Opens a raw socket
 * May print an error, and then exit with code 1
 * \return Socket
 */
Socket OpenRawSocket();

/**\brief Constructs an IP header
 * \return Size of the header
 */
int ConstructIPHeader(struct iphdr* iph, const unsigned int ttl);

#endif // __TRACEROUTE_H

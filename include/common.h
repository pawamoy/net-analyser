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
#include <netinet/udp.h>
#include <linux/icmp.h>
#include <linux/tcp.h>
#include <linux/ip.h>

#define MAX_ADDRESS 128
#define MAX_PACKET 1024

#define ICMP_LEN 28
#define UDP_LEN 28
#define TCP_LEN 40

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

/**\brief Parse address to see if it is my address (not local).
 * Search for address like XX[X].*, different from 127.0.0.1
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

/**\brief Set TTL field (IP header) for socket 
 * \param s socket identifier
 * \param ttl time-to-live field (IP header)
 * \return 0(false) on failure, 1(true) on success
 * \author tom
 */
int SetTTL(Socket s, int ttl);

/**\brief Set receiving timeout for socket 
 * \param s socket identifier
 * \param to Timeout to set
 * \return 0(false) on failure, 1(true) on success
 * \author tom
 */
int SetRCVTimeOut(Socket s, struct timeval to);

/**\brief Set sending timeout for socket 
 * \param s socket identifier
 * \param to Timeout to set
 * \return 0(false) on failure, 1(true) on success
 * \author tom
 */
int SetSNDTimeOut(Socket s, struct timeval to);

/**\brief Set HDRINCL to true (headers included) 
 * \param s socket identifier
 * \return 0(false) on failure, 1(true) on success
 * \author tom
 */
int SetHDRINCL(Socket s);

/**\brief Opens a raw socket.
 * May print an error, and then exit with code 1
 * \param protocol I:ICMP, U:UDP, T:TCP, default:TCP (same with lowercase)
 * \return Socket
 * \author tim
 */
Socket OpenRawSocket(char protocol);

/**\brief Opens a dgram socket.
 * May print an error, and then exit with code 1
 * \param protocol I:ICMP, U:UDP, T:TCP, default:TCP (same with lowercase)
 * \return Socket
 * \author tim
 */
Socket OpenDgramSocket(char protocol);

/**\brief Constructs an IP header
 * \param iph Pointer to an IP Header structure
 * \param ttl Time-to-live value
 * \param source IP address source
 * \param dest IP address destination
 * \param protocol U:UDP, I:ICMP, T:TCP , default:TCP (same with lowercase)
 * \author tim & tom
 */
void ConstructIPHeader(struct iphdr* iph,
        const unsigned int ttl,
        const char *source,
        const char *dest,
        const char protocol);

/**\brief Constructs an ICMP header
 * \param icmph Pointer to an ICMP header structure
 * \author tim
 */
void ConstructICMPHeader(struct icmphdr* icmph);

/**\brief Constructs an TCP header
 * \param icmph Pointer to an TCP header structure
 * \author tom
 */
void ConstructTCPHeader(struct tcphdr *tcph);

#endif // __COMMON_H

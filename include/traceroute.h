/**\file traceroute.h
 * \brief traceroute functions (header)
 * \date December 11, 2013, 15:39 AM
 */

#ifndef __TRACEROUTE_H
#define __TRACEROUTE_H

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

#include <unistd.h>


#define MAX_PACKET 1024
#define MAX_ADDRESS 128

typedef struct packet_icmp {
    struct iphdr iph;
    struct icmphdr icmph;
} PacketICMP;

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

/**\brief Set receiving timeout for socket 
 * \param s socket identifier
 * \param to Timeout to set
 * \return 0(false) on failure, 1(true) on success
 */
int SetRCVTimeOut(Socket s, struct timeval to);

/**\brief Set sending timeout for socket 
 * \param s socket identifier
 * \param to Timeout to set
 * \return 0(false) on failure, 1(true) on success
 */
int SetSNDTimeOut(Socket s, struct timeval to);

/**\brief Set HDRINCL to true (headers included) 
 * \param s socket identifier
 * \return 0(false) on failure, 1(true) on success
 */
int SetHDRINCL(Socket s);

/**\brief Opens a raw socket.
 * May print an error, and then exit with code 1
 * \param protocol I:ICMP, U:UDP, T:TCP, default:TCP (same with lowercase)
 * \return Socket
 */
Socket OpenRawSocket(char protocol);

/**\brief Opens a dgram socket.
 * May print an error, and then exit with code 1
 * \param protocol I:ICMP, U:UDP, T:TCP, default:TCP (same with lowercase)
 * \return Socket
 */
Socket OpenDgramSocket(char protocol);

/**\brief Constructs an IP header
 * \param iph Pointer to an IP Header structure
 * \param ttl Time-to-live value
 * \param source IP address source
 * \param dest IP address destination
 * \param protocol U:UDP, I:ICMP, T:TCP , default:TCP (same with lowercase)
 */
void ConstructIPHeader(struct iphdr* iph,
        const unsigned int ttl,
        const char *source,
        const char *dest,
        const char protocol);

/**\brief Constructs an ICMP header
 * \param icmph Pointer to an ICMP header structure
 * \param seq Sequence number
 */
void ConstructICMPHeader(struct icmphdr* icmph, int seq);

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

/**\brief Get host name from an IP address
 * \param ip The IP address
 * \return A string containing host name
 */
char* GetHostNameFromIP(const char* ip);

/**\brief Traceroute with UDP probes
 * \param rcvt Receive timer
 * \param sndt Send timer
 * \param ttl_t Contains min_ttl, max_ttl, hops in this order
 * \param logfile FILE pointer (if NULL, don't log)
 * \param server Destination data
 * \param my_addr Source data
 */
void LoopUDP(int rcvt, int sndt, int ttl_t[3], FILE* logfile,
             struct sockaddr_in server, struct sockaddr_in my_addr);

/**\brief Traceroute with ICMP probes
 * \param rcvt Receive timer
 * \param sndt Send timer
 * \param ttl_t Contains min_ttl, max_ttl, hops in this order
 * \param logfile FILE pointer (if NULL, don't log)
 * \param server Destination data
 * \param my_addr Source data
 */
void LoopICMP(int rcvt, int sndt, int ttl_t[3], FILE* logfile,
             struct sockaddr_in server, struct sockaddr_in my_addr);

#endif // __TRACEROUTE_H

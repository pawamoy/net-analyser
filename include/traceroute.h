/**\file traceroute.h
 * \brief traceroute functions (header)
 * \date December 11, 2013, 15:39 AM
 */

#ifndef __TRACEROUTE_H
#define __TRACEROUTE_H

#include <netinet/udp.h>
#include <linux/icmp.h>
#include <linux/tcp.h>
#include <linux/ip.h>
#include <unistd.h>

#include "../include/common.h"
#include "../include/log.h"

#define MAX_PACKET 1024

#define ICMP_LEN 28
#define UDP_LEN 28
#define TCP_LEN 40

typedef int Socket;

/**\brief Show usage of the program
 */
void UsageTraceroute(void);

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
 */
void ConstructICMPHeader(struct icmphdr* icmph);

/**\brief Constructs an TCP header
 * \param icmph Pointer to an TCP header structure
 */
void ConstructTCPHeader(struct tcphdr *tcph);

/**\brief Traceroute
 * \param rcvt Receive timer
 * \param sndt Send timer
 * \param ttl_t Contains min_ttl, max_ttl, hops and attempts in this order
 * \param logfile FILE pointer (if NULL, don't log)
 * \param probe Probe to use: i=icmp, t=tcp, u=udp
 * \param server Destination data
 * \param my_addr Source data
 */
void LoopTrace(int rcvt, int sndt, int ttl_t[4], FILE* logfile, char probe,
             struct sockaddr_in server, struct sockaddr_in my_addr);

/**\brief Launch the traceroute program
 * \param argc Number of args
 * \param argv List of args
 * \return 0 on success
 */
int main_traceroute(int argc, char* argv[]);

#endif // __TRACEROUTE_H

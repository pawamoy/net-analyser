/**\file traceroute.h
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
#include <netinet/udp.h>

#include <linux/icmp.h>
#include <linux/ip.h>

#include <unistd.h>

typedef struct packet_udp
{
	struct iphdr iph;
	struct udphdr udph;
} PacketUDP;

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

/**\brief Set TTL field in an IP header
 * \param iph The IP header pointer
 * \param ttl TTL value to set
 */
void SetIPHeaderTTL(struct iphdr* iph, int ttl);

/**\brief Opens a raw socket.
 * May print an error, and then exit with code 1
 * \param protocol I:ICMP, U:UDP, T:TCP, default:TCP (same with lowercase)
 * \return Socket
 */
Socket OpenRawSocket(char protocol);

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

/**\brief Constructs an UDP header
 * \param udph Pointer to an UDP header structure
 */
void ConstructUDPHeader(struct udphdr* udph);

/**\brief Constructs a packet with UDP and IP headers
 * \param buffer Address of a PacketUDP structure
 * \param source Source IP address
 * \param dest Destination IP address
 */
void ConstructUDPPacket(PacketUDP* buffer, const char* source, const char* dest);

/**\brief Get IP from an hostname (ie. google.fr)
 * \param hostname String of domain name
 * \return NULL on error, a pointer to char
 */ 
char *GetIPFromHostname(const char *hostname);

#endif // __TRACEROUTE_H

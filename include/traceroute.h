/**\file traceroute.h
 * \brief traceroute functions (header)
 */

#ifndef __TRACEROUTE_H
#define __TRACEROUTE_H

#include <netinet/udp.h>
#include <linux/icmp.h>
#include <linux/tcp.h>
#include <linux/ip.h>
#include <unistd.h>

#include "../include/common.h"

#define MAX_PACKET 1024

#define ICMP_LEN 28
#define UDP_LEN 28
#define TCP_LEN 40

/**\struct Structure for LoopTrace
 * \see LoopTrace
 * \see NewTrace
 * \author tim
 */
typedef struct str_trace {
	FILE* logfile; /**< FILE pointer to log data, can be null */
	char probe;    /**< Probe method: i/u/t */
	int rcvt,      /**< Receive timer */
		sndt,      /**< Send timer */
		min_ttl,   /**< First ttl to begin with */
		max_ttl,   /**< Maximum ttl to use */
		hops,      /**< Hops at each time */
		attempts;  /**< Number of attempts when packet is lost */
} StrTrace;

/**\brief Structure for main_traceroute
 * \see main_traceroute
 * \see NewTraceRoute
 * \author tim
 */
typedef struct str_traceroute {
	char *address, /**< Domain name or IP address */
		 *myip,    /**< My IP address */
		 *ipstr;   /**< Resolved IP address from domain name */
	int portno;    /**< Port number */
	StrTrace s;    /**< Probe, TTL values, timers, etc... see StrTrace */
} StrTraceRoute;

/**\brief Creates a new StrTrace structure
 * \see struct str_trace
 * \return StrTrace structure
 * \author tim
 */
StrTrace NewTrace(void);

/**\brief Creates a new StrTraceRoute structure
 * \see struct str_traceroute
 * \return StrTraceRoute structure
 * \author tim
 */
StrTraceRoute NewTraceRoute(void);

/**\brief Show usage of traceroute
 * \author tim
 */
void UsageTraceroute(void);

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

/**\brief Traceroute
 * \param s StrTrace structure
 * \see struct str_trace
 * \param server Destination data
 * \param my_addr Source data
 * \return Last used TTL value, or -1 if unreached destination
 * \author tim & tom
 */
int LoopTrace(StrTrace s, Sockin server, Sockin my_addr);

/**\brief Launch the traceroute program
 * \param tr StrTraceRoute structure
 * \see struct str_traceroute
 * \return 0 on success
 * \author tim & tom
 */
int main_traceroute(StrTraceRoute tr);

#endif // __TRACEROUTE_H

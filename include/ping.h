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

#define LONGHDR_IP 20
#define LONGHDR_ICMP 8
#define IP_MAXPACKET 1024

#define DELAY   1
#define LOSS    2
#define FAILURE 3

/**\brief Structure for ping program
 * \see NewPing
 * \author val
 */
typedef struct str_ping {
	int frequency, /**< time to wait between each sent probe */
	    wait,      /**< time to wait for a response (timeout) */
	    ttl,       /**< time to live to use for probes */
	    attempts,  /**< number of attempts when a packet is lost */
	    threshold; /**< delay variation threshold (if attempts=0, beyond threshold = return DELAY */ 
	char *address, /**< Domain name */
	     *myip,    /**< My IP address */
	     *ipstr;   /**< Resolved IP address */
	FILE* logfile; /**< FILE pointer for data logs */
} StrPing;

/**\brief Creates a new StrPing structure
 * \see struct str_ping
 * \return StrPing structure
 * \author val
 */
StrPing NewPing(void);

/**\brief Show usage of ping
 * \author val
 */
void UsagePing(void);

/**\brief Launch the ping program
 * \param p StrPing structure
 * \see struct str_ping
 * \param best_ttl Pointer to integer (used by netanalyser, or ping to set ip ttl)
 * \return 0
 * \author val
 */
int main_ping(StrPing p, int* best_ttl);

/**\brief SIGINT handler
 * \param s Signal number
 * \author val
 */
void handlerArret(int s);

#endif //__PING_H

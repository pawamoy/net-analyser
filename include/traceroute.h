/**\file traceroute.h
 * \brief traceroute functions (header)
 */

#ifndef __TRACEROUTE_H
#define __TRACEROUTE_H


#include <unistd.h>

#include "../include/common.h"

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

unsigned short csum(unsigned short *buf, int len);

#endif // __TRACEROUTE_H

/**\file ping.h
 * \brief netanalyser functions (header)
 */

#ifndef __NETANALYSER_H
#define __NETANALYSER_H

#include "../include/ping.h"
#include "../include/traceroute.h"
#include "../include/log.h"

/**\brief Print gathered statistics on stdout
 * \author tim
 */
void ShowStatistics(void);

/**\brief Handler for netanalyser
 * \param signum Signal number
 * \author tim
 */
void handler(int signum);

/**\brief Will be replaced by main_ping when finished
 * \author tom & val
 */
int ping(char* address, int threshold, int frequency, int attempts, int* best_ttl);

/**\brief Test if host specified by address is joinable (ICMP probes)
 * \param dest Destination IP address
 * \param source My IP address
 * \param max_ping TTL to use
 * \param attempts Number of attempts if no response
 * \return 1 if joinable, 0 else
 * \author tim
 */
int HostIsJoinable(char* dest, char* source, int max_ping, int attempts);

#endif // __NETANALYSER_H

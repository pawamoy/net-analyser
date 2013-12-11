/**\file traceroute.c
 * \author tom
 * \brief traceroute functions
 * \date December 10, 2013, 10:37 AM
 */

#include "../include/traceroute.h"
#include "../include/log.h"

int SetTTL(Socket s, int ttl)
{
    const int *ttl_p = &ttl;
    
    if (setsockopt(s, IPPROTO_IP, IP_TTL, ttl_p, sizeof(*ttl_p)) == -1)
    {
        perror("Cannot set ttl value to socket");
        return 0;
    }
    
    return 1;
}

void Usage()
{
    printf("USAGE: traceroute servername\n");
    exit(-1);
}

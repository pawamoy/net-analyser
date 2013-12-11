/**\file traceroute.c
 * \author tom
 * \brief traceroute functions
 * \date December 10, 2013, 10:37 AM
 */

#include "../include/traceroute.h"
#include "../include/log.h"

void Usage()
{
    printf("USAGE: traceroute servername\n");
    exit(-1);
}

int SetTTL(Socket s, int ttl)
{
    const int *ttl_p = &ttl;
    
    if (setsockopt(s, IPPROTO_IP, IP_TTL, ttl_p, sizeof(*ttl_p)) == -1)
    {
        perror("Cannot set TTL value of socket");
        return 0;
    }
    
    return 1;
}

Socket OpenRawSocket(void)
{
    Socket s;
    if ((s = socket(AF_INET, SOCK_RAW, IPPROTO_RAW)) == -1) {
            perror("Unable to open Raw Socket");
            exit(1);
    }
    return s;
}

/// IN DEV
int ConstructIPHeader(struct iphdr* iph, 
        const unsigned int ttl, 
        const char *dest)
{
    iph->ihl = 5;
    iph->version = 4;
    iph->tos = 16; // Low delay
    iph->id = htons(54321);
    iph->ttl = ttl; // hops
    iph->protocol = 17; // UDP
    /* Source IP address, can be spoofed */
    //~ iph->saddr = inet_addr(inet_ntoa(((struct sockaddr_in *)&if_ip.ifr_addr)->sin_addr));
    iph->saddr = inet_addr("192.168.0.1");
    /* Destination IP address */
    iph->daddr = inet_addr(dest);
    
    return 0;
}

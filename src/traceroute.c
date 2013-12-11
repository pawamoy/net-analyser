/**\file traceroute.c
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
    if ((s = socket(AF_INET, SOCK_RAW, IPPROTO_RAW)) == -1)
    {
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

char *GetIPFromHostname(const char *hostname)
{
    struct addrinfo hints, *p, *res;
    char *ipstr = malloc(sizeof(char)*INET6_ADDRSTRLEN);
    int status;
    
    if (ipstr == NULL)
    {
        perror("GetIPFromHostname(): Couldn't init ipstr");
        return NULL;
    }
    
    memset(&hints, 0, sizeof hints); // make sure the struct is empty
    hints.ai_family = AF_INET; // don't care IPv4 or IPv6
    hints.ai_socktype = SOCK_STREAM; // TCP stream sockets
    hints.ai_flags = AI_PASSIVE; // fill in my IP for me

    if ((status = getaddrinfo(hostname, "80", &hints, &res)) != 0) 
    {
        fprintf(stderr, "GetIPFromHostname(): getaddrinfo error: %s\n", gai_strerror(status));
        return NULL;
    }

    for (p = res; p != NULL; p = p->ai_next) 
    {
        void *addr;

        // get the pointer to the address itself,
        // different fields in IPv4 and IPv6:
        struct sockaddr_in *ipv4 = (struct sockaddr_in *) p->ai_addr;
        addr = &(ipv4->sin_addr);

        // convert the IP to a string and print it:
        inet_ntop(p->ai_family, addr, ipstr, sizeof ipstr);
        printf("%s\n", ipstr);
//        WriteLog(logfile, "Resolved IP address: ");
//        WriteLogLF(logfile, ipstr);
        break;
    }
    
    // get addr from command line and convert it
    if (inet_pton(AF_INET, ipstr, ipstr) != 1) 
    {
        perror("Cannot get addr from command line and convert it");
        //~ close(sockfd);
        exit(EXIT_FAILURE);
    }
    
    freeaddrinfo(res);
    
    return ipstr;
}

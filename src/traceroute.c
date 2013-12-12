/**\file traceroute.c
 * \brief traceroute functions
 * \date December 10, 2013, 10:37 AM
 */

#include "../include/traceroute.h"
#include "../include/log.h"

#define PACKET_LENGTH 8192

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

void SetIPHeaderTTL(struct iphdr* iph, int ttl)
{
	iph->ttl = ttl;
}

Socket OpenRawSocket(char protocol)
{
    Socket s;
    int p;
    
    switch(protocol) {
		case 'I':
		case 'i':
			p = IPPROTO_ICMP;
			break;
		case 'U':
		case 'u':
			p = IPPROTO_UDP;
			break;
		case 'T':
		case 't':
		default:
			p = IPPROTO_TCP;
	}
	
    if ((s = socket(PF_INET, SOCK_RAW, p)) == -1)
    {
        perror("Unable to open Raw Socket");
        exit(1);
    }
    return s;
}

void ConstructIPHeader(struct iphdr* iph, 
        const unsigned int ttl, 
        const char *source,
        const char *dest,
        const char protocol)
{
    iph->ihl = 5;
    iph->version = 4;
    iph->tos = 16; // Low delay
    iph->id = htons(54321);
    iph->ttl = ttl; // hops
    
    switch(protocol) {
		case 'I':
		case 'i':
			iph->protocol = 1; // ICMP
			break;
		case 'U':
		case 'u':
			iph->protocol = 17; // UDP
			break;
		case 'T':
		case 't':
		default:
			iph->protocol = 6; // TCP
			break;
	}
	
	iph->tot_len = sizeof(struct iphdr)+sizeof(struct udphdr);
    /* Source IP address, can be spoofed */
    iph->saddr = inet_addr(source);
    /* Destination IP address */
    iph->daddr = inet_addr(dest);
    
    //~ return sizeof(*iph);
}

void ConstructUDPHeader(struct udphdr* udph)
{
	udph->source = htons(3423);
	udph->dest = htons(5342);
	udph->len = sizeof(struct udphdr);
	udph->check = 0; // skip
}

void ConstructUDPPacket(PacketUDP* buffer, const char* source, const char* dest)
{
    ConstructIPHeader(&(buffer->iph), 64, source, dest, 'U');
    ConstructUDPHeader(&(buffer->udph));
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
		//~ WriteLog(logfile, "Resolved IP address: ");
		//~ WriteLogLF(logfile, ipstr);
        break;
    }
    
    // get addr from command line and convert it
    if (inet_pton(AF_INET, ipstr, ipstr) != 1) 
    {
        perror("Cannot get addr from command line and convert it");
        exit(EXIT_FAILURE);
    }
    
    freeaddrinfo(res);
    
    return ipstr;
}

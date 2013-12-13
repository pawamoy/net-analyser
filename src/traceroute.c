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

    if (setsockopt(s, IPPROTO_IP, IP_TTL, ttl_p, sizeof (*ttl_p)) == -1)
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

    switch (protocol)
    {
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

    switch (protocol)
    {
        case 'I':
        case 'i':
            iph->protocol = 1; // ICMP
            iph->tot_len = sizeof (struct icmphdr);
            break;
        case 'U':
        case 'u':
            iph->protocol = 17; // UDP
            iph->tot_len = sizeof (struct udphdr);
            break;
        case 'T':
        case 't':
        default:
            iph->protocol = 6; // TCP
            iph->tot_len = sizeof (struct tcphdr);
            break;
    }

    iph->tot_len += sizeof (struct iphdr);
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
    udph->len = sizeof (struct udphdr);
    udph->check = 0; // skip
}

void ConstructUDPPacket(PacketUDP* buffer, const char* source, const char* dest)
{
    ConstructIPHeader(&(buffer->iph), 64, source, dest, 'U');
    ConstructUDPHeader(&(buffer->udph));
}

char *GetIPFromHostname(const char *hostname)
{
    struct addrinfo hints, *p, *pbak, *res;
	struct sockaddr_in *ipv4;
    void *addr;
    char *ipstr = malloc(sizeof (char)*INET6_ADDRSTRLEN);
    int status, ret;

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
        // get the pointer to the address itself,
        // different fields in IPv4 and IPv6:
        ipv4 = (struct sockaddr_in *) p->ai_addr;
        addr = &(ipv4->sin_addr);
        pbak = p;
	}
	
	// convert the IP to a string and print it:
	inet_ntop(pbak->ai_family, addr, ipstr, sizeof (char)*INET6_ADDRSTRLEN);
	//~ printf("%s\n", ipstr);

    // get addr from command line and convert it
    if ((ret = inet_pton(AF_INET, ipstr, ipstr)) != 1)
    {
		switch(ret)
		{
			case 0 :
				fprintf(stderr, "inet_pton(): unvalid IPv4 address\n");
				break;
			case -1: 
			default:
				fprintf(stderr, "inet_pton(): unvalid address family\n");
				break;
		}
        
        perror("Cannot get addr from command line and convert it");
        exit(EXIT_FAILURE);
    }

    freeaddrinfo(res);

    return ipstr;
}
 
//~ int hostname_to_ip(char * hostname , char* ip)
//~ {
    //~ struct hostent *he;
    //~ struct in_addr **addr_list;
    //~ int i;
         //~ 
    //~ if ( (he = gethostbyname( hostname ) ) == NULL)
    //~ {
        //~ // get the host info
        //~ herror("gethostbyname");
        //~ return 1;
    //~ }
 //~ 
    //~ addr_list = (struct in_addr **) he->h_addr_list;
     //~ 
    //~ for(i = 0; addr_list[i] != NULL; i++)
    //~ {
        //~ //Return the first one;
        //~ strcpy(ip , inet_ntoa(*addr_list[i]) );
        //~ return 0;
    //~ }
     //~ 
    //~ return 1;
//~ }

char* GetMyIP()
{
	struct ifaddrs *ifaddr, *ifa;
	int family, s;
	char* host = (char*)malloc(128*sizeof(char));

	if (getifaddrs(&ifaddr) == -1) {
	   perror("getifaddrs");
	   exit(EXIT_FAILURE);
	}

	/* Walk through linked list, maintaining head pointer so we
	  can free list later */

	for (ifa = ifaddr; ifa != NULL; ifa = ifa->ifa_next) {
	   if (ifa->ifa_addr == NULL)
		   continue;

	   family = ifa->ifa_addr->sa_family;

	   /* Display interface name and family (including symbolic
		  form of the latter for the common families) */

	   //~ printf("%s  address family: %d%s\n",
			   //~ ifa->ifa_name, family,
			   //~ (family == AF_PACKET) ? " (AF_PACKET)" :
			   //~ (family == AF_INET) ?   " (AF_INET)" :
			   //~ (family == AF_INET6) ?  " (AF_INET6)" : "");

	   /* For an AF_INET* interface address, display the address */

	   if (family == AF_INET || family == AF_INET6) {
		   s = getnameinfo(ifa->ifa_addr,
				   (family == AF_INET) ? sizeof(struct sockaddr_in) :
										 sizeof(struct sockaddr_in6),
				   host, NI_MAXHOST, NULL, 0, NI_NUMERICHOST);
		   if (s != 0) {
			   printf("getnameinfo() failed: %s\n", gai_strerror(s));
			   exit(EXIT_FAILURE);
		   }
		   if (IsMyAddress(host) == 1)
		   {
				break;
		   }
		}
	}

	freeifaddrs(ifaddr);
	return host;
}

int IsMyAddress(char* addr)
{
	if (strcmp(addr, "127.0.0.1")==0)
	{
		return 0;
	}
	
	if (isdigit(addr[0])
		&& isdigit(addr[1])
		&& isdigit(addr[2])
		&& addr[3] == '.')
	{
		return 1;
	}
	
	return 0;
}

//~ void DecodeICMPHeader(char *buf, int bytes, struct sockaddr_in *from)
//~ {
    //~ struct iphdr   *iphdr = NULL;
    //~ struct icmphdr *icmphdr = NULL;
    //~ unsigned short  iphdrlen;
    //~ int             tick;
    //~ static   int    icmpcount = 0;
    //~ iphdr = (struct iphdr *)buf;
//~ // Number of 32-bit words * 4 = bytes
    //~ iphdrlen = iphdr->h_len * 4;
    //~ tick = GetTickCount();
    //~ if ((iphdrlen == MAX_IP_HDR_SIZE) && (!icmpcount))
        //~ DecodeIPOptions(buf, bytes);
    //~ if (bytes  < iphdrlen + ICMP_MIN)
    //~ {
        //~ printf("Too few bytes from %s\n",
            //~ inet_ntoa(from->sin_addr));
    //~ }
    //~ icmphdr = (IcmpHeader*)(buf + iphdrlen);
    //~ if (icmphdr->i_type != ICMP_ECHOREPLY)
    //~ {
        //~ printf("nonecho type %d recvd\n", icmphdr->i_type);
        //~ return;
    //~ }
    //~ // Make sure this is an ICMP reply to something we sent!
    //~ //
    //~ if (icmphdr->i_id != (USHORT)GetCurrentProcessId())
    //~ {
        //~ printf("someone else's packet!\n" );
        //~ return ;
    //~ }
    //~ printf("%d bytes from %s:", bytes, inet_ntoa(from->sin_addr));
    //~ printf(" icmp_seq = %d. ", icmphdr->i_seq);
    //~ printf(" time: %d ms", tick - icmphdr->timestamp);
    //~ printf("\n" );
    //~ icmpcount++;
    //~ return;
//~ }





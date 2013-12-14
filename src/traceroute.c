/**\file traceroute.c
 * \brief traceroute functions
 * \date December 10, 2013, 10:37 AM
 */

#include "../include/traceroute.h"
#include "../include/log.h"

void Usage()
{
    printf("USAGE: traceroute servername [-h HOPS] [-n MIN_TTL] [-m MAX_TTL] [-r SEC] [-s SEC]\n");
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

int SetRCVTimeOut(Socket s, struct timeval to)
{
	if (setsockopt(s, SOL_SOCKET, SO_RCVTIMEO, (const void*) &to, sizeof(to)) == -1)
    {
		perror("setsockopt receive timeout");
		return 0;
	}
	
	return 1;
}

int SetSNDTimeOut(Socket s, struct timeval to)
{
	if (setsockopt(s, SOL_SOCKET, SO_SNDTIMEO, (const void*) &to, sizeof(to)) == -1)
    {
		perror("setsockopt receive timeout");
		return 0;
	}
	
	return 1;
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

    if ((s = socket(AF_INET, SOCK_RAW, p)) == -1)
    {
        perror("Unable to open Raw Socket");
        exit(1);
    }
    return s;
}

Socket OpenDgramSocket(char protocol)
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

    if ((s = socket(AF_INET, SOCK_DGRAM, p)) == -1)
    {
        perror("Unable to open dgram Socket");
        exit(1);
    }
    return s;
}
/*
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
    // Source IP address, can be spoofed
    iph->saddr = inet_addr(source);
    // Destination IP address
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
    int status;
    //~ int ret;

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
    //~ if ((ret = inet_pton(AF_INET, ipstr, ipstr)) != 1)
    //~ {
		//~ switch(ret)
		//~ {
			//~ case 0 :
				//~ fprintf(stderr, "inet_pton(): unvalid IPv4 address\n");
				//~ break;
			//~ case -1: 
			//~ default:
				//~ fprintf(stderr, "inet_pton(): unvalid address family\n");
				//~ break;
		//~ }
        //~ 
        //~ perror("Cannot get addr from command line and convert it");
        //~ exit(EXIT_FAILURE);
    //~ }

    freeaddrinfo(res);

    return ipstr;
}
* */
 
char* GetIPFromHostname(const char* hostname)
{
    struct hostent *he;
    struct in_addr **addr_list;
    int i;
         
    if ( (he = gethostbyname( hostname ) ) == NULL)
    {
        // get the host info
        herror("gethostbyname()");
        exit(-1);
    }
 
    addr_list = (struct in_addr **) he->h_addr_list;
     
    for(i = 0; addr_list[i] != NULL; i++)
    {
        //Return the first one;
        return inet_ntoa(*addr_list[i]);
    }
     
    exit(-1);
}

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
	if (strcmp(addr, "127.0.0.1") == 0)
	{
		return 0;
	}
	
	if (strlen(addr) >= 8)
	{
		if (isdigit(addr[0]) &&
			isdigit(addr[1]) &&
			isdigit(addr[2]) &&
			addr[3] == '.')
		{
			return 1;
		}
	}
	
	return 0;
}

char* GetHostNameFromIP(const char* ip)
{
	struct hostent *hent;
	struct in_addr addr;
	char* host = (char*)malloc(128*sizeof(char));
	strcpy(host, ip);

	if (!inet_aton(ip, &addr))
		return host;

	if ((hent = gethostbyaddr((char *)&(addr.s_addr), sizeof(addr.s_addr), AF_INET)))
		strcpy(host, hent->h_name);

	return host;
}

void LoopUDP(int rcvt, int sndt, int ttl_t[3], FILE* logfile,
             struct sockaddr_in server, struct sockaddr_in my_addr)
{
	struct timeval r_timeout = { rcvt, 0 };
    struct timeval s_timeout = { sndt, 0 };
    
    Socket send_socket, receive_socket;
    socklen_t addrlen = sizeof (struct sockaddr_in);
    
    char rsaddr[MAX_ADDRESS], recvbuf[MAX_PACKET];
    char* host = NULL;
    
    struct iphdr* iph = NULL;
    
    int ttl, min_ttl = ttl_t[0], max_ttl = ttl_t[1], hops = ttl_t[2];

    for (ttl = min_ttl; ttl <= max_ttl; ttl += hops)
    {
		send_socket    = OpenDgramSocket('U');
		receive_socket = OpenRawSocket('I');
		
		if (bind(receive_socket, (struct sockaddr*)&my_addr, addrlen) == -1)
		{
			perror("bind receive socket");
			exit(-1);
		}
		
        if ( ! SetTTL(send_socket, ttl))			     exit(-1);
		if ( ! SetSNDTimeOut(send_socket, s_timeout))    exit(-1);
		if ( ! SetRCVTimeOut(receive_socket, r_timeout)) exit(-1);

        if (sendto(send_socket, "", 0, 0, (struct sockaddr*) &server, addrlen) == -1)
        {
            perror(" sendto()");
		}
		else
		{
			if (recvfrom(receive_socket, recvbuf, MAX_PACKET, 0, NULL, NULL) == -1)
			{
				printf(" %-2d %-15s *\n", ttl, "*");
				if (logfile != NULL)
				{
					fprintf(logfile, " %-2d %-15s *\n", ttl, "*");
				}
			}
			else
			{
				iph = (struct iphdr*) recvbuf;
				inet_ntop(AF_INET, &(iph->saddr), rsaddr, MAX_ADDRESS);
				host = GetHostNameFromIP(rsaddr);
				printf(" %-2d %-15s %s\n", ttl, rsaddr, host);
				if (logfile != NULL)
				{
					fprintf(logfile, " %-2d %-15s %s\n", ttl, rsaddr, host);
				}
			}
		}
		
		close(send_socket);
		close(receive_socket);
    }
}


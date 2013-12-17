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

int SetHDRINCL(Socket s)
{
	int tmp = 1;
	int* val = &tmp;
	if (setsockopt (s, IPPROTO_IP, IP_HDRINCL, val, sizeof(tmp)) < 0)
	{
		perror("setsockopt HDRINCL");
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
        case 'i':
            p = IPPROTO_ICMP;
            break;
        case 'u':
            p = IPPROTO_UDP;
            break;
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
        case 'i':
            p = IPPROTO_ICMP;
            break;
        case 'u':
            p = IPPROTO_UDP;
            break;
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
        case 'i':
            iph->protocol = 1; // ICMP
            iph->tot_len = sizeof (struct icmphdr);
            break;
        case 'u':
            iph->protocol = 17; // UDP
            iph->tot_len = sizeof (struct udphdr);
            break;
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

void ConstructICMPHeader(struct icmphdr* icmph, int seq)
{
	memset(icmph, 0, sizeof(struct icmphdr));
    icmph->type = ICMP_ECHO;
    icmph->un.echo.id = getpid();
    icmph->un.echo.sequence = seq;
    icmph->code = 0;
    //~ icmph->checksum = 0; // skip
}

void ConstructTCPHeader(struct tcphdr *tcph) 
{
    //TCP Header
    tcph->source = htons (1234);
    tcph->dest = htons (80);
    tcph->seq = 0;
    tcph->ack_seq = 0;
    tcph->doff = 5;      /* first and only tcp segment */
    tcph->fin=0;
    tcph->syn=1;
    tcph->rst=0;
    tcph->psh=0;
    tcph->ack=0;
    tcph->urg=0;
    tcph->window = htons (5840); /* maximum allowed window size */
    tcph->check = 0;/* if you set a checksum to zero, your kernel's IP stack
                should fill in the correct checksum during transmission */
    tcph->urg_ptr = 0;
}
 
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
    
    struct sockaddr_in recept = { 0 };
    
    char recvbuf[MAX_PACKET];
    char *host = NULL, *rsaddr = NULL;
    
    char dest[MAX_ADDRESS];
    strcpy(dest, inet_ntoa(server.sin_addr));
    
    int reach_dest = 0;
    
    //~ struct iphdr* iph = NULL;
    //~ struct icmphdr* icmph = NULL;
    
    int tent, tentative = 3;
    
    int ttl, min_ttl = ttl_t[0], max_ttl = ttl_t[1], hops = ttl_t[2];

    for (ttl = min_ttl; ttl <= max_ttl; ttl += hops)
    {
		send_socket    = OpenDgramSocket('u');
		receive_socket = OpenRawSocket('i');
		
		if (bind(receive_socket, (struct sockaddr*)&my_addr, addrlen) == -1)
		{
			perror("bind receive socket");
			exit(-1);
		}
		
        if ( ! SetTTL(send_socket, ttl))			     exit(-1);
		if ( ! SetSNDTimeOut(send_socket, s_timeout))    exit(-1);
		if ( ! SetRCVTimeOut(receive_socket, r_timeout)) exit(-1);

        if (sendto(send_socket, "hello", 4, 0, (struct sockaddr*) &server, addrlen) == -1)
        {
            perror("sendto()");
		}
		else
		{
			printf(" %-2d ", ttl);
			for (tent = 0; tent < tentative; tent++)
			{
				if (recvfrom(receive_socket, recvbuf, MAX_PACKET, 0, (struct sockaddr*)&recept, &addrlen) == -1)
				{
					//~ icmph = (struct icmphdr*) (recvbuf + sizeof(struct iphdr));
					//~ printf("type=%d\ncode=%d\n", icmph->type, icmph->code);
					printf("* ");
					if (logfile != NULL)
					{
						fprintf(logfile, " %-2d %-15s *\n", ttl, "*");
					}
				}
				else
				{
					// way 1
					rsaddr = inet_ntoa(recept.sin_addr);
					// way 2
					//~ iph = (struct iphdr*) recvbuf;
					//~ icmph = (struct icmphdr*) (recvbuf + sizeof(struct iphdr));
					//~ if (icmph->type != ICMP_TIME_EXCEEDED)
					//~ {
						//~ printf(" Reach destination\n");
					//~ }
					//~ printf("type=%d\ncode=%d\n", icmph->type, icmph->code);
					//~ inet_ntop(AF_INET, &(iph->saddr), rsaddr, MAX_ADDRESS);
					// end way
					host = GetHostNameFromIP(rsaddr);
					printf("%-15s %s", rsaddr, host);
					if (logfile != NULL)
					{
						fprintf(logfile, " %-2d %-15s %s\n", ttl, rsaddr, host);
					}
					if (strcmp(dest, rsaddr)==0)
					{
						reach_dest = 1;
					}
					break;
				}
			}
			printf("\n");
		}
		
		close(send_socket);
		close(receive_socket);
		
		if (reach_dest) return;
    }
}

void LoopICMP(int rcvt, int sndt, int ttl_t[3], FILE* logfile,
             struct sockaddr_in server, struct sockaddr_in my_addr)
{
	struct timeval r_timeout = { rcvt, 0 };
    struct timeval s_timeout = { sndt, 0 };
    
    Socket send_socket, receive_socket;
    socklen_t addrlen = sizeof (struct sockaddr_in);
    
    struct sockaddr_in recept = { 0 };
    
    char recvbuf[MAX_PACKET] = { 0 };
    char pack_icmp[MAX_PACKET] = { 0 };
    char *host = NULL, *rsaddr = NULL;
    
    char dest[MAX_ADDRESS];
    strcpy(dest, inet_ntoa(server.sin_addr));
    char source[MAX_ADDRESS];
    strcpy(source, inet_ntoa(my_addr.sin_addr));
    
    int seq = 0;
    int reach_dest = 0;
    int received = 0;
    int tent, tentative = 3;
    int ip_icmp_len = sizeof(struct iphdr) + sizeof(struct icmphdr);
    int ttl, min_ttl = ttl_t[0], max_ttl = ttl_t[1], hops = ttl_t[2];
	
    //~ struct iphdr* iph = NULL;
    struct icmphdr* icmph = NULL;      
    
    for (ttl = min_ttl; ttl <= max_ttl; ttl += hops, seq++)
    {
		printf(" %-2d ", ttl);
		for (tent = 0; tent < tentative; tent++)
		{
			send_socket    = OpenRawSocket('i');
			receive_socket = OpenRawSocket('i');
			
			if (bind(receive_socket, (struct sockaddr*)&my_addr, addrlen) == -1)
			{
				perror("bind receive socket");
				exit(-1);
			}
			
			ConstructIPHeader((struct iphdr*)pack_icmp, ttl, source, dest, 'i');
			ConstructICMPHeader((struct icmphdr*)(pack_icmp+sizeof(struct iphdr)), seq);
			
			if ( ! SetHDRINCL(send_socket))                  exit(-1);
			if ( ! SetSNDTimeOut(send_socket, s_timeout))    exit(-1);
			if ( ! SetRCVTimeOut(receive_socket, r_timeout)) exit(-1);

			if (sendto(send_socket, pack_icmp, ip_icmp_len, 0, (struct sockaddr*) &server, addrlen) == -1)
			{
				perror("sendto()");
			}
			else
			{
				received = 0;
				if (recvfrom(receive_socket, recvbuf, MAX_PACKET, 0, (struct sockaddr*)&recept, &addrlen) == -1)
				{
					icmph = (struct icmphdr*) (recvbuf + sizeof(struct iphdr));
					if (icmph->type == ICMP_TIME_EXCEEDED)
						printf("* ");
					if (logfile != NULL)
					{
						fprintf(logfile, " %-2d %-15s *\n", ttl, "*");
					}
				}
				else
				{
					received = 1;
					rsaddr = inet_ntoa(recept.sin_addr);
					host = GetHostNameFromIP(rsaddr);
					printf("%-15s %s", rsaddr, host);
					if (logfile != NULL)
					{
						fprintf(logfile, " %-2d %-15s %s\n", ttl, rsaddr, host);
					}
					if (strcmp(dest, rsaddr)==0)
					{
						reach_dest = 1;
					}
				}
			}
			
			close(send_socket);
			close(receive_socket);
			
			if (received == 1)
				break;
		}
		printf("\n");
		
		if (reach_dest) return;
    }
}

void LoopTCP(int rcvt, int sndt, int ttl_t[3], FILE* logfile,
             struct sockaddr_in server, struct sockaddr_in my_addr)
{
	struct timeval r_timeout = { rcvt, 0 };
    struct timeval s_timeout = { sndt, 0 };
    
    Socket send_socket, receive_socket;
    socklen_t addrlen = sizeof (struct sockaddr_in);
    
    struct sockaddr_in recept = { 0 };
    
    char recvbuf[MAX_PACKET];
    char *host = NULL, *rsaddr = NULL;
    
    char dest[MAX_ADDRESS];
    strcpy(dest, inet_ntoa(server.sin_addr));
    char source[MAX_ADDRESS];
    strcpy(source, inet_ntoa(my_addr.sin_addr));
    
    int reach_dest = 0;
    int tent, tentative = 3;
    int ttl, min_ttl = ttl_t[0], max_ttl = ttl_t[1], hops = ttl_t[2];
	
    PacketTCP pack_tcp;
    // *recv_pack;      
    
    for (ttl = min_ttl; ttl <= max_ttl; ttl += hops)
    {
		send_socket    = OpenRawSocket('t');
		receive_socket = OpenRawSocket('t');
		
		if (bind(receive_socket, (struct sockaddr*)&my_addr, addrlen) == -1)
		{
			perror("bind receive socket");
			exit(-1);
		}
		
		ConstructIPHeader(&(pack_tcp.iph), ttl, source, dest, 't');
		ConstructTCPHeader(&(pack_tcp.tcph));
		
		if ( ! SetSNDTimeOut(send_socket, s_timeout))    exit(-1);
		if ( ! SetRCVTimeOut(receive_socket, r_timeout)) exit(-1);

        if (sendto(send_socket, &pack_tcp, sizeof(pack_tcp), 0, (struct sockaddr*) &server, addrlen) == -1)
        {
            perror("sendto()");
		}
		else
		{
			printf(" %-2d ", ttl);
			for (tent = 0; tent < tentative; tent++)
			{
				if (recvfrom(receive_socket, recvbuf, MAX_PACKET, 0, (struct sockaddr*)&recept, &addrlen) == -1)
				{
					printf("* ");
					//recv_pack = (PacketTCP*) recvbuf;
					//printf("type=%d\ncode=%d\n", recv_pack->icmph.type, recv_pack->icmph.code);
					if (logfile != NULL)
					{
						fprintf(logfile, " %-2d %-15s *\n", ttl, "*");
					}
				}
				else
				{
					// way 1
					rsaddr = inet_ntoa(recept.sin_addr);
                    /*
					// way 2
					recv_pack = (PacketTCP*) recvbuf;
					if (recv_pack->icmph.type != ICMP_TIME_EXCEEDED)
					{
						printf(" Reach destination\n");
					}
					printf("type=%d\ncode=%d\n", recv_pack->icmph.type, recv_pack->icmph.code);
					//~ inet_ntop(AF_INET, &(iph->saddr), rsaddr, MAX_ADDRESS);
                    * */
					// end way
					host = GetHostNameFromIP(rsaddr);
					printf("%-15s %s", rsaddr, host);
					if (logfile != NULL)
					{
						fprintf(logfile, " %-2d %-15s %s\n", ttl, rsaddr, host);
					}
					if (strcmp(dest, rsaddr)==0)
					{
						reach_dest = 1;
					}
					break;
				}
			}
			printf("\n");
		}
		
		close(send_socket);
		close(receive_socket);
		
		if (reach_dest) return;
    }
}


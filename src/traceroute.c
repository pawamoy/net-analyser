/**\file traceroute.c
 * \brief traceroute functions
 */

#include "../include/traceroute.h"
#include "../include/log.h"

void UsageTraceroute()
{
    fprintf(stderr, "usage: traceroute servername [-I | -U | -T] [-p PORT] [-f FIRST_TTL] [-m MAX_TTL] [-h HOPS] [-r SEC] [-s SEC] [-a ATTEMPTS]\n");
    exit(-1);
}

StrTrace NewTrace()
{
	StrTrace s = {NULL,'i',3,3,1,30,1,3};
	return s;
}

StrTraceRoute NewTraceRoute()
{
	StrTraceRoute t = {NULL,NULL,NULL,80,NewTrace()};
	return t;
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

    if ((s = socket(PF_INET, SOCK_RAW, p)) == -1)
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
            //~ iph->tot_len = sizeof (struct icmphdr);
            break;
        case 'u':
            iph->protocol = 17; // UDP
            //~ iph->tot_len = sizeof (struct udphdr);
            break;
        case 't':
        default:
            iph->protocol = 6; // TCP
            //~ iph->tot_len = sizeof (struct tcphdr);
            break;
    }

    iph->tot_len = sizeof (struct iphdr);
    // Source IP address, can be spoofed
    iph->saddr = inet_addr(source);
    // Destination IP address
    iph->daddr = inet_addr(dest);
}

void ConstructICMPHeader(struct icmphdr* icmph)
{
	memset(icmph, 0, sizeof(struct icmphdr));
    icmph->type = ICMP_ECHO;
    icmph->code = 0;
    icmph->checksum = htons(~(ICMP_ECHO << 8));
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
 
int LoopTrace(StrTrace s, Sockin server, Sockin my_addr)
{
	//-----------------------------------------------------//
	// variable declaration
	//-----------------------------------------------------//
	struct timeval r_timeout = { s.rcvt, 0 },
                   s_timeout = { s.sndt, 0 };
    
    Socket send_socket,
           receive_socket;
           
    socklen_t addrlen = sizeof(Sockin);
    
    Sockin recept = { 0 };
    
    char recvbuf[MAX_PACKET] = { 0 },
         packet[MAX_PACKET]  = { 0 },
        *host                = NULL,
        *rsaddr              = NULL;
    
    char dest[MAX_ADDRESS],
         source[MAX_ADDRESS];
         
    strcpy(dest,   inet_ntoa(server.sin_addr));
    strcpy(source, inet_ntoa(my_addr.sin_addr));
    
    int reach_dest = 0,
        received   = 0,
        att        = 0,
        ttl        = 0,
        bytes      = 0,
        cur_pad    = 0,
        padding    = 19;
    
    //-----------------------------------------------------//
	// starting loop
	//-----------------------------------------------------//
    for (ttl = s.min_ttl; ttl <= s.max_ttl; ttl += s.hops)
    {
		printf("%2d  ", ttl);
		fflush(stdout);
		
		if (s.logfile != NULL)
			fprintf(s.logfile, "%2d  ", ttl);
		
		cur_pad = padding;
		
		for (att = 0; att < s.attempts; att++)
		{
			switch (s.probe)
			{
				case 'u':
					bytes = UDP_LEN;
					send_socket = OpenDgramSocket('u');
					receive_socket = OpenRawSocket('i');
					break;
				case 'i':
					bytes = ICMP_LEN;
					send_socket = OpenRawSocket('i');
					receive_socket = OpenRawSocket('i');
					break;
				case 't':
					bytes = TCP_LEN;
					send_socket = OpenRawSocket('t');
					receive_socket = OpenRawSocket('t');
					break;
			}
			
			if (bind(receive_socket, (struct sockaddr*)&my_addr, addrlen) == -1)
			{
				perror("bind receive socket");
				exit(-1);
			}
			
			switch (s.probe)
			{
				case 'u':
					if ( ! SetTTL(send_socket, ttl))
						exit(-1);
					break;
				case 'i':
					ConstructIPHeader((struct iphdr*)packet, ttl, source, dest, 'i');
					ConstructICMPHeader((struct icmphdr*)(packet+sizeof(struct iphdr)));
					if ( ! SetHDRINCL(send_socket))
						exit(-1);
					break;
				case 't':
					ConstructIPHeader((struct iphdr*)packet, ttl, source, dest, 't');
					ConstructTCPHeader((struct tcphdr*)(packet+sizeof(struct iphdr)));
					if ( ! SetHDRINCL(send_socket))
						exit(-1);
					break;
			}
					
			if ( ! SetSNDTimeOut(send_socket, s_timeout))    exit(-1);
			if ( ! SetRCVTimeOut(receive_socket, r_timeout)) exit(-1);

			if (sendto(send_socket, packet, bytes, 0, (struct sockaddr*) &server, addrlen) == -1)
				perror("sendto()");

			else
			{
				received = 0;
				if (recvfrom(receive_socket, recvbuf, MAX_PACKET, 0, (struct sockaddr*)&recept, &addrlen) == -1)
				{
					printf("* ");
					fflush(stdout);
					
					if (s.logfile != NULL)
						fprintf(s.logfile, "* ");
					
					cur_pad -= 2;
				}
				else
				{
					received = 1;
					rsaddr = inet_ntoa(recept.sin_addr);
					host = GetHostNameFromIP(rsaddr);
					printf("%-*s %s", cur_pad, rsaddr, host);
					fflush(stdout);
					
					if (s.logfile != NULL)
						fprintf(s.logfile, "%-*s %s", cur_pad, rsaddr, host);

					if (strcmp(dest, rsaddr)==0)
						reach_dest = 1;
				}
			}
			
			close(send_socket);
			close(receive_socket);
			
			if (received == 1)
				break;
		}
		
		printf("\n");
		if (s.logfile != NULL)
			fprintf(s.logfile, "\n");
			
		if (reach_dest) return ttl;
	}
	
	return -1;
}

int main_traceroute(StrTraceRoute tr)
{
	//-----------------------------------------------------//
	// variable declaration
	//-----------------------------------------------------//
    Sockin server,
           my_addr;
    int    domain   = AF_INET,
           bytes    = 0;


	//-----------------------------------------------------//
	// get packets length
	//-----------------------------------------------------//
	switch (tr.s.probe) {
		case 'u': bytes = UDP_LEN;  break;
		case 'i': bytes = ICMP_LEN;	break;
		case 't': bytes = TCP_LEN;	break; 
		default :                   break;
	}


	//-----------------------------------------------------//
	// first information outputs
	//-----------------------------------------------------//
    printf("traceroute to %s (%s), %d hops max, %d byte packets\n",
		tr.address, tr.ipstr, tr.s.max_ttl, bytes);
	
	//-----------------------------------------------------//
	// initializing some data
	//-----------------------------------------------------//
	
    // init remote addr structure and other params
    server.sin_family = domain;
    server.sin_port = htons(tr.portno);
    inet_aton(tr.ipstr, &(server.sin_addr));

	// init local addr structure
    my_addr.sin_family = domain;
    my_addr.sin_port = htons(tr.portno);
    inet_aton(tr.myip, &(my_addr.sin_addr));
    
    
    //-----------------------------------------------------//
	// starting traceroute
	//-----------------------------------------------------//
	return LoopTrace(tr.s, server, my_addr);
}

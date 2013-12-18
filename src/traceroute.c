/**\file traceroute.c
 * \brief traceroute functions
 * \date December 10, 2013, 10:37 AM
 */

#include "../include/traceroute.h"
#include "../include/log.h"

void UsageTraceroute()
{
    printf("USAGE: traceroute servername [-b icmp|udp|tcp] [-p PORT] [-n MIN_TTL] [-m MAX_TTL] [-h HOPS] [-r SEC] [-s SEC]\n");
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
 
void LoopTrace(int rcvt, int sndt, int ttl_t[4], FILE* logfile, char probe,
             struct sockaddr_in server, struct sockaddr_in my_addr)
{
	struct timeval r_timeout = { rcvt, 0 };
    struct timeval s_timeout = { sndt, 0 };
    
    Socket send_socket, receive_socket;
    socklen_t addrlen = sizeof (struct sockaddr_in);
    
    struct sockaddr_in recept = { 0 };
    
    char recvbuf[MAX_PACKET];
    char packet[MAX_PACKET] = { 0 };
    char *host = NULL, *rsaddr = NULL;
    
    char dest[MAX_ADDRESS];
    strcpy(dest, inet_ntoa(server.sin_addr));
    char source[MAX_ADDRESS];
    strcpy(source, inet_ntoa(my_addr.sin_addr));
    
    int reach_dest = 0;
    int received = 0;
    int att, attempt = ttl_t[3];
    int ttl, min_ttl = ttl_t[0], max_ttl = ttl_t[1], hops = ttl_t[2];
    int bytes = 0;
    
    for (ttl = min_ttl; ttl <= max_ttl; ttl += hops)
    {
		printf("%2d  ", ttl);
		fflush(stdout);
		for (att = 0; att < attempt; att++)
		{
			switch (probe)
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
			
			switch (probe)
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
					
					if (logfile != NULL)
						fprintf(logfile, " %-2d %-15s *\n", ttl, "*");

				}
				else
				{
					received = 1;
					rsaddr = inet_ntoa(recept.sin_addr);
					host = GetHostNameFromIP(rsaddr);
					printf("%-15s %s", rsaddr, host);
					fflush(stdout);
					
					if (logfile != NULL)
						fprintf(logfile, " %-2d %-15s %s\n", ttl, rsaddr, host);

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
		if (reach_dest) return;
	}
}

int main_traceroute(int argc, char* argv[])
{
	//-----------------------------------------------------//
	// variable declaration
	//-----------------------------------------------------//
    struct sockaddr_in server,
                       my_addr;
    int                domain       = AF_INET,
                       portno       = 80,
                       min_ttl      = 1,
                       max_ttl      = 30,
                       hops         = 1,
					   rcv_timeout  = 3,
					   snd_timeout  = 3,
                       attempt      = 3,
                       i            = 0,
                       bytes        = 0,
                       log_data     = 0;
    char              *ipstr        = NULL,
                      *myip         = NULL,
                      probe         = 'i';
    FILE              *logfile      = NULL;
    //~ uid_t              uid;


	//-----------------------------------------------------//
	// first verifications
	//-----------------------------------------------------//
	
    // check the number of args on command line
    if (argc < 2) UsageTraceroute();

    // check root privileges
    //~ uid = getuid();
    //~ setuid(uid);
    if (getuid()) {
        fprintf(stderr, "\nError: you must be root to use raw sockets\n");
        exit(-1);
    }
    
    // argument analysis
    for (i=2; i<argc; i++)
    {
		     if (strcmp(argv[i], "-m") == 0 ||
		         strcmp(argv[i], "--maxttl") == 0) {
					if (i+1<argc) {
						max_ttl = atoi(argv[i+1]); i++;
					} else {
						fprintf(stderr, "-m: missing value: INT>0\n");
						exit(-1);
					}
				}
		else if (strcmp(argv[i], "-n") == 0 ||
		         strcmp(argv[i], "--minttl") == 0) {
					if (i+1<argc) {
						min_ttl = atoi(argv[i+1]); i++;
					} else {
						fprintf(stderr, "-n: missing value: INT>0\n");
						exit(-1);
					}
				}
		else if (strcmp(argv[i], "-h") == 0 ||
		         strcmp(argv[i], "--hops") == 0) {
					if (i+1<argc) {
						hops = atoi(argv[i+1]); i++;
					} else {
						fprintf(stderr, "-h: missing value: INT>0\n");
						exit(-1);
					}
				}
		else if (strcmp(argv[i], "-r") == 0 ||
		         strcmp(argv[i], "--recv-timeout") == 0) {
					if (i+1<argc) {
						rcv_timeout = atoi(argv[i+1]); i++;
					} else {
						fprintf(stderr, "-r: missing value: INT>0\n");
						exit(-1);
					}
				}
		else if (strcmp(argv[i], "-s") == 0 ||
		         strcmp(argv[i], "--send-timeout") == 0) {
					if (i+1<argc) {
						snd_timeout = atoi(argv[i+1]); i++;
					} else {
						fprintf(stderr, "-s: missing value: INT>0\n");
						exit(-1);
					}
				}
		else if (strcmp(argv[i], "-l") == 0 ||
		         strcmp(argv[i], "--log") == 0) log_data = 1;
		else if (strcmp(argv[i], "-p") == 0 ||
		         strcmp(argv[i], "--port") == 0) {
					if (i+1<argc) {
						portno = atoi(argv[i+1]); i++;
					} else {
						fprintf(stderr, "-p: missing value: INT\n");
						exit(-1);
					}
				}
		else if (strcmp(argv[i], "-b") == 0 ||
		         strcmp(argv[i], "--probe") == 0) {
					if (i+1<argc) {
						probe = tolower(argv[i+1][0]); i++;
					} else {
						fprintf(stderr, "-b: missing value: ICMP|TCP|UDP\n");
						exit(-1);
					}
				}
		else if (strcmp(argv[i], "-a") == 0 ||
		         strcmp(argv[i], "--attempt") == 0) {
					if (i+1<argc) {
						attempt = atoi(argv[i+1]); i++;
					} else {
						fprintf(stderr, "-a: missing value: INT>0\n");
						exit(-1);
					}
				}
		else {
			fprintf(stderr, "%s: unknown option\n", argv[i]);
			UsageTraceroute();
		}
	}
	
	switch (min_ttl && max_ttl && hops && rcv_timeout && snd_timeout && attempt)
	{
		case 0:
			fprintf(stderr, "All TTL values (min, max, hops), Timers (recv, send) and Attempts MUST BE greater than 0 !\n");
			exit(-1);
		default:
			break;
	}
	
	switch (probe) {
		case 'u':
			bytes = UDP_LEN;
			break;
		case 'i':
			bytes = ICMP_LEN;
			break;
		case 't':
			bytes = TCP_LEN;
			break; 
		default :
			fprintf(stderr, "%c: invalid probe method: use with 'udp', 'icmp' (default) or 'tcp'\n", probe);
			exit(-1);
	}


	//-----------------------------------------------------//
	// first information outputs and log
	//-----------------------------------------------------//
	
	// get infos
    ipstr = GetIPFromHostname(argv[1]);
    myip = GetMyIP();
    
    // stdout
    printf("traceroute to %s (%s), %d hops max, %d byte packets\n", argv[1], ipstr, max_ttl, bytes);
    
	// opens a log file, exit if error
	if (log_data == 1)
	{
		logfile = OpenLog();
		if (logfile == NULL) exit(-1);
		fprintf(logfile, "Domain: %s\n", argv[1]);
		fprintf(logfile, "Resolved IP address: %s\n", ipstr);
		fprintf(logfile, "My IP address: %s\n", myip);
	}


	//-----------------------------------------------------//
	// initializing some data
	//-----------------------------------------------------//
	
    // init remote addr structure and other params
    server.sin_family = domain;
    server.sin_port = htons(portno);
    inet_aton(ipstr, &(server.sin_addr));

	// init local addr structure
    my_addr.sin_family = AF_INET;
    my_addr.sin_port = htons(portno);
    inet_aton(myip, &(my_addr.sin_addr));
    
    
    //-----------------------------------------------------//
	// starting traceroute
	//-----------------------------------------------------//
	int ttl_t[4] = {min_ttl, max_ttl, hops, attempt};
	
	switch (probe) {
		case 'u':
		case 'i':
		case 't':
			LoopTrace(rcv_timeout, snd_timeout, ttl_t, logfile, probe, server, my_addr);
			break;
		default: 
			break;
	}
	//-----------------------------------------------------//
	// close log file
	//-----------------------------------------------------//
    if (log_data == 1)
		CloseLog(logfile);

    return (EXIT_SUCCESS);
}

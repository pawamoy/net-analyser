/**\file ping.c
 * \brief ping functions
 */

#include "../include/ping.h"

//~ static float rtt_min     = 0;
//~ static float rtt_max     = 0;
//~ static float rtt_avg     = 0;
static int   count       = 0;
static int   transmitted = 0;
static int   received    = 0;

static char domain[MAX_ADDRESS] = { 0 };

StrPing NewPing()
{
	StrPing p = {1,2,64,-1,25, NULL, NULL, NULL, NULL};
	return p;
}

void UsagePing(void)
{
    fprintf(stderr, "usage: ping servername [-i FREQ] [-t TTL] [-W WAIT_RESPONSE] [-A ATTEMPTS] [-d DELAY_THRESHOLD]\n");
    exit(-1);
}

void handlerArret(int s){
	printf("\n--- %s ping statistics ---\n", domain);
	printf("%d packets transmitted, %d received, %.0f%% packet loss, time ???ms\n",
		transmitted, received, (float)((float)(transmitted-received))/(float)transmitted*100);
	printf("rtt ... in dev\n");
	exit(s); 
}

int main_ping(StrPing p, int* best_ttl)
{
	//-----------------------------------------------------//
	// variable declaration
	//-----------------------------------------------------//
	struct timeval r_timeout = { p.wait, 0 };
    
    Socket send_socket,
           receive_socket;
           
    socklen_t addrlen = sizeof(Sockin);
    
    Sockin server  = { 0 },
           my_addr = { 0 },
           recept  = { 0 };
    
    char recvbuf[MAX_PACKET] = { 0 },
         packet[MAX_PACKET]  = { 0 },
        *rsaddr              = NULL;
    
    char dest[MAX_ADDRESS],
         source[MAX_ADDRESS];
    
    int portno     = 80,
		reached    = 0,
		out        = FAILURE,
		att        = p.attempts,
        bytes      = ICMP_LEN;
    
    struct sigaction sigIntHandler;
    
        
    //-----------------------------------------------------//
	// initialization
	//-----------------------------------------------------//
    strcpy(dest,   p.ipstr);
    strcpy(source, p.myip);
    
    strcpy(domain, p.address);
    
    // init remote addr structure and other params
    server.sin_family = AF_INET;
    server.sin_port = htons(portno);
    inet_aton(p.ipstr, &(server.sin_addr));

	// init local addr structure
    my_addr.sin_family = AF_INET;
    my_addr.sin_port = htons(portno);
    inet_aton(p.myip, &(my_addr.sin_addr));
    
    //gestion Ctrl+C
    if (best_ttl == NULL)
    {
		sigIntHandler.sa_handler = handlerArret;
		sigemptyset(&sigIntHandler.sa_mask);
		sigIntHandler.sa_flags = 0;
		sigaction(SIGINT, &sigIntHandler, NULL);
	}
	
	/*rtt_min = rtt_max = rtt_avg = */count = transmitted = received = 0;
    
    printf("PING %s (%s) %d bytes of data.\n", p.address, p.ipstr, bytes);
    if (p.logfile != NULL)
		fprintf(p.logfile, "PING %s (%s) %d bytes of data.\n", p.address, p.ipstr, bytes);
    
    //-----------------------------------------------------//
	// starting loop
	//-----------------------------------------------------//
	for (;;)
    {
		count++;
		
		send_socket = OpenRawSocket('i');
		receive_socket = OpenRawSocket('i');
		
		if (bind(receive_socket, (struct sockaddr*)&my_addr, addrlen) == -1)
		{
			perror("bind receive socket");
			exit(-1);
		}
		
		ConstructIPHeader((struct iphdr*)packet, p.ttl, source, dest, 'i');
		ConstructICMPHeader((struct icmphdr*)(packet+sizeof(struct iphdr)));
		if ( ! SetHDRINCL(send_socket))                  exit(-1);
		if ( ! SetRCVTimeOut(receive_socket, r_timeout)) exit(-1);

		if (sendto(send_socket, packet, bytes, 0, (struct sockaddr*) &server, addrlen) == -1)
			perror("sendto()");

		else
		{
			reached = 0;
			transmitted++;
			if (recvfrom(receive_socket, recvbuf, MAX_PACKET, 0, (struct sockaddr*)&recept, &addrlen) != -1)
			{
				received++;
				rsaddr = inet_ntoa(recept.sin_addr);				
				if (strcmp(dest, rsaddr)==0)
				{
					// traitement réponse (timeout, ttl exceeded, dest unreach...)
					reached = 1;
					printf("%d bytes from %s (%s): icmp_req=%-2d ttl=%-3d time=???\n", ICMP_LEN, p.address, p.ipstr, count, p.ttl);
					if (p.logfile != NULL)
						fprintf(p.logfile, "%d bytes from %s (%s): icmp_req=%-2d ttl=%-3d time=???\n", ICMP_LEN, p.address, p.ipstr, count, p.ttl);
				}
			}
			
			if (!reached)
			{
				printf("ping: destination unreached: ???\n");
				if (p.logfile != NULL)
					fprintf(p.logfile, "ping: destination unreached: ???\n");
				// attempts
				if (att == 0)
				{
					out = LOSS;
					if (best_ttl != NULL) (*best_ttl)++;
				}
				else if (att > 0)
				{
					close(send_socket);
					close(receive_socket);
					
					do
					{
						count++;
						
						if (best_ttl != NULL) (*best_ttl)++;
						p.ttl++;
						printf("ping: attempt %2d to reach destination (ttl=%d)", att, p.ttl);
						if (p.logfile != NULL)
							fprintf(p.logfile, "ping: attempt %2d to reach destination (ttl=%d)", att, p.ttl);
						
						send_socket = OpenRawSocket('i');
						receive_socket = OpenRawSocket('i');
						
						if (bind(receive_socket, (struct sockaddr*)&my_addr, addrlen) == -1)
						{
							perror("bind receive socket");
							exit(-1);
						}
						
						ConstructIPHeader((struct iphdr*)packet, p.ttl, source, dest, 'i');
						ConstructICMPHeader((struct icmphdr*)(packet+sizeof(struct iphdr)));
						if ( ! SetHDRINCL(send_socket))                  exit(-1);
						if ( ! SetRCVTimeOut(receive_socket, r_timeout)) exit(-1);

						if (sendto(send_socket, packet, bytes, 0, (struct sockaddr*) &server, addrlen) == -1)
							perror("sendto()");

						else
						{
							reached = 0;
							transmitted++;
							if (recvfrom(receive_socket, recvbuf, MAX_PACKET, 0, (struct sockaddr*)&recept, &addrlen) != -1)
							{
								received++;
								rsaddr = inet_ntoa(recept.sin_addr);				
								if (strcmp(dest, rsaddr)==0)
								{
									reached = 1;
									printf("\n%d bytes from %s (%s): icmp_req=%-2d ttl=%-3d time=???\n", ICMP_LEN, p.address, p.ipstr, count, p.ttl);
									if (p.logfile != NULL)
										fprintf(p.logfile, "\n%d bytes from %s (%s): icmp_req=%-2d ttl=%-3d time=???\n", ICMP_LEN, p.address, p.ipstr, count, p.ttl);
									out = LOSS;
								}
							}
							
							if (!reached)
							{
								printf("... failed\n");
								if (p.logfile != NULL)
									fprintf(p.logfile, "... failed\n");
								att--;
							}
						}
						
						close(send_socket);
						close(receive_socket);
						
						if (out != FAILURE) return out;
						
						sleep(p.frequency);
					}
					while (att > 0);
					
					return FAILURE;
				}
				
				if (out != FAILURE) return out;
			}
		}
		
		close(send_socket);
		close(receive_socket);
		
		sleep(p.frequency);
	}
	
	return FAILURE;
}


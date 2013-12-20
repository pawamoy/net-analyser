/**\file ping.c
 * \brief ping functions
 */

#include "../include/ping.h"

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
           printf("%d\n",s);
	   printf("--- Affichage stats ping ---");
           exit(1); 
}

//~ int main_ping(StrPing p, int* best_ttl)
//~ {
	//~ int ip_flags[4] = { 0 },
		//~ cptTransmis = 0,
		//~ cptRecus    = 0;
//~ 
	//~ Socket sd;
	//~ Sockin /**server,*/ my_addr;
//~ 
	//~ struct iphdr hdrip;
	//~ struct icmphdr hdricmp;
	//~ struct addrinfo hints/*, *resolv*/;
	//~ struct sigaction sigIntHandler;
//~ 
	//~ uint8_t *paquet;
//~ 
	//~ //allocation mémoire des diff. var
	//~ paquet = (uint8_t *)malloc(IP_MAXPACKET*sizeof(uint8_t));
	//~ memset (paquet, 0, IP_MAXPACKET*sizeof(uint8_t));
//~ 
	//~ //gestion Ctrl+C
	//~ sigIntHandler.sa_handler = handlerArret;
	//~ sigemptyset(&sigIntHandler.sa_mask);
	//~ sigIntHandler.sa_flags = 0;
	//~ sigaction(SIGINT, &sigIntHandler, NULL);
//~ 
    //~ //Infos pour getaddrinfo().
    //~ memset (&hints, 0, sizeof(struct addrinfo));
    //~ hints.ai_family = AF_INET;
    //~ hints.ai_socktype = 0;
    //~ hints.ai_flags = hints.ai_flags|AI_CANONNAME;
//~ 
    //~ //stdout
    //~ printf("PING %s (%s) %d bytes of data\n", p.address, p.ipstr, LONGHDR_IP+LONGHDR_ICMP);
//~ 
    //~ //Entête IP
    //~ hdrip.ihl = LONGHDR_IP/sizeof(uint32_t);
    //~ hdrip.version = 4;
    //~ hdrip.tos = 0;
    //~ hdrip.tot_len = htons(LONGHDR_IP+LONGHDR_ICMP);
    //~ hdrip.id = htons(0);
    //~ hdrip.frag_off = htons((ip_flags[0]<<15)+(ip_flags[1]<<14)+(ip_flags[2]<<13)+ip_flags[3]);
    //~ hdrip.ttl = *best_ttl;
    //~ hdrip.protocol = IPPROTO_ICMP;
//~ 
    //~ //checksum entête IP
    //~ hdrip.check = 0;
    //~ //hdrip.check = checksum((uint16_t *)&hdrip,LONGHDR_IP);
//~ 
    //~ //Entête ICMP
    //~ hdricmp.type = ICMP_ECHO;
    //~ hdricmp.code = 0;
    //~ hdricmp.un.echo.id = htons(4444);
    //~ hdricmp.un.echo.sequence = htons(0);
    //~ hdricmp.checksum = htons(~(ICMP_ECHO << 8));
//~ 
    //~ //memcpy(paquet, &hdricmp, LONGHDR_IP);
    //~ //memcpy((paquet+LONGHDR_IP), &hdricmp, LONGHDR_ICMP);
//~ 
    //~ //checksum entête ICMP
    //~ //hdricmp.checksum = checksum((uint16_t *)(paquet+LONGHDR_IP), LONGHDR_ICMP);
    //~ //memcpy((paquet+LONGHDR_IP), &hdricmp, LONGHDR_ICMP);
//~ 
    //~ //préparation de l'envoi du paquet
    //~ memset (&my_addr, 0, sizeof (struct sockaddr_in));
    //~ my_addr.sin_family = AF_INET;
    //~ inet_aton(p.ipstr, &(my_addr.sin_addr));
//~ 
    //~ sd = socket (AF_INET, SOCK_RAW, IPPROTO_ICMP); //IPPROTO_RAW
    //~ 
    //~ //envoi du paquet
    //~ if (sendto(sd, paquet, LONGHDR_IP+LONGHDR_ICMP, 0, (struct sockaddr *)&my_addr, sizeof(struct sockaddr)) == -1)
    //~ {
		//~ perror ("échec sendto()");
		//~ exit(EXIT_FAILURE);
    //~ }
    //~ else
    //~ {
		//~ printf("Envoi du paquet ICMP réussi\n");
		//~ printf("Envoi du paquet ICMP %u réussi\n", ntohs(hdricmp.un.echo.sequence));
		//~ cptTransmis++;
    //~ }
    //~ 
    //~ //réception du paquet
    //~ if (recvfrom(sd, paquet, LONGHDR_IP+LONGHDR_ICMP, 0, (struct sockaddr *)&my_addr, (socklen_t*)sizeof(struct sockaddr)) == -1)
    //~ {
		//~ perror ("échec recvfrom()");
		//~ exit(EXIT_FAILURE);
    //~ }
    //~ else
    //~ {
		//~ printf("Réception du paquet ICMP %u réussie\n", ntohs(hdricmp.un.echo.sequence));
		//~ cptRecus++;
    //~ }
//~ 
    //~ //fermeture socket
    //~ close (sd);
	//~ 
    //~ //free mémoire
    //~ //free (cible);
    //~ free (paquet);
//~ 
    //~ return LOSS;
//~ }

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
		out        = FAILURE,
		att        = p.attempts,
        bytes      = ICMP_LEN;
    
    struct sigaction sigIntHandler;
    
        
    //-----------------------------------------------------//
	// initialization
	//-----------------------------------------------------//
    strcpy(dest,   p.ipstr);
    strcpy(source, p.myip);
    
    // init remote addr structure and other params
    server.sin_family = AF_INET;
    server.sin_port = htons(portno);
    inet_aton(p.ipstr, &(server.sin_addr));

	// init local addr structure
    my_addr.sin_family = AF_INET;
    my_addr.sin_port = htons(portno);
    inet_aton(p.myip, &(my_addr.sin_addr));
    
    //gestion Ctrl+C
    if (p.attempts == -1)
    {
		sigIntHandler.sa_handler = handlerArret;
		sigemptyset(&sigIntHandler.sa_mask);
		sigIntHandler.sa_flags = 0;
		sigaction(SIGINT, &sigIntHandler, NULL);
	}
    
    
    //-----------------------------------------------------//
	// starting loop
	//-----------------------------------------------------//
	for (;;)
    {
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
			if (recvfrom(receive_socket, recvbuf, MAX_PACKET, 0, (struct sockaddr*)&recept, &addrlen) != -1)
			{
				rsaddr = inet_ntoa(recept.sin_addr);				
				if (strcmp(dest, rsaddr)==0)
				{
					printf("PING OK\n");
					// traitement réponse (timeout, ttl exceeded, dest unreach...)
				}
			}
			else
			{
				printf("PING PAS OK\n");
				// attempts
				if (att == 0)
				{
					out = LOSS;
					//~ *best_ttl++;
				}
				else if (att > 0)
				{
					printf("ATTEMPTS...\n");
					close(send_socket);
					close(receive_socket);
					
					do
					{
						printf("att %d\n", att);
						p.ttl = *++best_ttl;
						
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
							if (recvfrom(receive_socket, recvbuf, MAX_PACKET, 0, (struct sockaddr*)&recept, &addrlen) != -1)
							{
								rsaddr = inet_ntoa(recept.sin_addr);				
								if (strcmp(dest, rsaddr)==0)
								{
									out = LOSS;
								}
							}
							else
							{
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


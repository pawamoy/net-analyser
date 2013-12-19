/**\file ping.c
 * \brief netanalyser functions
 */

#include "../include/netanalyser.h"


void ShowStatistics()
{
	printf("Statistics :D\n");
}

void handler(int signum)
{
	ShowStatistics();
	CloseLog();
	exit(signum);
}

int ping(char* address, int threshold, int frequency, int attempts, int* best_ttl)
{
	int i;
	for (i=0; i<2; i++)
	{
		printf("Ping at %s with a threshold of %d%%, a frequency of %d, %d attempts if packets are lost, and a max ttl of %d\n",
			address, threshold, frequency, attempts, *best_ttl);
		sleep(frequency);
	}
	return LOSS;
}

int HostIsJoinable(char* address, int max_ping, int attempts)
{
	//-----------------------------------------------------//
	// variable declaration
	//-----------------------------------------------------//
	struct timeval r_timeout = { 1, 0 },
                   s_timeout = { 1, 0 };
    
    Socket send_socket,
           receive_socket;
           
    socklen_t addrlen = sizeof(Sockin);
    
    Sockin recept = { 0 };
    
    char recvbuf[MAX_PACKET] = { 0 },
         packet[MAX_PACKET]  = { 0 },
        *rsaddr              = NULL;
    
    char source[MAX_ADDRESS];
    strcpy(source, inet_ntoa(my_addr.sin_addr));
    
    int reach_dest = 0,
        att        = 0,
        bytes      = ICMP_LEN;
        

    //-----------------------------------------------------//
	// starting attempts loop
	//-----------------------------------------------------//
    for (att=0; att<attempts; att++)
    {
		send_socket = OpenRawSocket('i');
		receive_socket = OpenRawSocket('i');
		
		if (bind(receive_socket, (struct sockaddr*)&my_addr, addrlen) == -1)
		{
			perror("bind receive socket");
			exit(-1);
		}
		
		ConstructIPHeader((struct iphdr*)packet, max_ping, source, address, 'i');
		ConstructICMPHeader((struct icmphdr*)(packet+sizeof(struct iphdr)));
		if ( ! SetHDRINCL(send_socket))                  exit(-1);
		if ( ! SetSNDTimeOut(send_socket, s_timeout))    exit(-1);
		if ( ! SetRCVTimeOut(receive_socket, r_timeout)) exit(-1);

		if (sendto(send_socket, packet, bytes, 0, (struct sockaddr*) &server, addrlen) == -1)
			perror("sendto()");

		else
		{
			if (recvfrom(receive_socket, recvbuf, MAX_PACKET, 0, (struct sockaddr*)&recept, &addrlen) != -1)
			{
				rsaddr = inet_ntoa(recept.sin_addr);				
				if (strcmp(address, rsaddr)==0)
					reach_dest = 1;
			}
		}
		
		close(send_socket);
		close(receive_socket);
		
		if (reach_dest == 1)
			return 1;
	}
	
	return 0;
}

/**\file main_traceroute.c
 * \brief traceroute main
 * \date December 10, 2013, 10:37 AM
 */

#include <assert.h>
#include "../include/traceroute.h"
#include "../include/log.h" 
 
/*
 * Transformer le nom de domaine fourni en adresse IP										DONE
 * SetTTL																					DONE
 * 
 * modif champ TTL dans l'entête IP
 * à 0, le routeur renvoie TTL exceeded
 * Reverse-DNS pour avoir le nom de routeurs à partir de l'IP
 * support de TCP, UDP et ICMP
 * plusieurs modes : pas (nb sauts), fréquence sonde, tentatives, temporisateurs
 * 
 * TODO:
 * 		fonction pour envoyer un paquet plusieurs fois en augmentant le ttl à chaque fois
 * 			ttl static dans la fonction ? 
 * 			passage de paramètres ? (ttl, pas, freq, tentative) 
 * 			boucle dans/sur la fonction ?
 * 
 * RAW SOCKET - UDP
 * http://austinmarton.wordpress.com/2011/09/14/sending-raw-ethernet-packets-from-a-specific-interface-in-c-on-linux/
 * http://pwet.fr/man/linux/conventions/raw
 * http://pwet.fr/man/linux/conventions/packet
 * http://stackoverflow.com/questions/14774668/what-is-raw-socket-in-socket-programming
 * http://www.tenouk.com/Module43a.html
 */




int main(int argc, char** argv)
{
    Socket sockfd = 0;
    
    
	struct sockaddr_in server;
	socklen_t addrlen = 0;
    
	int portno = 0;
	int domain = AF_INET;
	char *ipstr = NULL;

    //~ FILE* logfile;

    // check the number of args on command line
    if (argc != 2) 
    {
        Usage();
    }
    
    // check root privileges
    if (getuid())
    {
        fprintf(stderr, "\nError: you must be root to use raw sockets\n");
        exit(-1);
    }
    
    /* opens a log file */
    //~ logfile = OpenLog();
    //~ assert(logfile != NULL);
    
    //~ WriteLog(logfile, "Domain: ");
    //~ WriteLogLF(logfile, argv[1]);
    
    int one = 1;
    int* val = &one;

	ipstr = GetIPFromHostname(argv[1]);
	portno = 80;

    // init remote addr structure and other params
	server.sin_family = domain;
	server.sin_port = htons(portno);
	addrlen = sizeof (struct sockaddr_in);
    
	inet_aton(ipstr, &(server.sin_addr));


	/* UDP Version */
	
	sockfd = OpenRawSocket('U');
	
	// Do not fill the packet structure
	if (setsockopt(sockfd, IPPROTO_IP, IP_HDRINCL, val, sizeof(one)) < 0)
	{
		perror("setsockopt()");
		exit(-1);
	}
	
	PacketUDP PU;
	// I don't think giving 'localhost' will work, how to get our own local IP address ?
	ConstructUDPPacket(&PU, "127.0.0.1", ipstr);

    int ttl = 0;
    
    for (ttl = 1; ttl < 16; ttl++)
    {
		SetIPHeaderTTL(&(PU.iph), ttl);
		
		if (sendto(sockfd, &PU, PU.iph.tot_len, 0, (struct sockaddr*)&server, addrlen) < 0)
		{
			perror("sendto()");
			//~ exit(-1);
		}
		else
		{
			printf("TTL #%-2d - sendto() OK\n", ttl);
			sleep(1);
		}
		
		/* need a recvfrom: TTL exceeded or TTL ... */
    }
    
    close(sockfd);
    
    /* End of UDP Version */
    
    //~ CloseLog(logfile);

    return (EXIT_SUCCESS);
}

/**\file main_traceroute.c
 * \brief traceroute main
 * \date December 10, 2013, 10:37 AM
 */

#include <assert.h>
#include "../include/traceroute.h"
#include "../include/log.h" 
#include <errno.h>

#define MAX_PACKET 1024

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
 * http://forum.hardware.fr/hfr/Programmation/C-2/code-source-ping-sujet_30136_1.htm
 * http://stackoverflow.com/questions/13543554/how-to-receive-icmp-request-in-c-with-raw-sockets
 * http://cities.lk.net/trproto.html
 */

void SwitchErrno(int);

void SwitchErrno(int err)
{
    switch (err)
    {
        case EACCES: printf("EACCES\n");                break;
        case EADDRINUSE: printf("EADDRINUSE\n");        break;
        case EADDRNOTAVAIL: printf("EADDRNOTAVAIL\n");  break;
        case EAGAIN: printf("EAGAIN/EWOULDBLOCK\n");    break;
        case EBADF: printf("EBADF\n");                  break;
        case ECONNRESET: printf("ECONNRESET\n");        break;
        case EDESTADDRREQ: printf("EDESTADDRREQ\n");    break;
        case EFAULT: printf("EFAULT\n");                break;
        case EINTR: printf("EINTR\n");                  break;
        case EINVAL: printf("EINVAL\n");                break;
        case EISCONN: printf("EISCONN\n");              break;
        case ELOOP: printf("ELOOP\n");                  break;
        case EMSGSIZE: printf("EMSGSIZE\n");            break;
        case ENAMETOOLONG: printf("ENAMETOOLONG\n");    break;
        case ENOENT: printf("ENOENT\n");                 break;
        case ENOTDIR: printf("ENOTDIR\n");              break;
        case ENOBUFS: printf("ENOBUFS\n");              break;
        case ENOMEM: printf("ENOMEM\n");                break;
        case ENOTCONN: printf("ENOTCONN\n");            break;
        case ENOTSOCK: printf("ENOTSOCK\n");            break;
        case EOPNOTSUPP: printf("EOPNOTSUPP\n");        break;
        case EPIPE: printf("EPIPE\n");                  break;
        case EROFS: printf("EROFS\n");                  break;
    }
}

int main(int argc, char** argv)
{
    Socket send_socket    = 0,
	       receive_socket = 0;

    struct sockaddr_in server,
                       my_addr;
    socklen_t addrlen    = 0,
	          my_addrlen = 0;

    struct iphdr* iph;
    //~ struct icmphdr* icmph;

    char rsaddr[128];
    char rdaddr[128];

    int portno = 80;
    int domain = AF_INET;
    char *ipstr = NULL;
    char* myip = NULL;

    int bread;
    int bwrote;
    
    //~ int one = 1;
    //~ int* val = &one;

    char recvbuf[1024];

    //~ FILE* logfile;

    // check the number of args on command line
    if (argc != 2)
        Usage();

    // check root privileges
    if (getuid()) {
        fprintf(stderr, "\nError: you must be root to use raw sockets\n");
        exit(-1);
    }

    /* opens a log file */
    //~ logfile = OpenLog();
    //~ assert(logfile != NULL);

    printf("Domain: %s\n", argv[1]);
    //~ WriteLog(logfile, "Domain: ");
    //~ WriteLogLF(logfile, argv[1]);


    ipstr = GetIPFromHostname(argv[1]);
    printf("Resolved address: %s\n", ipstr);
    //~ WriteLog(logfile, "Resolved IP address: ");
    //~ WriteLogLF(logfile, ipstr);

    myip = GetMyIP();
    printf("My own IP address: %s\n\n\n", myip);
    //~ WriteLog(logfile, "My IP address: ");
    //~ WriteLogLF(logfile, myip);

    // init remote addr structure and other params
    server.sin_family = domain;
    server.sin_port = htons(portno);
    addrlen = sizeof (struct sockaddr_in);
    inet_aton(ipstr, &(server.sin_addr));

	// init local addr structure and other params
    my_addr.sin_family      = AF_INET;
    my_addr.sin_port        = htons(atoi(argv[1]));
    my_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    my_addrlen              = sizeof(struct sockaddr_in);
    
    
    
    
    
    
    
    /* UDP Version */


    // Kernel, please do not fill the packet structure
    //~ if (setsockopt(send_socket, IPPROTO_IP, IP_HDRINCL, val, sizeof (one)) < 0)
    //~ {
        //~ perror("setsockopt(): cannot set IP_HDRINCL to true");
        //~ exit(-1);
    //~ }
    
    // Timeout of 3 seconds for the socket
    struct timeval timeout = { 2, 0};   /* 2 seconds, 0 microseconds */

    //~ PacketUDP PU;
    //~ ConstructUDPHeader(&(PU.udph));

    int ttl = 0;

    for (ttl = 1; ttl <= 16; ttl++)
    {
		//~ ConstructIPHeader(&(PU.iph), ttl, myip, ipstr, 'U');
        //~ SetIPHeaderTTL(&(PU.iph), ttl);
        
		send_socket    = OpenDgramSocket('U');
		receive_socket = OpenRawSocket('I');
		
		if (bind(receive_socket, (struct sockaddr*)&my_addr, my_addrlen) == -1)
		{
			perror("bind receive socket");
			SwitchErrno(errno);
		}
		
        if ( ! SetTTL(send_socket, ttl))
			exit(-1);
			
		if ( ! SetRCVTimeOut(receive_socket, timeout))
			exit(-1);

        //~ bwrote = sendto(send_socket, &PU, PU.iph.tot_len, 0, (struct sockaddr*) &server, addrlen);
        bwrote = sendto(send_socket, "", 0, 0, (struct sockaddr*) &server, addrlen);
        if (bwrote == -1)
        {
            perror("sendto()");
            //~ SwitchErrno(errno);
        }
        //~ else
        //~ {
            //~ printf("TTL %-2d - sendto() OK\n", ttl);
        //~ }

        bread = recvfrom(receive_socket, recvbuf, MAX_PACKET, 0, NULL, NULL); //
        if (bread == -1)
        {
            //~ perror("recvfrom()");
            //~ SwitchErrno(errno);
            printf(" %-2d %-15s *\n", ttl, "*");
        }
        else
        {
            // Print recvbuf contents :
            iph = (struct iphdr*) recvbuf;

            inet_ntop(AF_INET, &(iph->saddr), rsaddr, 128);
            inet_ntop(AF_INET, &(iph->daddr), rdaddr, 128);

            printf(" %-2d %-15s %s\n", ttl, rsaddr, GetHostNameFromIP(rsaddr));
            //~ printf("daddr: %s\t%s\n", rdaddr, GetHostNameFromIP(rdaddr));
        }
		
		close(send_socket);
		close(receive_socket);
    }


    /* End of UDP Version */

    //~ CloseLog(logfile);

    return (EXIT_SUCCESS);
}

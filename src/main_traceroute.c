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
 */
 
void SwitchErrno(int);

void SwitchErrno(int err)
{
	switch (err)
	{
		case EACCES: printf("EACCES\n"); break;
		 //~ (For UNIX domain sockets,  which  are  identified  by  pathname)
		  //~ Write  permission  is  denied on the destination socket file, or
		  //~ search permission is denied for one of the directories the  path
		  //~ prefix.  (See path_resolution(7).)
		  //~ (For  UDP  sockets)  An  attempt  was  made  to  send  to a net‐
		  //~ work/broadcast address as though it was a unicast address.
		case EAGAIN: printf("EAGAIN or EWOULDBLOCK\n"); break;
		  //~ The socket is marked nonblocking  and  the  requested  operation
		  //~ would  block.   POSIX.1-2001  allows either error to be returned
		  //~ for this case, and does not require these constants to have  the
		  //~ same value, so a portable application should check for both pos‐
		  //~ sibilities.
		case EBADF: printf("EBADF\n"); break;
		//~ An invalid descriptor was specified.
		case ECONNRESET: printf("ECONNRESET\n"); break;
		  //~ Connection reset by peer.
		case EDESTADDRREQ: printf("EDESTADDRREQ\n"); break;
		  //~ The socket is not connection-mode, and no peer address is set.
		case EFAULT: printf("EFAULT\n"); break;
		//~ An invalid user space address was specified for an argument.
		case EINTR: printf("EINTR\n"); break;
			//~ A signal occurred before any  data  was  transmitted;  see  sig‐
		  //~ nal(7).
		case EINVAL:  printf("EINVAL\n"); break;
		//~ Invalid argument passed.
		case EISCONN: printf("EISCONN\n"); break;
		  //~ The connection-mode socket was connected already but a recipient
		  //~ was specified.  (Now either  this  error  is  returned,  or  the
		  //~ recipient specification is ignored.)
		case EMSGSIZE: printf("EMSGSIZE\n"); break;
		  //~ The  socket  type  requires that message be sent atomically, and
		  //~ the size of the message to be sent made this impossible.
		case ENOBUFS: printf("ENOBUFS\n"); break;
		  //~ The output queue for a network interface was full.  This  gener‐
		  //~ ally  indicates  that the interface has stopped sending, but may
		  //~ be caused by transient congestion.   (Normally,  this  does  not
		  //~ occur in Linux.  Packets are just silently dropped when a device
		  //~ queue overflows.)
		case ENOMEM:  printf("ENOMEM\n"); break;
		//~ No memory available.
		case ENOTCONN: printf("ENOTCONN\n"); break;
		  //~ The socket is not connected, and no target has been given.
		case ENOTSOCK: printf("ENOTSOCK\n"); break;
		  //~ The argument sockfd is not a socket.
		case EOPNOTSUPP: printf("EOPNOTSUPP\n"); break;
		  //~ Some bit in the flags argument is inappropriate for  the  socket
		  //~ type.
		case EPIPE:   printf("EPIPE\n"); break;
		//~ The  local  end  has  been  shut  down  on a connection oriented
		  //~ socket.  In this case the process will also  receive  a  SIGPIPE
		  //~ unless MSG_NOSIGNAL is set.
	}
}


int main(int argc, char** argv)
{
    Socket sockfd = 0;

    struct sockaddr_in server;
    socklen_t addrlen = 0;
    
    struct iphdr* iph;
    //~ struct icmphdr* icmph;

	char rsaddr[128];
	char rdaddr[128];

    int portno = 0;
    int domain = AF_INET;
    char *ipstr = NULL;
    char* myip = NULL;
    
    int bread;
    int bwrote;
    
    char recvbuf[1024];

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

	printf("Domain: %s\n", argv[1]);
    //~ WriteLog(logfile, "Domain: ");
    //~ WriteLogLF(logfile, argv[1]);

    int one = 1;
    int* val = &one;
    
    //~ int timeout = 1000;
    //~ int* time_val = &timeout;

    ipstr = GetIPFromHostname(argv[1]);
    printf("Resolved address: %s\n", ipstr);
    //~ WriteLog(logfile, "Resolved IP address: ");
	//~ WriteLogLF(logfile, ipstr);
	
	myip = GetMyIP();
	printf("My own IP address: %s\n\n\n", myip);
    //~ WriteLog(logfile, "My IP address: ");
	//~ WriteLogLF(logfile, myip);
	
    portno = 80;

    // init remote addr structure and other params
    server.sin_family = domain;
    server.sin_port = htons(portno);
    addrlen = sizeof (struct sockaddr_in);

    inet_aton(ipstr, &(server.sin_addr));


    /* UDP Version */

    sockfd = OpenRawSocket('U');

    // Kernel, please do not fill the packet structure
    if (setsockopt(sockfd, IPPROTO_IP, IP_HDRINCL, val, sizeof (one)) < 0)
    {
        perror("setsockopt(): cannot set IP_HDRINCL to true");
        exit(-1);
    }
    
    // Sets timeout
	//~ bread = setsockopt(sockfd, SOL_SOCKET, SO_SNDTIMEO, time_val, sizeof(timeout));
	//~ if (bread == -1)
	//~ {
		//~ perror("setsockopt(SO_SNDTIMEO)");
		//~ SwitchErrno(errno);
		//~ exit(-1);
	//~ }

    PacketUDP PU;
    ConstructUDPPacket(&PU, myip, ipstr);

    int ttl = 0;

    for (ttl = 1; ttl < 16; ttl++)
    {
        SetIPHeaderTTL(&(PU.iph), ttl);

        bwrote = sendto(sockfd, &PU, PU.iph.tot_len, 0, (struct sockaddr*)&server, addrlen);
        if (bwrote == -1)
        {
            perror("sendto()");
            SwitchErrno(errno);
        }
        else
        {
            printf("TTL %-2d - sendto() OK\n", ttl);
        }

        bread = recvfrom(sockfd, recvbuf, MAX_PACKET, 0, (struct sockaddr*)&server, &addrlen);
        if (bread == -1)
        {
			perror("recvfrom()");
			SwitchErrno(errno);
		}
		else
		{
			// Print recvbuf contents :
			iph = (struct iphdr*)recvbuf;
			printf("Received TTL: %d\n", iph->ttl);
			//~ printf("Received Protocol: %d\n", iph->protocol);
			printf("Received saddr: %s\n", inet_ntop(AF_INET, &(iph->saddr), rsaddr, 128));
			printf("... corresponding to host %s\n", GetHostNameFromIP(inet_ntop(AF_INET, &(iph->saddr), rsaddr, 128)));
			printf("Received daddr: %s\n", inet_ntop(AF_INET, &(iph->daddr), rdaddr, 128));
		}
			
    }

    close(sockfd);

    /* End of UDP Version */

    //~ CloseLog(logfile);

    return (EXIT_SUCCESS);
}

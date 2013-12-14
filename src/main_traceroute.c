/**\file main_traceroute.c
 * \brief traceroute main
 * \date December 10, 2013, 10:37 AM
 */

#include "../include/traceroute.h"
#include "../include/log.h" 
//~ #include <errno.h>

#define MAX_PACKET 1024
#define MAX_ADDRESS 128

/*
 * Transformer le nom de domaine fourni en adresse IP										DONE
 * SetTTL																					DONE
 * modif champ TTL sur socket																DONE
 * à 0, le routeur renvoie TTL exceeded														OSEF
 * Reverse-DNS pour avoir le nom de routeurs à partir de l'IP								DONE
 * support de TCP, UDP et ICMP																DONE: UDP
 * plusieurs modes : pas (nb sauts), fréquence sonde, tentatives, temporisateurs			INDEV
 * 
 * Ressources internet:
 * http://austinmarton.wordpress.com/2011/09/14/sending-raw-ethernet-packets-from-a-specific-interface-in-c-on-linux/
 * http://pwet.fr/man/linux/conventions/raw
 * http://pwet.fr/man/linux/conventions/packet
 * http://stackoverflow.com/questions/14774668/what-is-raw-socket-in-socket-programming
 * http://www.tenouk.com/Module43a.html
 * http://forum.hardware.fr/hfr/Programmation/C-2/code-source-ping-sujet_30136_1.htm
 * http://stackoverflow.com/questions/13543554/how-to-receive-icmp-request-in-c-with-raw-sockets
 * http://cities.lk.net/trproto.html
 */

/*void SwitchErrno(int);

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
}*/

int main(int argc, char** argv)
{
	//-----------------------------------------------------//
	// variable declaration
	//-----------------------------------------------------//
    Socket             send_socket          = 0,
	                   receive_socket       = 0;
    struct sockaddr_in server,
                       my_addr;
    struct iphdr      *iph                  = NULL;
    socklen_t          addrlen              = 0,
	                   my_addrlen           = 0;
    char               rsaddr[MAX_ADDRESS],
                       recvbuf[MAX_PACKET];
    int                domain               = AF_INET,
                       portno               = 80,
                       ttl                  = 0,
                       min_ttl              = 1,
                       max_ttl              = 30,
                       hops                 = 1,
					   rcv_timeout          = 3,
					   snd_timeout          = 3,
                       i                    = 0;
    char              *ipstr                = NULL,
                      *myip                 = NULL;
    FILE              *logfile              = NULL;


	//-----------------------------------------------------//
	// first verifications
	//-----------------------------------------------------//
	
    // check the number of args on command line
    if (argc < 2) Usage();

    // check root privileges
    if (getuid()) {
        fprintf(stderr, "\nError: you must be root to use raw sockets\n");
        exit(-1);
    }
    
    // argument analysis
    for (i=1; i<argc; i++)
    {
		     if (strcmp(argv[i], "-m") == 0 ||
		         strcmp(argv[i], "--maxttl") == 0)       max_ttl = atoi(argv[i+1]);
		else if (strcmp(argv[i], "-n") == 0 ||
		         strcmp(argv[i], "--minttl") == 0)       min_ttl = atoi(argv[i+1]);
		else if (strcmp(argv[i], "-h") == 0 ||
		         strcmp(argv[i], "--hops") == 0)         hops = atoi(argv[i+1]);
		else if (strcmp(argv[i], "-r") == 0 ||
		         strcmp(argv[i], "--recv-timeout") == 0) rcv_timeout = atoi(argv[i+1]);
		else if (strcmp(argv[i], "-s") == 0 ||
		         strcmp(argv[i], "--send-timeout") == 0) snd_timeout = atoi(argv[i+1]);
	}


	//-----------------------------------------------------//
	// first information outputs and log
	//-----------------------------------------------------//
	
    // opens a log file, exit if error
    logfile = OpenLog();
    if (logfile == NULL) exit(-1);

	// get infos
    ipstr = GetIPFromHostname(argv[1]);
    myip = GetMyIP();
    
    // stdout
    printf("Domain: %s\tAddress: %s\n", argv[1], ipstr);
    
	// log
    WriteLog(logfile, "Domain: ");
    WriteLogLF(logfile, argv[1]);
    WriteLog(logfile, "Resolved IP address: ");
    WriteLogLF(logfile, ipstr);
    WriteLog(logfile, "My IP address: ");
    WriteLogLF(logfile, myip);


	//-----------------------------------------------------//
	// initializing some data
	//-----------------------------------------------------//
	
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
    
    
    
    
    
    
    
    //-----------------------------------------------------//
	// starting UDP version
	//-----------------------------------------------------//

    struct timeval r_timeout = { rcv_timeout, 0 };
    struct timeval s_timeout = { snd_timeout, 0 };

    for (ttl = min_ttl; ttl <= max_ttl; ttl += hops)
    {
		send_socket    = OpenDgramSocket('U');
		receive_socket = OpenRawSocket('I');
		
		if (bind(receive_socket, (struct sockaddr*)&my_addr, my_addrlen) == -1)
		{
			perror("bind receive socket");
			exit(-1);
		}
		
        if ( ! SetTTL(send_socket, ttl))
			exit(-1);
		
		if ( ! SetSNDTimeOut(send_socket, s_timeout))
			exit(-1);
		
		if ( ! SetRCVTimeOut(receive_socket, r_timeout))
			exit(-1);

        if (sendto(send_socket, "", 0, 0, (struct sockaddr*) &server, addrlen) == -1)
            perror("sendto()");
		else
		{
			if (recvfrom(receive_socket, recvbuf, MAX_PACKET, 0, NULL, NULL) == -1)
				printf(" %-2d %-15s *\n", ttl, "*");
			else
			{
				iph = (struct iphdr*) recvbuf;
				inet_ntop(AF_INET, &(iph->saddr), rsaddr, 128);
				printf(" %-2d %-15s %s\n", ttl, rsaddr, GetHostNameFromIP(rsaddr));
			}
		}
		
		close(send_socket);
		close(receive_socket);
    }


    /* End of UDP Version */

    CloseLog(logfile);

    return (EXIT_SUCCESS);
}

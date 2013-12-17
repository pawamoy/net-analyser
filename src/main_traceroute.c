/**\file main_traceroute.c
 * \brief traceroute main
 * \date December 10, 2013, 10:37 AM
 */

#include "../include/traceroute.h"
#include "../include/log.h" 
//~ #include <errno.h>

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
 * https://blogs.oracle.com/ksplice/entry/learning_by_doing_writing_your
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
    struct sockaddr_in server,
                       my_addr;
    int                domain       = AF_INET,
                       portno       = 80,
                       min_ttl      = 1,
                       max_ttl      = 30,
                       hops         = 1,
					   rcv_timeout  = 3,
					   snd_timeout  = 3,
                       i            = 0,
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
    if (argc < 2) Usage();

    // check root privileges
    //~ uid = getuid();
    //~ setuid(uid);
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
		else if (strcmp(argv[i], "-l") == 0 ||
		         strcmp(argv[i], "--log") == 0)          log_data = 1;
		else if (strcmp(argv[i], "-p") == 0 ||
		         strcmp(argv[i], "--port") == 0)         portno = atoi(argv[i+1]);
		else if (strcmp(argv[i], "-b") == 0 ||
		         strcmp(argv[i], "--probe") == 0)        probe = tolower(argv[i+1][0]);
	}
	
	switch (probe) {
		case 'u':
		case 'i':
		case 't':
			break;
		default :
			fprintf(stderr, "Invalid probe method: use with 'udp', 'icmp' or 'tcp'");
			exit(-1);
	}


	//-----------------------------------------------------//
	// first information outputs and log
	//-----------------------------------------------------//
	
	// get infos
    ipstr = GetIPFromHostname(argv[1]);
    myip = GetMyIP();
    
    // stdout
    printf("Domain: %s\nAddress: %s\n\n", argv[1], ipstr);
    
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
    my_addr.sin_family      = AF_INET;
    my_addr.sin_port        = htons(portno);
    inet_aton(myip, &(my_addr.sin_addr));
    //~ my_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    
    
    //-----------------------------------------------------//
	// starting loops
	//-----------------------------------------------------//
	int ttl_t[3] = {min_ttl, max_ttl, hops};
	
	printf("With UDP Probes\n");
	//~ LoopUDP(rcv_timeout, snd_timeout, ttl_t, logfile, server, my_addr);

	printf("With ICMP Probes\n");
	//LoopICMP(rcv_timeout, snd_timeout, ttl_t, logfile, server, my_addr);

	printf("With TCP Probes\n");
	LoopTCP(rcv_timeout, snd_timeout, ttl_t, logfile, server, my_addr);
	
	//-----------------------------------------------------//
	// close log file
	//-----------------------------------------------------//
    if (log_data == 1)
		CloseLog(logfile);

    return (EXIT_SUCCESS);
}

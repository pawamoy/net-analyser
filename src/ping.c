/**\file ping.c
 * \author val
 * \brief ping main
 * \date December 10, 2013, 10:37 AM
 */

#include "../include/ping.h"
//#include <netinet/udp.h>
//#include <netinet/tcp.h>

#define LONGHDR_IP 20
#define LONGHDR_ICMP 8
#define IP_MAXPACKET 1024

/*
 * TODO:
 * 	- utiliser le protocole ICMP afin d’envoyer, vers une machine ou un routeur, un message de type ECHO_REQUEST
 *	(réponse avec un ECHO_REPLY)
 *	- Les pertes ainsi que le RTT sont enregistrés
 *	- être capable d’envoyer des sondes ICMP, UDP et TCP et "interpréter les différents type de messages reçus"
 */

//<--- FUNCTIONS --->
char* GetIPFromHostname(const char* hostname)
{
    struct hostent *he;
    struct in_addr **addr_list;
    int i;
         
    if ( (he = gethostbyname( hostname ) ) == NULL)
    {
        // get the host info
        herror("gethostbyname()");
        exit(-1);
    }
 
    addr_list = (struct in_addr **) he->h_addr_list;
     
    for(i = 0; addr_list[i] != NULL; i++)
    {
        //Return the first one;
        return inet_ntoa(*addr_list[i]);
    }
     
    exit(-1);
}

char* GetMyIP()
{
	struct ifaddrs *ifaddr, *ifa;
	int family, s;
	char* host = (char*)malloc(128*sizeof(char));

	if (getifaddrs(&ifaddr) == -1) {
	   perror("getifaddrs");
	   exit(EXIT_FAILURE);
	}

	/* Walk through linked list, maintaining head pointer so we
	  can free list later */

	for (ifa = ifaddr; ifa != NULL; ifa = ifa->ifa_next) {
	   if (ifa->ifa_addr == NULL)
		   continue;

	   family = ifa->ifa_addr->sa_family;

	   /* For an AF_INET* interface address, display the address */

	   if (family == AF_INET || family == AF_INET6) {
		   s = getnameinfo(ifa->ifa_addr,
				   (family == AF_INET) ? sizeof(struct sockaddr_in) :
										 sizeof(struct sockaddr_in6),
				   host, NI_MAXHOST, NULL, 0, NI_NUMERICHOST);
		   if (s != 0) {
			   printf("getnameinfo() failed: %s\n", gai_strerror(s));
			   exit(EXIT_FAILURE);
		   }
		   if (IsMyAddress(host) == 1)
		   {
				break;
		   }
		}
	}

	freeifaddrs(ifaddr);
	return host;
}

int IsMyAddress(char* addr)
{
	if (strcmp(addr, "127.0.0.1") == 0)
	{
		return 0;
	}
	
	if (strlen(addr) >= 8)
	{
		if (isdigit(addr[0]) &&
			isdigit(addr[1]) &&
			isdigit(addr[2]) &&
			addr[3] == '.')
		{
			return 1;
		}
	}
	
	return 0;
}

void Usage(void)
{
    printf("USAGE: ping servername\n");
    exit(-1);
}

//<--- MAIN --->
int main(int argc, char** argv)
{
    int *ip_flags, sd;
    char *cible, *ipstr = NULL, *myip = NULL;
    struct iphdr hdrip;
    struct icmphdr hdricmp;
    uint8_t *paquet;
    struct addrinfo hints, *resolv;
    struct sockaddr_in *server, my_addr;

	//vérification arg
    if (argc != 2) Usage();   
	
    //vérif. root
    if (getuid())
    {
        fprintf(stderr, "\nError: you must be root to use raw sockets\n");
        exit(-1);
    }

	//allocation mémoire des diff. var
	cible = (char *)malloc(40*sizeof(char));
        memset(cible, 0, 40*sizeof(char));
	ipstr = (char *)malloc((INET_ADDRSTRLEN*sizeof(char)));
	memset(ipstr, 0, (INET_ADDRSTRLEN*sizeof(char)));
	myip = (char *)malloc((INET_ADDRSTRLEN*sizeof(char)));
	memset(ipstr, 0, (INET_ADDRSTRLEN*sizeof(char)));
	ip_flags = (int *)malloc(4*sizeof(int));
        memset (ip_flags, 0, 4*sizeof(int));
	paquet = (uint8_t *)malloc(IP_MAXPACKET*sizeof(uint8_t));
        memset (paquet, 0, IP_MAXPACKET*sizeof(uint8_t));

    //@IP à partir du domaine fourni en arg
    ipstr = GetIPFromHostname(argv[1]);
    //@source IPv4
    myip = GetMyIP();

    //Infos pour getaddrinfo().
    memset (&hints, 0, sizeof(struct addrinfo));
    hints.ai_family = AF_INET;
    hints.ai_socktype = 0;
    hints.ai_flags = hints.ai_flags|AI_CANONNAME;

    //stdout
    printf("Domaine: %s\nAddresse: %s\n\n", argv[1], ipstr);

    //Entête IP
    hdrip.ihl = LONGHDR_IP/sizeof(uint32_t);
    hdrip.version = 4;
    hdrip.tos = 0;
    hdrip.tot_len = htons(LONGHDR_IP+LONGHDR_ICMP);
    hdrip.id = htons(0);
    ip_flags[0] = 0;
    ip_flags[1] = 0;
    ip_flags[2] = 0;
    ip_flags[3] = 0;
    hdrip.frag_off = htons((ip_flags[0]<<15)+(ip_flags[1]<<14)+(ip_flags[2]<<13)+ip_flags[3]);
    hdrip.ttl = 255;
    hdrip.protocol = IPPROTO_ICMP;

    //checksum entête IP
    hdrip.check = 0;
    //hdrip.check = checksum((uint16_t *)&hdrip,LONGHDR_IP);

    //Entête ICMP
    hdricmp.type = ICMP_ECHO;
    hdricmp.code = 0;
    hdricmp.un.echo.id = htons(4444);
    hdricmp.un.echo.sequence = htons(0);
    hdricmp.checksum = 0;

    //memcpy(paquet, &hdricmp, LONGHDR_IP);
    //memcpy((paquet+LONGHDR_IP), &hdricmp, LONGHDR_ICMP);

    //checksum entête ICMP
    //hdricmp.checksum = checksum((uint16_t *)(paquet+LONGHDR_IP), LONGHDR_ICMP);
    //memcpy((paquet+LONGHDR_IP), &hdricmp, LONGHDR_ICMP);

    //préparation de l'envoi du paquet
    memset (&my_addr, 0, sizeof (struct sockaddr_in));
    my_addr.sin_family = AF_INET;
    my_addr.sin_addr.s_addr = hdrip.saddr;//hdrip.dst.s_addr;

    sd = socket (AF_INET, SOCK_RAW, IPPROTO_ICMP); //IPPROTO_RAW
    //envoi du paquet
    if (sendto(sd, paquet, LONGHDR_IP+LONGHDR_ICMP, 0, (struct sockaddr *)&my_addr, sizeof(struct sockaddr))<0)
    {
      perror ("échec sendto()");
      exit(EXIT_FAILURE);
    }

    //fermeture socket
    close (sd);
	
    //free mémoire
    free (cible);
    free (myip);
    free (ipstr);
    free (ip_flags);
    free (paquet);

    return (EXIT_SUCCESS);
}

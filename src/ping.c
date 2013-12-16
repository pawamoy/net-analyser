/**\file ping.c
 * \author val
 * \brief ping main
 * \date December 10, 2013, 10:37 AM
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/ip_icmp.h>
#include <netinet/ip.h>
#include <netinet/in.h>
#include <arpa/inet.h>  
#include <netdb.h>
#include <errno.h>
#include <unistd.h>
#include "../include/traceroute.c"
//#include <netinet/udp.h>
//#include <netinet/tcp.h>

#define LONGHDR_IP 20
#define LONGHDR_ICMP 8

/*
 * TODO:
 * 	- utiliser le protocole ICMP afin d’envoyer, vers une machine ou un routeur, un message de type ECHO_REQUEST
 *	(réponse avec un ECHO_REPLY)
 *	- Les pertes ainsi que le RTT sont enregistrés
 *	- être capable d’envoyer des sondes ICMP, UDP et TCP et "interpréter les différents type de messages reçus"
 */

void Usage(void)
{
    printf("USAGE: ping servername\n");
    exit(-1);
}

int main(int argc, char** argv)
{
    int *ip_flags, sd;
    char *cible, *ipstr = NULL, *myip = NULL;
    struct ip hdrip;
    struct icmp hdricmp;
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
    hdrip.ip_hl = LONGHDR_IP/sizeof(uint32_t);
    hdrip.ip_v = 4;
    hdrip.ip_tos = 0;
    hdrip.ip_len = htons(LONGHDR_IP+LONGHDR_ICMP);
    hdrip.ip_id = htons(0);
    ip_flags[0] = 0;
    ip_flags[1] = 0;
    ip_flags[2] = 0;
    ip_flags[3] = 0;
    hdrip.ip_off = htons((ip_flags[0]<<15)+(ip_flags[1]<<14)+(ip_flags[2]<<13)+ip_flags[3]);
    hdrip.ip_ttl = 255;
    hdrip.ip_p = IPPROTO_ICMP;

    //checksum entête IP
    hdrip.ip_sum = 0;
    hdrip.ip_sum = checksum((uint16_t *)&hdrip,LONGHDR_IP);

    //Entête ICMP
    hdricmp.icmp_type = ICMP_ECHO;
    hdricmp.icmp_code = 0;
    hdricmp.icmp_id = htons(4444);
    hdricmp.icmp_seq = htons(0);
    hdricmp.icmp_cksum = 0;

    memcpy(paquet, &hdricmp, LONGHDR_IP);
    memcpy((paquet+LONGHDR_IP), &hdricmp, LONGHDR_ICMP);

    //checksum entête ICMP
    hdricmp.icmp_cksum = checksum((uint16_t *)(paquet+LONGHDR_IP), LONGHDR_ICMP);
    memcpy((paquet+LONGHDR_IP), &hdricmp, LONGHDR_ICMP);

    //préparation de l'envoi du paquet
    memset (&my_addr, 0, sizeof (struct sockaddr_in));
    my_addr.sin_family = AF_INET;
    my_addr.sin_addr.s_addr = hdrip.ip_dst.s_addr;

    sd = socket (AF_INET, SOCK_RAW, IPPROTO_RAW);
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

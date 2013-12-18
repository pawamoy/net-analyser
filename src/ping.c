/**\file ping.c
 * \author val
 * \brief ping functions
 * \date December 10, 2013, 10:37 AM
 */

#include "../include/ping.h"

void UsagePing(void)
{
    printf("USAGE: ping servername\n");
    exit(-1);
}

void handlerArret(int s){
           printf("%d\n",s);
	   printf("--- Affichage stats ping ---");
           exit(1); 
}

int main_ping(int argc, char* argv[])
{
    int *ip_flags, sd, *cptTransmis, *cptRecus;
    char /**cible,*/ *ipstr = NULL, *myip = NULL;
    struct iphdr hdrip;
    struct icmphdr hdricmp;
    uint8_t *paquet;
    struct addrinfo hints/*, *resolv*/;
    struct sockaddr_in /**server,*/ my_addr;
    struct sigaction sigIntHandler;

	//vérification arg
    if (argc != 2) UsagePing();   
	
    //vérif. root
    if (getuid())
    {
        fprintf(stderr, "\nError: you must be root to use raw sockets\n");
        exit(-1);
    }

	//allocation mémoire des diff. var
	//cible = (char *)malloc(40*sizeof(char));
        //memset(cible, 0, 40*sizeof(char));
	cptTransmis = (int *)malloc(4*sizeof(int));
        memset (cptTransmis, 0, 4*sizeof(int));
	cptRecus = (int *)malloc(4*sizeof(int));
        memset (cptRecus, 0, 4*sizeof(int));
	ipstr = (char *)malloc((INET_ADDRSTRLEN*sizeof(char)));
	memset(ipstr, 0, (INET_ADDRSTRLEN*sizeof(char)));
	myip = (char *)malloc((INET_ADDRSTRLEN*sizeof(char)));
	memset(ipstr, 0, (INET_ADDRSTRLEN*sizeof(char)));
	ip_flags = (int *)malloc(4*sizeof(int));
        memset (ip_flags, 0, 4*sizeof(int));
	paquet = (uint8_t *)malloc(IP_MAXPACKET*sizeof(uint8_t));
        memset (paquet, 0, IP_MAXPACKET*sizeof(uint8_t));
	

	//gestion Ctrl+C
   sigIntHandler.sa_handler = handlerArret;
   sigemptyset(&sigIntHandler.sa_mask);
   sigIntHandler.sa_flags = 0;

   sigaction(SIGINT, &sigIntHandler, NULL);


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
    } else {
	printf("Envoi du paquet ICMP réussi\n");
	printf("Envoi du paquet ICMP %u réussi\n", ntohs(hdricmp.un.echo.sequence));
	cptTransmis++;
    }
    //réception du paquet
    if (recvfrom(sd, paquet, LONGHDR_IP+LONGHDR_ICMP, 0, (struct sockaddr *)&my_addr, (socklen_t*)sizeof(struct sockaddr))<0)
    {
      perror ("échec recvfrom()");
      exit(EXIT_FAILURE);
    } else {
	printf("Réception du paquet ICMP %u réussie\n", ntohs(hdricmp.un.echo.sequence));
	cptRecus++;
    }

    //fermeture socket
    close (sd);
	
    //free mémoire
    //free (cible);
    free (myip);
    free (ipstr);
    free (ip_flags);
    free (paquet);

    return (EXIT_SUCCESS);
}

/**\file ping.c
 * \author val
 * \brief ping main
 * \date December 10, 2013, 10:37 AM
 */

#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/ip.h>
#include <netinet/in.h>
#include <netinet/udp.h>
#include <netinet/tcp.h>

void Usage(void)
{
    printf("USAGE: ping servername\n");
    exit(-1);
}

int main(int argc, char** argv)
{
	int status, datalen, *ip_flags;
    //const int on = 1;
    char *cible, *ipstr = NULL, *myip = NULL;;
    struct ip hdrip;
    struct icmp hdricmp;
    uint8_t *donnees, *paquet;
    struct addrinfo hints, *resolv;
    struct sockaddr_in *server, my_addr;

	//vérification arg
    if (argc != 2) Usage();   
	
	/*
	* --- allocation mémoire des diff. var ---
	*/
	
	//@IP à partir du domaine fourni en arg
    ipstr = GetIPFromHostname(argv[1]);
	//@source IPv4
    myip = GetMyIP();
	
	
	/* ENTETES
    //structure entête IP
    struct header_ip
    {
        unsigned char hip_longueur_entete_IP:5/*en octets//, hip_version:4;//4 = IPv4; 6 = IPv6
        unsigned char hip_type_de_service;//(ToS)
        unsigned short int hip_longueur;
        unsigned short int hip_id;
        unsigned char hip_flags;
        unsigned short int iph_offset;
        unsigned char hip_TTL;
        unsigned char hip_protocole;
        unsigned short int hip_checksum;
        unsigned int hip_source;
        unsigned int hip_destination;
    };

    //structure entête ICMP
    struct header_icmp
    {
        unsigned char hicmp_type;
        unsigned char hicmp_code;
        unsigned short int hicmp_checksum;
        unsigned short int hicmp_id;
        unsigned short int hicmp_numero_sequence;
    };

    //structure entête UDP
    struct header_UDP
    {
        unsigned short int hudp_port_source;
        unsigned short int hudp_port_destination;
        unsigned short int hudp_longueur;
        unsigned short int hudp_checksum;
    };

    //structure entête TCP
    struct header_TCP {
        unsigned short int htcp_port_source;
        unsigned short int htcp_port_destination;
        unsigned int htcp_numero_sequence;
        unsigned int htcp_numero_ACK;
        unsigned char htcp_reserve:4/*bits reservés//, htcp_offset:5;
        unsigned char htcp_flags;
        unsigned short int htcp_fenetre;
        unsigned short int htcp_checksum;
        unsigned short int htcp_pointeur_urgent;
    };
*/

    return (EXIT_SUCCESS);
}

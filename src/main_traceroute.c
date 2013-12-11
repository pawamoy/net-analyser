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
 */




int main(int argc, char** argv)
{
    Socket sockfd = 0;
    
//    struct sockaddr_in server;
//    socklen_t addrlen = 0;
    
//    int portno = 0;
//    int domain = AF_INET;
    //char *ipstr = NULL;

    FILE* logfile;

    // check the number of args on command line
    if (argc != 2) 
    {
        Usage();
    }
    
    // opens a log file
    logfile = OpenLog();
    assert(logfile != NULL);
    
    WriteLog(logfile, "Domain: ");
    WriteLogLF(logfile, argv[1]);

    //ipstr = GetIPFromHostname(argv[1]);
//    portno = 80;

    // init remote addr structure and other params
//    server.sin_family = domain;
//    server.sin_port = htons(portno);
    //addrlen = sizeof (struct sockaddr_in);
    
    //server.sin_addr.s_addr = GetIPFromHostname(argv[1]);


    

    int ttl = 0;
    
    for (ttl = 0; ttl < 16; ttl++)
    {
        sockfd = OpenRawSocket();
        
        printf("Connection OK\n");
        
        printf("Disconnection\n");
        
        close(sockfd);
    }
    
    CloseLog(logfile);

    return (EXIT_SUCCESS);
}

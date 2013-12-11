/**\file main_traceroute.c
 * \author tim
 * \brief traceroute main
 * \date December 10, 2013, 10:37 AM
 */
 
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
 */
int main(int argc, char** argv) {
    Socket sockfd;
    struct sockaddr_in server;
    socklen_t addrlen;
    int portno = 0;
    int domain = AF_INET;
    int status;
    struct addrinfo hints, *res, *p;
    char ipstr[INET6_ADDRSTRLEN];


    // check the number of args on command line
    if (argc != 2) 
    {
        Usage();
    }

    memset(&hints, 0, sizeof hints); // make sure the struct is empty
    hints.ai_family = AF_INET; // don't care IPv4 or IPv6
    hints.ai_socktype = SOCK_STREAM; // TCP stream sockets
    hints.ai_flags = AI_PASSIVE; // fill in my IP for me

    if ((status = getaddrinfo(argv[1], "80", &hints, &res)) != 0) 
    {
        fprintf(stderr, "getaddrinfo error: %s\n", gai_strerror(status));
        exit(1);
    }

    for (p = res; p != NULL; p = p->ai_next) 
    {
        void *addr;

        // get the pointer to the address itself,
        // different fields in IPv4 and IPv6:
        struct sockaddr_in *ipv4 = (struct sockaddr_in *) p->ai_addr;
        addr = &(ipv4->sin_addr);

        // convert the IP to a string and print it:
        inet_ntop(p->ai_family, addr, ipstr, sizeof ipstr);
        printf("%s\n", ipstr);
        break;
    }

    portno = 80;

    // init remote addr structure and other params
    server.sin_family = domain;
    server.sin_port = htons(portno);
    addrlen = sizeof (struct sockaddr_in);

    // get addr from command line and convert it
    if (inet_pton(domain, ipstr, &server.sin_addr.s_addr) != 1) 
    {
        perror("Cannot get addr from command line and convert it");
        //~ close(sockfd);
        exit(EXIT_FAILURE);
    }
    


    //~ char buf[1024] = {0};
    //~ int bytes_sent = 0;
    int ttl = 0;
    
    for (ttl = 16; ttl > 0; ttl--)
    {
        // socket factory
        if ((sockfd = socket(domain, SOCK_STREAM, IPPROTO_TCP)) == -1) {
            perror("Cannot create the TCP socket");
            exit(EXIT_FAILURE);
        }
        
        if (SetTTL(sockfd, ttl) == 0) 
        {
            perror("Cannot set TTL value\n");
            exit(EXIT_FAILURE);
        }

        printf("Trying to connect to the remote host; TTL: %d\n", ttl);
        if (connect(sockfd, (struct sockaddr*) &server, addrlen) == -1) {
            perror("Cannot connect to server");
            exit(EXIT_FAILURE);
        }

        printf("Connection OK\n");
        
        printf("Disconnection\n");

        // close the socket
        close(sockfd);
    }
    

//    sprintf(buf, "GET %s\n", "index.html");
//    printf("Cmd sent : %s\n", buf);
//
//    // send string
//    if ((bytes_sent = send(sockfd, buf, strlen(buf) + 1, 0)) == -1)
//    {
//        perror("Cannot send string");
//        close(sockfd);
//        exit(EXIT_FAILURE);
//    }
//    printf("Bytes sent with GET cmd : %d\n", bytes_sent);
//
//    memset(buf, 0, sizeof buf);
//
//    // send string
//    if ((bytes_sent = recv(sockfd, buf, 1024, 0)) == -1) 
//    {
//        perror("Cannot receive string");
//        close(sockfd);
//        exit(EXIT_FAILURE);
//    }
//
//    printf("%s", buf);


    freeaddrinfo(res);


    return (EXIT_SUCCESS);
}

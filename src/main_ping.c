/**\file main_ping.c
 * \brief ping main
 */

#include "../include/ping.h"

/*
 * TODO:
 * 	- utiliser le protocole ICMP afin d’envoyer, vers une machine ou un routeur, un message de type ECHO_REQUEST
 *	(réponse avec un ECHO_REPLY)
 *	- Les pertes ainsi que le RTT sont enregistrés
 *	- être capable d’envoyer des sondes ICMP, UDP et TCP et "interpréter les différents type de messages reçus"
 */

int main(int argc, char* argv[]) {
	//vérification arg
    if (argc != 2) UsagePing();   
	
    //vérif. root
    if (getuid())
    {
        fprintf(stderr, "\nError: you must be root to use raw sockets\n");
        exit(-1);
    }
    
	return main_ping(argv[1]);
}

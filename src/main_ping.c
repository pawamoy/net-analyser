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
	//-----------------------------------------------------//
	// variables declaration
	//-----------------------------------------------------//
    StrPing p = NewPing();
    int i;
    
    
	//-----------------------------------------------------//
	// verifications
	//-----------------------------------------------------//
	// usage
    if (argc != 2) UsagePing();   
	
    // non-root
    if (getuid() != 0)
    {
        fprintf(stderr, "\nError: you must be root to use raw sockets\n");
        exit(-1);
    }
    
	// argument analysis
    for (i=2; i<argc; i++)
    {
		     if (strcmp(argv[i], "-i") == 0 ||
		         strcmp(argv[i], "--frequency") == 0) {
					if (i+1<argc) {
						p.frequency = atoi(argv[i+1]); i++;
					} else {
						fprintf(stderr, "-i (frequency): missing value: INT>0\n");
						exit(-1);
					}
				}
		else if (strcmp(argv[i], "-t") == 0 ||
		         strcmp(argv[i], "--ttl") == 0) {
					if (i+1<argc) {
						p.ttl = atoi(argv[i+1]); i++;
					} else {
						fprintf(stderr, "-t (ttl): missing value: INT>0\n");
						exit(-1);
					}
				}
		else if (strcmp(argv[i], "-W") == 0 ||
		         strcmp(argv[i], "--wait") == 0) {
					if (i+1<argc) {
						p.wait = atoi(argv[i+1]); i++;
					} else {
						fprintf(stderr, "-W (wait): missing value: INT>0\n");
						exit(-1);
					}
				}
		else if (strcmp(argv[i], "-A") == 0 ||
		         strcmp(argv[i], "--attempts") == 0) {
					if (i+1<argc) {
						p.attempts = atoi(argv[i+1]); i++;
					} else {
						fprintf(stderr, "-A (attempts): missing value: INT>0\n");
						exit(-1);
					}
				}
		else if (strcmp(argv[i], "-d") == 0 ||
		         strcmp(argv[i], "--threshold") == 0) {
					if (i+1<argc) {
						p.threshold = atoi(argv[i+1]); i++;
					} else {
						fprintf(stderr, "-d (threshold): missing value: INT>0\n");
						exit(-1);
					}
				}
		else {
			fprintf(stderr, "%s: unknown option\n", argv[i]);
			UsagePing();
		}
	}
	
	// non-zero values
	switch (p.ttl && p.threshold && p.wait)
	{
		case 0:
			fprintf(stderr, "TTL, threshold and response waiting time MUST BE greater than 0 !\n");
			exit(-1);
		default:
			break;
	}
	
	//-----------------------------------------------------//
	// set some data and start ping
	//-----------------------------------------------------//
	p.address = argv[1];
    p.ipstr = GetIPFromHostname(argv[1]);
    p.myip = GetMyIP();
    
	return main_ping(p, NULL);
}

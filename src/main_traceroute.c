/**\file main_traceroute.c
 * \brief traceroute main
 * \date December 10, 2013, 10:37 AM
 */

#include "../include/traceroute.h"

/*
 * Transformer le nom de domaine fourni en adresse IP										DONE
 * SetTTL																					DONE
 * modif champ TTL sur socket																DONE
 * à 0, le routeur renvoie TTL exceeded														OSEF
 * Reverse-DNS pour avoir le nom de routeurs à partir de l'IP								DONE
 * support de TCP, UDP et ICMP																DONE: UDP & ICMP
 * plusieurs modes : pas (nb sauts), fréquence sonde, tentatives, temporisateurs			DONE (fréquence sonde ???)
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
 
int main(int argc, char* argv[]) {
	//-----------------------------------------------------//
	// variable declaration
	//-----------------------------------------------------//
	int   portno      = 80,
          min_ttl     = 1,
          max_ttl     = 30,
          hops        = 1,
          rcv_timeout = 3,
          snd_timeout = 3,
          attempt     = 3,
          //~ log_data    = 0,
          i;
          
    char *probe       = "icmp";
    
    
    //-----------------------------------------------------//
	// first verifications
	//-----------------------------------------------------//
	
    // check the number of args on command line
    if (argc < 2) UsageTraceroute();

    // check root privileges
    //~ uid = getuid();
    //~ setuid(uid);
    if (getuid()) {
        fprintf(stderr, "\nError: you must be root to use raw sockets\n");
        exit(-1);
    }
    
    // argument analysis
    for (i=2; i<argc; i++)
    {
		     if (strcmp(argv[i], "-m") == 0 ||
		         strcmp(argv[i], "--maxttl") == 0) {
					if (i+1<argc) {
						max_ttl = atoi(argv[i+1]); i++;
					} else {
						fprintf(stderr, "-m: missing value: INT>0\n");
						exit(-1);
					}
				}
		else if (strcmp(argv[i], "-n") == 0 ||
		         strcmp(argv[i], "--minttl") == 0) {
					if (i+1<argc) {
						min_ttl = atoi(argv[i+1]); i++;
					} else {
						fprintf(stderr, "-n: missing value: INT>0\n");
						exit(-1);
					}
				}
		else if (strcmp(argv[i], "-h") == 0 ||
		         strcmp(argv[i], "--hops") == 0) {
					if (i+1<argc) {
						hops = atoi(argv[i+1]); i++;
					} else {
						fprintf(stderr, "-h: missing value: INT>0\n");
						exit(-1);
					}
				}
		else if (strcmp(argv[i], "-r") == 0 ||
		         strcmp(argv[i], "--recv-timeout") == 0) {
					if (i+1<argc) {
						rcv_timeout = atoi(argv[i+1]); i++;
					} else {
						fprintf(stderr, "-r: missing value: INT>0\n");
						exit(-1);
					}
				}
		else if (strcmp(argv[i], "-s") == 0 ||
		         strcmp(argv[i], "--send-timeout") == 0) {
					if (i+1<argc) {
						snd_timeout = atoi(argv[i+1]); i++;
					} else {
						fprintf(stderr, "-s: missing value: INT>0\n");
						exit(-1);
					}
				}
		//~ else if (strcmp(argv[i], "-l") == 0 ||
		         //~ strcmp(argv[i], "--log") == 0) log_data = 1;
		else if (strcmp(argv[i], "-p") == 0 ||
		         strcmp(argv[i], "--port") == 0) {
					if (i+1<argc) {
						portno = atoi(argv[i+1]); i++;
					} else {
						fprintf(stderr, "-p: missing value: INT\n");
						exit(-1);
					}
				}
		else if (strcmp(argv[i], "-b") == 0 ||
		         strcmp(argv[i], "--probe") == 0) {
					if (i+1<argc) {
						probe = argv[i+1]; i++;
					} else {
						fprintf(stderr, "-b: missing value: ICMP|TCP|UDP\n");
						exit(-1);
					}
				}
		else if (strcmp(argv[i], "-a") == 0 ||
		         strcmp(argv[i], "--attempt") == 0) {
					if (i+1<argc) {
						attempt = atoi(argv[i+1]); i++;
					} else {
						fprintf(stderr, "-a: missing value: INT>0\n");
						exit(-1);
					}
				}
		else {
			fprintf(stderr, "%s: unknown option\n", argv[i]);
			UsageTraceroute();
		}
	}
	
	switch (min_ttl && max_ttl && hops && rcv_timeout && snd_timeout && attempt)
	{
		case 0:
			fprintf(stderr, "All TTL values (min, max, hops), Timers (recv, send) and Attempts MUST BE greater than 0 !\n");
			exit(-1);
		default:
			break;
	}
	
	switch (probe[0]) {
		case 'u':
		case 'i':
		case 't':
			break;
		default :
			fprintf(stderr, "%s: invalid probe method: use with 'udp', 'icmp' (default) or 'tcp'\n", probe);
			exit(-1);
	}
    
	return main_traceroute(argv[1], portno, min_ttl, max_ttl, hops, probe[0],
	                       rcv_timeout, snd_timeout, attempt/*, log_data*/);
}

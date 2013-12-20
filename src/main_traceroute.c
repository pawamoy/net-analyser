/**\file main_traceroute.c
 * \brief traceroute main
 */

#include "../include/traceroute.h"

/* Ressources internet:
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
	// variables declaration
	//-----------------------------------------------------//
    StrTraceRoute tr = NewTraceRoute();
    int i = 0;
    char *icmp  = "icmp";
    char *udp   = "udp" ;
    char *tcp   = "tcp" ;
    char *probe = icmp  ; // see NewTraceRoute for default probe
    
    
    //-----------------------------------------------------//
	// verifications
	//-----------------------------------------------------//
	// usage
	if (argc < 2) UsageTraceroute();
	
	// non-root
    if (getuid() != 0) {
        fprintf(stderr, "\nError: you must be root to use raw sockets\n");
        exit(-1);
    }
    
    // argument analysis
    for (i=2; i<argc; i++)
    {
		     if (strcmp(argv[i], "-m") == 0 ||
		         strcmp(argv[i], "--max-ttl") == 0) {
					if (i+1<argc) {
						tr.s.max_ttl = atoi(argv[i+1]); i++;
					} else {
						fprintf(stderr, "-m (max ttl): missing value: INT>0\n");
						exit(-1);
					}
				}
		else if (strcmp(argv[i], "-f") == 0 ||
		         strcmp(argv[i], "--first-ttl") == 0) {
					if (i+1<argc) {
						tr.s.min_ttl = atoi(argv[i+1]); i++;
					} else {
						fprintf(stderr, "-f (first ttl): missing value: INT>0\n");
						exit(-1);
					}
				}
		else if (strcmp(argv[i], "-h") == 0 ||
		         strcmp(argv[i], "--hops") == 0) {
					if (i+1<argc) {
						tr.s.hops = atoi(argv[i+1]); i++;
					} else {
						fprintf(stderr, "-h (hops): missing value: INT>0\n");
						exit(-1);
					}
				}
		else if (strcmp(argv[i], "-r") == 0 ||
		         strcmp(argv[i], "--recv-timer") == 0) {
					if (i+1<argc) {
						tr.s.rcvt = atoi(argv[i+1]); i++;
					} else {
						fprintf(stderr, "-r (receive timer): missing value: INT>0\n");
						exit(-1);
					}
				}
		else if (strcmp(argv[i], "-s") == 0 ||
		         strcmp(argv[i], "--send-timer") == 0) {
					if (i+1<argc) {
						tr.s.sndt = atoi(argv[i+1]); i++;
					} else {
						fprintf(stderr, "-s (send timer): missing value: INT>0\n");
						exit(-1);
					}
				}
		else if (strcmp(argv[i], "-p") == 0 ||
		         strcmp(argv[i], "--port") == 0) {
					if (i+1<argc) {
						tr.portno = atoi(argv[i+1]); i++;
					} else {
						fprintf(stderr, "-p (port): missing value: INT\n");
						exit(-1);
					}
				}
		else if (strcmp(argv[i], "-I") == 0 ||
		         strcmp(argv[i], "--icmp") == 0) {
					probe = icmp;
				}
		else if (strcmp(argv[i], "-U") == 0 ||
		         strcmp(argv[i], "--udp") == 0) {
					probe = udp;
				}
		else if (strcmp(argv[i], "-T") == 0 ||
		         strcmp(argv[i], "--tcp") == 0) {
					probe = tcp;
				}
		else if (strcmp(argv[i], "-a") == 0 ||
		         strcmp(argv[i], "--attempts") == 0) {
					if (i+1<argc) {
						tr.s.attempts = atoi(argv[i+1]); i++;
					} else {
						fprintf(stderr, "-a (attempts): missing value: INT>0\n");
						exit(-1);
					}
				}
		else {
			fprintf(stderr, "%s: unknown option\n", argv[i]);
			UsageTraceroute();
		}
	}
	
	// non-zero values
	if (tr.s.min_ttl<0 || tr.s.max_ttl<0 || tr.s.hops<0 || tr.s.rcvt<0 || tr.s.sndt<0 || tr.s.attempts<0)
	{
		fprintf(stderr, "All TTL values (min, max, hops), Timers (recv, send) and Attempts MUST BE greater than 0 !\n");
		exit(-1);
	}
	
	//-----------------------------------------------------//
	// set some data and start traceroute
	//-----------------------------------------------------//
	tr.s.probe = probe[0];
	tr.address = argv[1];
    tr.ipstr = GetIPFromHostname(argv[1]);
    tr.myip = GetMyIP();
    
	return main_traceroute(tr);
}

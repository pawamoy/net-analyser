/**\file main.c
 * \brief netanalyser (traceroute + ping)
 */

#include "../include/netanalyser.h" 

static int loop = 1;

int main(int argc, char* argv[]) {
	//-----------------------------------------------------//
	// variable declaration
	//-----------------------------------------------------//
	StrTraceRoute tr = NewTraceRoute();
	StrPing p        = NewPing();
	int i, best_ttl  = 0;
    char *icmp       = "icmp",
         *udp        = "udp" ,
         *tcp        = "tcp" ,
         *probe      = icmp  ; // see NewTraceRoute for default probe
	FILE* logfile    = NULL;
    struct sigaction action;
	
	
	//-----------------------------------------------------//
	// verifications
	//-----------------------------------------------------//
	//usage
	if (argc < 2) UsageNetAnalyser();
	
	// non-root
    if (getuid() != 0) {
        fprintf(stderr, "\nError: you must be root to use raw sockets\n");
        exit(-1);
    }
    
	// argument analysis
    for (i=2; i<argc; i++) {
		     if (strcmp(argv[i], "-m") == 0 ||
		         strcmp(argv[i], "--max-ttl") == 0) {
					if (i+1<argc) {
						tr.s.max_ttl = atoi(argv[i+1]); i++;
					} else {
						fprintf(stderr, "-m (traceroute max ttl): missing value: INT>0\n");
						exit(-1);
					}
				}
		else if (strcmp(argv[i], "-f") == 0 ||
		         strcmp(argv[i], "--first-ttl") == 0) {
					if (i+1<argc) {
						tr.s.min_ttl = atoi(argv[i+1]); i++;
					} else {
						fprintf(stderr, "-f (traceroute first ttl): missing value: INT>0\n");
						exit(-1);
					}
				}
		else if (strcmp(argv[i], "-h") == 0 ||
		         strcmp(argv[i], "--hops") == 0) {
					if (i+1<argc) {
						tr.s.hops = atoi(argv[i+1]); i++;
					} else {
						fprintf(stderr, "-h (traceroute hops): missing value: INT>0\n");
						exit(-1);
					}
				}
		else if (strcmp(argv[i], "-r") == 0 ||
		         strcmp(argv[i], "--recv-timer") == 0) {
					if (i+1<argc) {
						tr.s.rcvt = atoi(argv[i+1]); i++;
					} else {
						fprintf(stderr, "-r (traceroute receive timer): missing value: INT>0\n");
						exit(-1);
					}
				}
		else if (strcmp(argv[i], "-s") == 0 ||
		         strcmp(argv[i], "--send-timer") == 0) {
					if (i+1<argc) {
						tr.s.sndt = atoi(argv[i+1]); i++;
					} else {
						fprintf(stderr, "-s (traceroute send timer): missing value: INT>0\n");
						exit(-1);
					}
				}
		else if (strcmp(argv[i], "-p") == 0 ||
		         strcmp(argv[i], "--port") == 0) {
					if (i+1<argc) {
						tr.portno = atoi(argv[i+1]); i++;
					} else {
						fprintf(stderr, "-p (traceroute port): missing value: INT\n");
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
		         strcmp(argv[i], "--attempt") == 0) {
					if (i+1<argc) {
						tr.s.attempts = atoi(argv[i+1]); i++;
					} else {
						fprintf(stderr, "-a (traceroute attempts): missing value: INT>0\n");
						exit(-1);
					}
				}
		else if (strcmp(argv[i], "-i") == 0 ||
		         strcmp(argv[i], "--frequency") == 0) {
					if (i+1<argc) {
						p.frequency = atoi(argv[i+1]); i++;
					} else {
						fprintf(stderr, "-i (ping frequency): missing value: INT>0\n");
						exit(-1);
					}
				}
		else if (strcmp(argv[i], "-t") == 0 ||
		         strcmp(argv[i], "--ttl") == 0) {
					if (i+1<argc) {
						p.ttl = atoi(argv[i+1]); i++;
					} else {
						fprintf(stderr, "-t (ping ttl): missing value: INT>0\n");
						exit(-1);
					}
				}
		else if (strcmp(argv[i], "-W") == 0 ||
		         strcmp(argv[i], "--wait") == 0) {
					if (i+1<argc) {
						p.wait = atoi(argv[i+1]); i++;
					} else {
						fprintf(stderr, "-W (ping wait): missing value: INT>0\n");
						exit(-1);
					}
				}
		else if (strcmp(argv[i], "-A") == 0 ||
		         strcmp(argv[i], "--attempts") == 0) {
					if (i+1<argc) {
						p.attempts = atoi(argv[i+1]); i++;
					} else {
						fprintf(stderr, "-A (ping attempts): missing value: INT>0\n");
						exit(-1);
					}
				}
		else if (strcmp(argv[i], "-d") == 0 ||
		         strcmp(argv[i], "--threshold") == 0) {
					if (i+1<argc) {
						p.threshold = atoi(argv[i+1]); i++;
					} else {
						fprintf(stderr, "-d (ping delays threshold): missing value: INT>0\n");
						exit(-1);
					}
				}
		else {
			fprintf(stderr, "%s: unknown option\n", argv[i]);
			UsageTraceroute();
		}
	}
	
	// non-zero values
	switch (tr.s.min_ttl && tr.s.max_ttl && tr.s.hops && tr.s.rcvt && tr.s.sndt && tr.s.attempts) {
		case 0:
			fprintf(stderr, "Traceroute: All TTL values (min, max, hops), Timers (recv, send) and Attempts MUST BE greater than 0 !\n");
			exit(-1);
		default:
			break;
	}
	switch (p.ttl && p.threshold && p.wait)	{
		case 0:
			fprintf(stderr, "Ping: TTL, threshold and response waiting time MUST BE greater than 0 !\n");
			exit(-1);
		default:
			break;
	}
	
	
	//-----------------------------------------------------//
	// log file and get some infos
	//-----------------------------------------------------//
	logfile = OpenLog();
	if (logfile == NULL) exit(-1);
	tr.s.logfile = p.logfile = logfile;
	
	tr.s.probe = probe[0];
	
	tr.address = p.address = argv[1];
    tr.ipstr   = p.ipstr   = GetIPFromHostname(argv[1]);
    tr.myip    = p.myip    = GetMyIP();
    
	fprintf(logfile, "Domain: %s\n", tr.address);
	fprintf(logfile, "Resolved IP address: %s\n", tr.ipstr);
	fprintf(logfile, "My IP address: %s\n", tr.myip);
	

	//-----------------------------------------------------//
	// start program
	//-----------------------------------------------------//
	action.sa_handler = handler;
	sigemptyset(&action.sa_mask);
	action.sa_flags = 0;
	sigaction(SIGINT, &action, NULL);
	
	/* STEP 1: PING WITH BIG TTL (64+)
	 * the called function must return immediatly after joining the distant host
	 * we should provide a number of attempt before returning failure status
	 * no logs are written here
	 */
	if (HostIsJoinable(p.ipstr, p.myip, p.ttl, p.attempts))	{
		while (loop) {
			/* STEP 2: SIMPLE TRACEROUTE WITH BIG TTL(30)
			 * the called function should return the last TTL value, or -1 if failure
			 */
			best_ttl = main_traceroute(tr);
			
			if (best_ttl == -1) {
				// destination unreached but host was joinable
				if (HostIsJoinable(p.ipstr, p.myip, p.ttl, p.attempts))
					printf("Traceroute protocol issue\n");
				
				loop = 0;
			} else {
				/* STEP 3: launch infinite ping
				 * the function should return:
				 *  	DELAY when getting a variation beyond a threshold
				 *  	LOSS if some probes are lost, but destination is reached
				 *  		before attempts are done
				 *  	FAILURE if some probes are lost, and
				 *  		all attempts failed (destination unreached)
				 */
				switch (main_ping(p, &best_ttl)) {
					case DELAY:                          continue;
					case LOSS:    tr.s.max_ttl=best_ttl; continue;
					case FAILURE:
					default:      loop = 0;
				}
			}
			/* STEP 4: loop until destination can't be reach or SIGINT
			 */	
		}
	} else {
		printf("Host is not joinable\n");
	}
	
	
	//-----------------------------------------------------//
	// close log file
	//-----------------------------------------------------//
	CloseLog();
	
	return 0;
}

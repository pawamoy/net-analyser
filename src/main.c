/**\file main.c
 * \brief netanalyser (traceroute + ping)
 */

#include "../include/netanalyser.h" 

static int loop = 1;

int main(int argc, char* argv[]) {
	//-----------------------------------------------------//
	// variable declaration
	//-----------------------------------------------------//
	int best_ttl    = 0; // ttl minimal trouvé pour atteindre la destination
	int attempts    = 5; // tentatives en cas de perte pour ping (0=infini)
	int max_ping    = 64; // ttl vérification joignabilité de l'hôte distant
	int frequency   = 1; // fréquence inter-sonde pour ping
	int threshold   = 25;
	
	StrTraceRoute tr = NewTraceRoute();
	
    struct sigaction sigIntHandler;
    sigIntHandler.sa_handler = handler;
	sigemptyset(&sigIntHandler.sa_mask);
	sigIntHandler.sa_flags = 0;
	sigaction(SIGINT, &sigIntHandler, NULL);
	
	//-----------------------------------------------------//
	// first verifications
	//-----------------------------------------------------//
	if (argc != 2) exit(-1);
	/* analyse des arguments
	 * séparation arg ping et arg traceroute
	 * 
	 * all        : port
	 * ping       : frequency, threshold, attempts, timeouts, 
	 * traceroute : probe, attempts, timeouts, 
	 */
	
	//-----------------------------------------------------//
	// log file
	//-----------------------------------------------------//
	tr.s.logfile = OpenLog();
	if (tr.s.logfile == NULL) exit(-1);
	
	// get infos
	tr.address = argv[1];
    tr.ipstr = GetIPFromHostname(argv[1]);
    tr.myip = GetMyIP();
    
	fprintf(tr.s.logfile, "Domain: %s\n", tr.address);
	fprintf(tr.s.logfile, "Resolved IP address: %s\n", tr.ipstr);
	fprintf(tr.s.logfile, "My IP address: %s\n", tr.myip);
	

	//-----------------------------------------------------//
	// start program
	//-----------------------------------------------------//
	//~ printf("%d, %s\n", argc, *argv); // pour que ce pd de compilo fasse pas chier
	
	/* STEP 1: PING WITH BIG TTL (64+)
	 * the called function must return immediatly after joining the distant host
	 * we should provide a number of attempt before returning failure status
	 * no logs are written here
	 */
	if (HostIsJoinable(argv[1], max_ping, attempts))
	{
		while (loop)
		{
			/* STEP 2: SIMPLE TRACEROUTE WITH BIG TTL(30)
			 * the called function should return the last TTL value, or -1 if failure
			 */
			best_ttl = main_traceroute(tr);
			
			if (best_ttl == -1)
			{
				// destination unreached but host was joinable
				if (HostIsJoinable(argv[1], max_ping, attempts))
					printf("Traceroute protocol issue\n");
				
				loop = 0;
			}
			else
			{
				/* STEP 3: launch infinite ping
				 * the function should return:
				 *  	DELAY when getting a variation beyond a threshold
				 *  	LOSS if some probes are lost, but destination is reached
				 *  		before attempts are done
				 *  	FAILURE if some probes are lost, and
				 *  		all attempts failed (destination unreached)
				 */
				switch (ping(argv[1], threshold, frequency, attempts, &best_ttl))
				{
					case DELAY:                          continue;
					case LOSS:    tr.s.max_ttl=best_ttl; continue;
					case FAILURE:
					default:      loop = 0;
				}
			}
			/* STEP 4: loop until destination can't be reach or SIGINT
			 */	
		}
	}
	
	//-----------------------------------------------------//
	// close log file
	//-----------------------------------------------------//
	CloseLog();
	
	return 0;
}

/**\file main.c
 * \brief traceroute + ping (//)
 * \author tim
 * \date December 18, 2013, 15:57 AM
 */

#include "../include/ping.h"
#include "../include/traceroute.h"
#include "../include/log.h" 

#define DELAY   1
#define LOSS    2
#define FAILURE 3

/* principe:
 * ping pour vérifier la joignabilité de l'hôte
 * première découverte de route (simple affichage + permet de récup le ttl min) + log
 * puis on fait tourner ping (option de fréquence en ligne de commande) + log
 * changement de TTL / variation de délai -> re-découverte de route
 * on répète jusqu'à Ctrl-C : affichage de statistiques (voir vrai ping)
 *     du genre % perte, délai, min max moy, routes, etc...
 * estimation asymétrie entres routes allers et retours
 */

/* PSEUDO-CODE:
 * 
 * cur_ttl := 30
 * N := 5
 * 
 * if HostIsJoinable ( TTL:=64 )
 *  	begin loop
 *  		best_ttl := traceroute ( TTL:=cur_ttl )
 *  		if destination reached
 *  			while ping ( TTL:=best_ttl ) ;
 *  			case changed delay
 *  				continue loop
 *  			case packet loss
 *  				for N attempts
 *  					best_ttl++
 *  					if ping ( TTL:=best_ttl )
 *  						cur_ttl := best_ttl
 *  						continue loop
 * 						end if
 *  				end for
 *  				print info and exit
 * 				end case
 *  		else
 *  			if HostIsJoinable ( TTL:=64 )
 *  				print protocol problem
 *  			end if
 *  			print info and exit
 *  		end if
 * 		end loop
 * else
 *  	print info and exit
 * end if
 */

void ShowStatistics(void);
void ShowStatistics()
{
	printf("Statistics :D\n");
}

int ping(char* address, int threshold, int frequency, int attempts, int* best_ttl);
int ping(char* address, int threshold, int frequency, int attempts, int* best_ttl)
{
	int i;
	for (i=0; i<10; i++)
	{
		printf("Ping at %s with a threshold of %d%%, a frequency of %d, %d attempts if packets are lost, and a max ttl of %d\n",
			address, threshold, frequency, attempts, *best_ttl);
		sleep(frequency);
	}
	return DELAY;
}

int HostIsJoinable(char* address, int max_ping, int attempts);
int HostIsJoinable(char* address, int max_ping, int attempts)
{
	printf("Host %s is joinable (ttl=%d and attemps=%d)\n", address, max_ping, attempts);
	return 1;
}

int main(int argc, char* argv[]) {
	//-----------------------------------------------------//
	// variable declaration
	//-----------------------------------------------------//
	int best_ttl    = 0; // ttl minimal trouvé pour atteindre la destination
	int cur_ttl     = 30; // ttl pour traceroute
	int attempts    = 5; // tentatives en cas de perte pour ping (0=infini)
	int max_ping    = 64; // ttl vérification joignabilité de l'hôte distant
	int frequency   = 1; // fréquence inter-sonde pour ping
	int threshold   = 25;
	
	int portno = 80;
	char probe = 'i';
	int rcv_timeout = 1;
	int snd_timeout = 1;
	int attempts_t = 3;
	
    //~ struct sigaction sigIntHandler;
    //~ sigIntHandler.sa_handler = handler;
	//~ sigemptyset(&sigIntHandler.sa_mask);
	//~ sigIntHandler.sa_flags = 0;
	//~ sigaction(SIGINT, &sigIntHandler, NULL);
	
	//-----------------------------------------------------//
	// first verifications
	//-----------------------------------------------------//
	if (argc != 2) exit(-1);
	/* analyse des arguments
	 * séparation arg ping et arg traceroute
	 */
	 
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
		for (;;)
		{
			/* STEP 2: SIMPLE TRACEROUTE WITH BIG TTL(30)
			 * the called function should return the last TTL value, or -1 if failure
			 */
			best_ttl = main_traceroute(argv[1], portno, 1, cur_ttl, 1, probe,
	                                   rcv_timeout, snd_timeout, attempts_t);
			
			if (best_ttl == -1)
			{
				// destination unreached but host was joinable
				if (HostIsJoinable(argv[1], max_ping, attempts))
					printf("Traceroute protocol issue\n");
				
				ShowStatistics();
				exit(-1);
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
					case DELAY:                       continue;
					case LOSS:    cur_ttl = best_ttl; continue;
					case FAILURE:
					default:
						ShowStatistics();
						exit(-1);
				}
			}
			/* STEP 4: loop until destination can't be reach or SIGINT
			 */	
		}
	}
	else
	{
		ShowStatistics();
		exit(-1);
	}
	
	
	return 0;
}

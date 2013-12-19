/**\file main.c
 * \brief traceroute + ping (//)
 * \author tim
 * \date December 18, 2013, 15:57 AM
 */

#include "../include/ping.h"
#include "../include/traceroute.h"
#include "../include/log.h" 

int main(int argc, char* argv[]) {
	printf("%d, %s\n", argc, *argv); // pour que ce pd de compilo fasse pas chier
	
	/* analyse des arguments
	 * séparation arg ping et arg traceroute
	 */
	 
	/* principe:
	 * ping pour vérifier la joignabilité de l'hôte
	 * première découverte de route (simple affichage + permet de récup le ttl min) + log
	 * puis on fait tourner ping (option de fréquence en ligne de commande) + log
	 * changement de TTL / variation de délai -> re-découverte de route
	 * on répète jusqu'à Ctrl-C : affichage de statistiques (voir vrai ping)
	 *     du genre % perte, délai, min max moy, routes, etc...
	 * estimation asymétrie entres routes allers et retours
	 */
	 
	
	return 0;
}

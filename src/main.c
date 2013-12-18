/**\file main.c
 * \brief traceroute + ping (//)
 * \author tim
 * \date December 18, 2013, 15:57 AM
 */
 
//~ - vérif avec msg ICMP request que l'hote distant est joignable, puis traceroute
//~ - par défaut : ICMP request à intervalle régulier (opt cli)
//~ - log des délais (ping), TTL et erreurs
//~ - si changements (délais, erreurs) re-traceroute
//~ - handler SIGINT : résumé statistique/graphique de toutes les infos (routes + changements, délais, pertes...)
//~ - estimation asymétrie entres routes allers et retours

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
	 */
	 
	
	return 0;
}
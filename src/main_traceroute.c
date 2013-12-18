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
 
int main(int argc, char** argv) {
	return main_traceroute(argc, argv);
}

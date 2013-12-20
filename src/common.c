/**\file common.c
 * \brief ip realtive functions
 * \date December 18, 2013, 17:07
 */

#include "../include/common.h"

char* GetIPFromHostname(const char* hostname)
{
    struct hostent *he;
    struct in_addr **addr_list;
    int i;
         
    if ( (he = gethostbyname( hostname ) ) == NULL)
    {
        // get the host info
        herror("gethostbyname()");
        exit(-1);
    }
 
    addr_list = (struct in_addr **) he->h_addr_list;
     
    for(i = 0; addr_list[i] != NULL; i++)
    {
        //Return the first one;
        return inet_ntoa(*addr_list[i]);
    }
     
    exit(-1);
}

char* GetMyIP()
{
	struct ifaddrs *ifaddr, *ifa;
	int family, s;
	char* host = (char*)malloc(128*sizeof(char));

	if (getifaddrs(&ifaddr) == -1) {
	   perror("getifaddrs");
	   exit(EXIT_FAILURE);
	}

	/* Walk through linked list, maintaining head pointer so we
	  can free list later */

	for (ifa = ifaddr; ifa != NULL; ifa = ifa->ifa_next) {
	   if (ifa->ifa_addr == NULL)
		   continue;

	   family = ifa->ifa_addr->sa_family;

	   /* For an AF_INET* interface address, display the address */

	   if (family == AF_INET || family == AF_INET6) {
		   s = getnameinfo(ifa->ifa_addr,
				   (family == AF_INET) ? sizeof(struct sockaddr_in) :
										 sizeof(struct sockaddr_in6),
				   host, NI_MAXHOST, NULL, 0, NI_NUMERICHOST);
		   if (s != 0) {
			   printf("getnameinfo() failed: %s\n", gai_strerror(s));
			   exit(EXIT_FAILURE);
		   }
		   if (IsMyAddress(host) == 1)
		   {
				break;
		   }
		}
	}

	freeifaddrs(ifaddr);
	return host;
}

int IsMyAddress(char* addr)
{
	if (strcmp(addr, "127.0.0.1") == 0)
	{
		return 0;
	}
	
	if (strlen(addr) >= 8)
	{
		if (isdigit(addr[0]) &&
			isdigit(addr[1]) && (
			(
				isdigit(addr[2]) &&
				addr[3] == '.'
			) || addr[2] == '.'))
		{
			return 1;
		}
	}
	
	return 0;
}

char* GetHostNameFromIP(const char* ip)
{
	struct hostent *hent;
	struct in_addr addr;
	char* host = (char*)malloc(MAX_ADDRESS*sizeof(char));
	strcpy(host, ip);

	if (!inet_aton(ip, &addr))
		return host;

	if ((hent = gethostbyaddr((char *)&(addr.s_addr), sizeof(addr.s_addr), AF_INET)))
		strcpy(host, hent->h_name);

	return host;
}

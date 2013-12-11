/**\file log.c
 * \author tim
 * \brief functions to log informations
 * \date December 11, 2013, 15:01 AM
 */

#include "../include/log.h"

char* GetDate() {
	time_t rawtime;
	struct tm * timeinfo;
	char* date;
	char* space;

	time ( &rawtime );
	timeinfo = localtime ( &rawtime );
	date = asctime (timeinfo);
	
	while (1==1) {
		if ((space = strchr(date, ' ')) != NULL)
			*space = '-';
		else break;
	}
	
	return date;
}

FILE* OpenLog() {
	char logpath[1024];
	snprintf(logpath, 1024, "%s%s", LOGFILE, GetDate());
	return fopen(logpath, "w");
}

int CloseLog(FILE* logfile) {
	return fclose(logfile);
}

int WriteLogLF(FILE* logfile, char* string) {
	int ret = 0;
	if (string == NULL)
		ret = fprintf(logfile, "\n");
	else
		ret = fprintf(logfile, "%s\n", string); 
	
	return ret;
}

int WriteLog(FILE* logfile, char* string) {
	if (string != NULL)
		return fprintf(logfile, "%s", string); 
	
	return 0;
}



/**\file log.c
 * \brief functions to log informations
 */

#include "../include/log.h"

static FILE* logfile = NULL;

char* GetDate()
{
    time_t rawtime;
    struct tm * timeinfo;
    char *date, *space, *lf;

    time(&rawtime);
    timeinfo = localtime(&rawtime);
    date = asctime(timeinfo);

    while (1 == 1)
    {
        if ((space = strchr(date, ' ')) != NULL)
            *space = '-';
        else break;
    }
    if ((lf = strchr(date, '\n')) != NULL)
        *lf = '\0';

    return date;
}

FILE* OpenLog()
{
    char logpath[1024];
    snprintf(logpath, 1024, "%s%s%s", LOGFILE, GetDate(), "\0");
    logfile = fopen(logpath, "w");
    return logfile;
}

int CloseLog()
{
    return fclose(logfile);
}


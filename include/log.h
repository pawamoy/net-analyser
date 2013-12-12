/**\file log.h
 * \author tim
 * \brief functions to log informations (header)
 * \date December 11, 2013, 15:35 AM
 */

#ifndef __LOG_H
#define __LOG_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define LOGFILE "Log-"

/**\brief Returns date of the day like 'Wday-Mon-Day-Hour:Min:Sec-Year'
 * \return A char pointer to the date
 */
char* GetDate(void);

/**\brief Opens a log file in current working directory, named "Log-Date"
 * \see GetDate()
 * \return A FILE pointer to the opened log file
 */
FILE* OpenLog(void);

/**\brief Closes a log file
 * \param logfile The log file to close
 * \return fclose return code (success:0, failure:EOF)
 */
int CloseLog(FILE* logfile);

/**\brief Appends a string and a line feed in a log file.
 * If the string is NULL, writes a line feed
 * \param logfile The log file to write
 * \param string The string to append
 * \return N>0 (chars written on success), N<0 (writing error: ferror is set, encoding error: errno is set to EILSEQ)
 */
int WriteLogLF(FILE* logfile, char* string);

/**\brief Appends a string in a log file.
 * If the string is NULL, doesn't write anything
 * \param logfile The log file to write
 * \param string The string to append
 * \return N>=0 (chars written on success), N<0 (writing error: ferror is set, encoding error: errno is set to EILSEQ)
 */
int WriteLog(FILE* logfile, char* string);

#endif // __LOG_H

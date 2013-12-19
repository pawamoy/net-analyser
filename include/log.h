/**\file log.h
 * \brief functions to log informations (header)
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
 * \author tim
 */
char* GetDate(void);

/**\brief Opens a log file in current working directory, named "Log-Date"
 * \see GetDate()
 * \return A FILE pointer to the opened log file
 * \author tim
 */
FILE* OpenLog(void);

/**\brief Closes log file
 * \return fclose return code (success:0, failure:EOF)
 * \author tim
 */
int CloseLog(void);

#endif // __LOG_H

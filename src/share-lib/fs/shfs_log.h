

/*
 * @copyright
 *
 *  Copyright 2014 Neo Natura
 *
 *  This file is part of the Share Library.
 *  (https://github.com/neonatura/share)
 *        
 *  The Share Library is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version. 
 *
 *  The Share Library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with The Share Library.  If not, see <http://www.gnu.org/licenses/>.
 *
 *  @endcopyright
 *
*/  

#ifndef __FS__SHFS_LOG_H__
#define __FS__SHFS_LOG_H__

/**
 * @addtogroup libshare_fs
 * @{
 */


#define LOG_INFO 0
#define LOG_VERBOSE 1
#define LOG_TIMING 2
#define LOG_DEBUG 3
#define LOG_WARNING 4
#define LOG_ERROR 5
#define LOG_FATAL 6
#define MAX_LOG_LEVELS 7

#define MAX_LOG_SIZE 256
//#define MAX_LOG_SIZE 4096

#define MAX_LOG_TEXT_LENGTH 512

typedef struct shlog_t {

  /** LOG_XXX level of message. */
  int log_level;
  /** the time when the message was logged. */
  shtime_t log_stamp; 
  /** content of the message. */
  char log_text[MAX_LOG_TEXT_LENGTH];

} shlog_t;

int shlog(int level, char *msg);

int shlog_print(int lines, shbuf_t *buff);

void shlog_print_line(shbuf_t *buff, shlog_t *log, shtime_t *stamp_p);

char *shlog_level_label(int level);

#define shlog_info(_msg) \
  (shlog(LOG_INFO, (_msg)))

#define shlog_debug(_msg) \
  (shlog(LOG_DEBUG, (_msg)))

#define shlog_warn(_msg) \
  (shlog(LOG_WARNING, (_msg)))

#define shlog_err(_msg) \
  (shlog(LOG_ERROR, (_msg)))


/**
 * @}
 */

#endif /* ndef __FS__SHFS_LOG_H__ */



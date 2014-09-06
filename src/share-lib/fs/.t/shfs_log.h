

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



int shlog(int level, char *msg);

int shlog_print(int lines, shbuf_t *buff);

void shlog_print_line(shbuf_t *buff, shlog_t *log, shtime_t *stamp_p);

char *shlog_level_label(int level);



/**
 * @}
 */

#endif /* ndef __FS__SHFS_LOG_H__ */



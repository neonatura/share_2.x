
/*
 * @copyright
 *
 *  Copyright 2013 Brian Burrell 
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
 */  



#ifndef __SHFILE_H__
#define __SHFILE_H__

/**
 * Disk access routines.
 * @brief libshare_file Local file-system file operation routines.
 * @addtogroup libshare
 * @{
 */

/**
 * A reference to a local file-system path.
 */
typedef struct shfile_t shfile_t;

/**
 * A reference to a local file-system path.
 */
struct shfile_t {
/* if POSIX int fd; if STDIO FILE *fl, etc.. */
  size_t size;
};

/**
 * Generates a reference to a file on the local file-system.
 * @note Does not access the sharefs sub-system.
 */
shfile_t *shfile_init(char *path);

/**
 * Free's a reference to a file on the local file-system.
 * @see shfile_init()
 */
void shfile_free(shfile_t **file_p);

/**
 * @}
 */

#endif /* ndef __SHFILE_H__ */



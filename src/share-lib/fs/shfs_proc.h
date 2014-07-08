
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
 *
 */

#ifndef __FS__SHFS_PROC_H__
#define __FS__SHFS_PROC_H__

/**
 * @addtogroup libshare_fs
 * @{
 */

/**
 * Obtain an exclusive lock to a process with the same @c process_path and @c runtime_mode.
 * @param process_path The path to the process's executable file. (i.e. argv[0] in main() or static string)
 * @param runtime_mode An optional method to clarify between multiple process locks.
 * @returns A zero (0) on success and a negative one (-1) on failure. 
 */
int shfs_proc_lock(char *process_path, char *runtime_mode);

/**
 * @}
 */

#endif /* ndef __FS__SHFS_PROC_H__ */

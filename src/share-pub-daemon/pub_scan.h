
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
 *  @file share.h 
 *  @brief Public share synchronization daemon.
 *  @date 2014
 *
 *  Provides: Distribution of data in the "$HOME/share" directory.
 */  

#ifndef __SHARE_PUB_DAEMON__PUB_SCAN_H__
#define __SHARE_PUB_DAEMON__PUB_SCAN_H__
 


void pubd_scan_dir(pubuser_t *u, char *dir_path);

#endif /* ndef __SHARE_PUB_DAEMON__PUB_SCAN_H__ */


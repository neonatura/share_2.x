
/*
 * @copyright
 *
 *  Copyright 2013 Brian Burrell 
 *
 *  This file is part of the Share Library.
 *  (https://github.com/briburrell/share)
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

#ifndef __FS__SHFS_REV_H__
#define __FS__SHFS_REV_H__

/**
 * @addtogroup libshare_fs
 * @{
 */

/**
 * Describes a particular revision of a data segment.
 */
typedef struct shrev_s {
  /**
   * The parent delta of this revision. 
   */
  struct shrev_s *delta;

  /**
   * The machine on which the revision resides.
   * @note A value of @c SHFS_LOCAL_PEER refernces the current user.
   */ 
  shfs_peer peer; 

  /**
   * The sharefs journal number.
   */
  shfs_jno d_jno;
  shfs_ino d_ino;
} 

/**
 * @}
 */

#endif /* ndef __FS__SHFS_REV_H__ */



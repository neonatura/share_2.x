/*
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
 */

#ifndef __FS__SHFS_H__
#define __FS__SHFS_H__

#define SHFS_LEVEL_PUBLIC 0
#define SHFS_MAX_LEVELS 1

#ifndef NAME_MAX
#define NAME_MAX 4095
#endif


typedef struct shfs_node {
  long  d_ino;              /* inode number */
  off_t d_off;              /* offset to this old_linux_dirent */
  unsigned short d_reclen;  /* length of this d_name */
  char  d_name[NAME_MAX+1]; /* filename (null-terminated) */
} shfs_node;

typedef struct shfs_tree {
  shfs_node *root[SHFS_MAX_LEVELS];
  struct sockaddr *root_addr;
} shfs_tree;
#define shfs_public_root root[SHFS_LEVEL_PUBLIC]

struct shfs_tree *shfs_init(void);
struct shfs_node *shfs_node_entry(char *path);

#endif /* ndef __FS__SHFS_H__ */


/*
 *  Copyright 2013 Neo Natura
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
 */  

#include "share.h"
#include "sharetool.h"

int share_file_list(char *path, int pflags)
{
  shfs_t *tree;
  shfs_ino_t *file;
  shfs_ino_t *lfile;
  char *tok, tok_r;
  char buf[256];
  char *ptr;
  int type;
  int err;

  tree = shfs_init(NULL);
  if (!tree) {
    perror("shfs_init");
    return (SHERR_IO);
  }

  file = shfs_file_find(tree, path);
  if (!file) {
    perror("shfs_file_find");
    shfs_free(&tree);
    return (SHERR_NOENT);
  }

  if ((pflags & PFLAG_VERBOSE)) {
    memset(buf, 0, sizeof(buf));
    if (file->tree)
      strcpy(buf, shpeer_print(&file->tree->peer));
    if (file->blk.hdr.type == SHINODE_DIRECTORY) {
      printf("[%s \"%s\" @ %s]\n",
          shfs_inode_type(file->blk.hdr.type),
          shfs_inode_filename_get(file), buf);
    } else if (file->parent && IS_INODE_CONTAINER(file->blk.hdr.type)) {
      /* print parent header */
      printf("[%s \"%s\" @ %s]\n",
          shfs_inode_type(file->parent->blk.hdr.type),
          shfs_inode_filename_get(file->parent), buf);
    }
  }

  if (file->blk.hdr.type == SHINODE_DIRECTORY) {
    shbuf_t *buff;
    buff = shbuf_init();
    err = shfs_link_list(file, buff);
    if (shbuf_size(buff) != 0) {
      if ((pflags & PFLAG_VERBOSE)) {
        tok = strtok(shbuf_data(buff), "\n");
        while (tok) {
          ptr = strchr(tok, ' ');
          if (ptr) {
            type = atoi(tok);
            lfile = shfs_inode(file, ptr + 1, type);
            printf ("%s\n", shfs_inode_print(lfile));
          }

          tok = strtok(NULL, "\n");
        }
      } else {
        printf ("%s", shbuf_data(buff));
      }
    }
    shbuf_free(&buff);
    if (err) {
      perror("shfs_link_list");
      shfs_free(&tree);
      return (err);
    }
  } else {
    if ((pflags & PFLAG_VERBOSE)) {
      printf ("%s\n", shfs_inode_print(file));
    } else {
      printf("%s\n", shfs_inode_filename_get(file));
    }
  }

  shfs_free(&tree);
  return (0);
}





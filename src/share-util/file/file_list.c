
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

int share_file_list_container(shfs_ino_t *file, int level, int pflags)
{
  shbuf_t *buff;
  shfs_dirent_t *ents;
  shfs_dirent_t *ent;
  shfs_ino_t *lfile;
  int ent_tot;
  int i; 

  buff = shbuf_init();
  ent_tot = shfs_list(file, &ents);
  if (ent_tot < 0) {
    shfs_list_free(&ents);
    return (ent_tot);
  }
  for (i = 0; i < ent_tot; i++) {
    ent = (ents + i);
    if ((pflags & PFLAG_VERBOSE)) {
      lfile = shfs_inode(file, ent->d_name, ent->d_type);
      fprintf(sharetool_fout, "%-*.*s%s\n", 
          level, level, "", shfs_inode_print(lfile));
    } else {
      fprintf(sharetool_fout, "%-*.*s%s\n", 
          level, level, "", ent->d_name);
    }
  }
  shfs_list_free(&ents);

  return (0);
}

int share_file_list(char *path, int pflags)
{
  shfs_t *tree;
  shfs_ino_t *file;
  struct stat st;
  char *tok, tok_r;
  char buf[256];
  char *ptr;
  int err;

  file = sharetool_file(path, &tree);
  if (!file) {
    perror("shfs_file_find");
    shfs_free(&tree);
    return (SHERR_NOENT);
  }

  err = shfs_fstat(file, &st);
  if (err) {
    fprintf(stderr, "%s: cannot access %s: %s\n",
      process_path, path, sherr_str(err));
    shfs_free(&tree);
    return (err);
  }

  if ((pflags & PFLAG_VERBOSE)) {
    memset(buf, 0, sizeof(buf));
    if (file->tree)
      strcpy(buf, shpeer_print(&file->tree->peer));
    if (file->blk.hdr.type == SHINODE_DIRECTORY) {
      printf("[%s \"%s\" @ %s]\n",
          shfs_type_str(shfs_type(file)),
          shfs_inode_filename_get(file), buf);
    } else if (file->parent && IS_INODE_CONTAINER(file->blk.hdr.type)) {
      /* print parent header */
      printf("[%s \"%s\" @ %s]\n",
          shfs_type_str(shfs_type(file->parent)),
          shfs_inode_filename_get(file->parent), buf);
    }
  }

  if (file->blk.hdr.type == SHINODE_DIRECTORY) {
    share_file_list_container(file, 0, pflags);
  } else {
    if ((pflags & PFLAG_VERBOSE)) {
      fprintf(sharetool_fout, "%s\n", shfs_inode_print(file));
      if (IS_INODE_CONTAINER(shfs_type(file))) {
        share_file_list_container(file, 1, pflags);
      }
    } else {
      fprintf(sharetool_fout, "%s\n", shfs_inode_filename_get(file));
    }
  }

  shfs_free(&tree);
  return (0);
}





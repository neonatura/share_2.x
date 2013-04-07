
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

#include "share.h"

char *shfs_journal_path(shfs_t *tree, int index)
{
  static char ret_path[PATH_MAX+1];
  char *base_path;

  if (index < 0 || index >= SHFS_MAX_JOURNAL)
    return (NULL); /* invalid */

  base_path = shpref_base_dir();
  MKDIR(base_path);

  sprintf(ret_path, "%s/_t%u", base_path,
      shcrc(&tree->peer, sizeof(tree->peer)));
  MKDIR(ret_path);

  sprintf(ret_path + strlen(ret_path), "/_j%d", index);
  return (ret_path);
}

_TEST(shfs_journal_path)
{
  shfs_t *tree;
  char *path;
  int i;

  _TRUEPTR(tree = shfs_init(NULL, 0));
  for (i = 0; i < SHFS_MAX_JOURNAL; i++) {
    _TRUEPTR(path = shfs_journal_path(tree, i));
    if (path)
      _TRUE(0 != strlen(path));
  }
  shfs_free(&tree);

}

shfs_journal_t *shfs_journal_open(shfs_t *tree, int index)
{
  shfs_journal_t *j;
  struct stat st;
  char *path;
  char *data;
  int err;

  if (!tree)
    return (NULL); /* all done */

  j = (shfs_journal_t *)calloc(1, sizeof(shfs_journal_t));
  if (!j)
    return (NULL); /* woop woop */

  /* set journal index number. */
  j->index = index;

  path = shfs_journal_path(tree, index);
  err = stat(path, &st);
  if (!err && st.st_size != 0) { 
    err = shfs_read_mem(path, &data, &j->data_max); 
    if (err) {
      free(j); 
      return (NULL);
    }
    j->data = (shfs_journal_data_t *)data;
  } else {
    j->data_max = SHFS_BLOCK_SIZE;
    j->data = (shfs_journal_data_t *)calloc(j->data_max, sizeof(char));
  }
  if (!j->data) {
    free(j);
    return (NULL); /* woop woop */
  }

  return (j);
}

_TEST(shfs_journal_open)
{
  shfs_t *tree;
  shfs_journal_t *jrnl;
  int jno;

  _TRUEPTR(tree = shfs_init(NULL, 0));
  if (!tree)
    return;

  for (jno = 0; jno < SHFS_MAX_JOURNAL; jno++) {
    jrnl = shfs_journal_open(tree, jno);    
    _TRUEPTR(jrnl);
    _TRUEPTR(jrnl->data);
    _TRUE(jrnl->data_max != 0);
    _TRUE(jrnl->index == jno);
    shfs_journal_close(tree, &jrnl);
  }

  shfs_free(&tree);
}

int shfs_journal_write(shfs_t *tree, shfs_journal_t *jrnl)
{
  shfs_ino_t blank_inode;
  struct stat st;
  FILE *fl;
  char *path;
  char *data;
  ssize_t b_len;
  int err;

  if (!jrnl)
    return (0); /* all done */

  path = shfs_journal_path(tree, jrnl->index);
  fl = fopen(path, "wb+");
  if (!fl)
    return (-1);

  if (jrnl->data && jrnl->data_max >= SHFS_BLOCK_SIZE) {
    data = (char *)jrnl->data;
  } else {
    /* journal base marker. */
    memset(&blank_inode, 0, sizeof(blank_inode));
    data = (char *)&blank_inode;
  }

  /* sanity check */
  jrnl->data_max = MAX(SHFS_BLOCK_SIZE, jrnl->data_max);
  jrnl->data_max = MIN(jrnl->data_max, SHFS_MAX_JOURNAL_SIZE);

  b_len = fwrite(data, sizeof(char), jrnl->data_max, fl);
  if (b_len == -1) {
    fclose(fl);
    return (-1);
  }

  err = fclose(fl);
  if (err) 
    return (-1);

  return (0);
}

_TEST(shfs_journal_write)
{
  shfs_t *tree;
  shfs_journal_t *jrnl;
  int index;

  _TRUEPTR(tree = shfs_init(NULL, 0));
  if (!tree)
    return;

  for (index = 0; index < SHFS_MAX_JOURNAL; index++) {
    _TRUEPTR(jrnl = shfs_journal_open(tree, index));
    if (!jrnl)
      continue;

    _TRUE(!shfs_journal_write(tree, jrnl));
    shfs_journal_close(tree, &jrnl);
  }
}

int shfs_journal_close(shfs_t *tree, shfs_journal_t **jrnl_p)
{
  shfs_journal_t *jrnl;
  int ret_err;

  if (!tree || !jrnl_p)
    return (0); /* all done */

  jrnl = *jrnl_p;
  *jrnl_p = NULL;
  if (!jrnl)
    return (0); /* all done */

  ret_err = shfs_journal_write(tree, jrnl);

  free(jrnl->data);
  free(jrnl);

  return (ret_err);
}

int shfs_journal_scan(shfs_t *tree, int jno)
{
  shfs_journal_t *jrnl;
  shfs_ino_t *ino;
  int ino_nr;

  jrnl = shfs_journal_open(tree, jno);
  if (!jrnl)
    return (-1);

  for (ino_nr = 0; ino_nr < SHFS_MAX_BLOCK; ino_nr++) {
    ino = (shfs_ino_t *)jrnl->data->block[ino_nr];
    if (!ino->hdr.d_size)
      break; /* found empty inode */
  }

  shfs_journal_close(tree, &jrnl);

  if (ino_nr == SHFS_MAX_BLOCK)
    return (-1); /* none avail */

  return (ino_nr);
}

_TEST(shfs_journal_scan)
{
  int jno;

  for (jno = 0; jno < SHFS_MAX_JOURNAL; jno++) {
    
  }

}

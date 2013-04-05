
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

shfs_journal_t *shfs_journal(shfs_t *tree, int index)
{
  static shfs_journal_t *j;
  struct stat st;
  char path[PATH_MAX+1];
  FILE *fl;
  int err;

  if (j) { 
    shfs_journal_close(&j);
  }

  sprintf(path, ".shj%d", index);
  fl = fopen(path, "rb+");
  if (!fl)
    fl = fopen(path, "wb+");
  if (!fl)
    return (NULL);

  j = (shfs_journal_t *)calloc(1, sizeof(shfs_journal_t));
  if (!j)
    return (NULL); /* woop woop */

  j->data = (shfs_journal_data_t *)calloc(SHFS_MAX_JOURNAL_SIZE, sizeof(char));
  if (!j->data) {
    free(j);
    return (NULL); /* woop woop */
  }

  fstat(fileno(fl), &st);
  if (st.st_size < SHFS_MAX_JOURNAL_SIZE) {
    fwrite(j->data, SHFS_BLOCK_SIZE, SHFS_MAX_BLOCK, fl); 
    //fseek(fl, 0L, SEEK_SET);
  } else {
    fread(j->data, SHFS_BLOCK_SIZE, SHFS_MAX_BLOCK, fl);
  }

  err = fclose(fl);
  if (err) {
    free(j->data);
    free(j);
    return (NULL);
  }

  return (j);
}

int shfs_journal_write(shfs_journal_t *jrnl)
{
  static shfs_journal_t *j;
  struct stat st;
  char path[PATH_MAX+1];
  FILE *fl;
  int err;

  if (!jrnl || !jrnl->data)
    return (0); /* all done */

  sprintf(path, ".shj%d", index);
  fl = fopen(path, "wb+");
  if (!fl)
    return (-1);

  fwrite(j->data, SHFS_BLOCK_SIZE, SHFS_MAX_BLOCK, fl); 

  err = fclose(fl);
  if (err) 
    return (-1);

  return (0);
}

void shfs_journal_close(shfs_journal_t **jrnl_p)
{
  shfs_journal_t *jrnl;

  if (!jrnl_p)
    return;
  jrnl = *jrnl_p;
  *jrnl_p = NULL;

  shfs_journal_write(jrnl);

  free(jrnl->data);
  free(jrnl);

}

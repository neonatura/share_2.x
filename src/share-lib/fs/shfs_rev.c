
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

#include "share.h"

shfs_ino_t *shfs_rev_get(shfs_ino_t *repo, shkey_t *rev_key)
{
  shfs_ino_t *rev;

  if (0 == shkey_cmp(rev_key, ashkey_blank())) {
    return (NULL);
  }

  return (shfs_inode(repo, (char *)shkey_hex(rev_key), SHINODE_REVISION));
}

int shfs_rev_branch(shfs_ino_t *repo, char *name, shfs_ino_t *rev)
{
  shfs_ino_t *branch;
  shfs_ino_t *ref;
  char buf[SHFS_PATH_MAX];
  int err;

  if (!rev)
    return (SHERR_INVAL);

  memset(buf, 0, sizeof(buf));
  snprintf(buf, sizeof(buf)-1, "ref/head/%s", name);
  err = shfs_obj_set(repo, buf, &rev->blk.hdr.name);
  if (err)
    return (err);

  return (0);
}

shfs_ino_t *shfs_rev_branch_resolve(shfs_ino_t *repo, char *name)
{
  shfs_ino_t *branch;
  shfs_ino_t *ref;
  shkey_t *key;
  char buf[SHFS_PATH_MAX];
  int err;

  memset(buf, 0, sizeof(buf));
  snprintf(buf, sizeof(buf)-1, "ref/head/%s", name);
  key = shfs_obj_get(repo, buf);
  if (!key)
    return (NULL);

  branch = shfs_rev_get(repo, key);
  shkey_free(&key);

  return (branch);
}

int shfs_rev_tag(shfs_ino_t *repo, char *name, shfs_ino_t *rev)
{
  return (shfs_rev_branch(repo, name, rev));
} 

shfs_ino_t *shfs_rev_tag_resolve(shfs_ino_t *repo, char *name)
{
  return (shfs_rev_branch_resolve(repo, name));
}

int shfs_rev_init(shfs_ino_t *file)
{
  shfs_attr_t attr;
  shfs_ino_t *repo;
  shfs_ino_t *head;
  shfs_ino_t *rev;
  shfs_ino_t *tag;
  shfs_ino_t *commit_rev;
  int err;

  if (shfs_type(file) == SHINODE_DIRECTORY)
    return (0);

  attr = shfs_attr(file);
  if (attr & SHATTR_VER) {
    /* inode is already initialized for repository. */
    return (0);
  }

  /* create new repository for inode. */
  repo = shfs_inode(file, NULL, SHINODE_REPOSITORY);
  if (!repo)
    return (SHERR_IO);

  /* create 'head' initial revision. */
  rev = shfs_inode(repo, NULL, SHINODE_REVISION);
  if (!rev)
    return (SHERR_IO);

  /* create master branch */
  err = shfs_rev_branch(repo, "master", rev);
  if (err)
    return (err);

  /* assign master revision to HEAD tag. */
  err = shfs_rev_tag(repo, "HEAD", rev);
  if (err)
    return (err);

  /* assign commit revision to BASE tag. */
  err = shfs_rev_tag(repo, "BASE", rev);
  if (err)
    return (err);

  /* commit current data content */
  err = shfs_rev_commit(file, "initial revision", &commit_rev);
  if (err)
    return (err);

  return (0);
}

int shfs_rev_clear(shfs_ino_t *file)
{
  shfs_ino_t *repo;

  /* obtain repository for inode. */
  repo = shfs_inode(file, NULL, SHINODE_REPOSITORY);

  /* clear contents of repository. */
  return (shfs_inode_clear(repo));
}

shfs_ino_t *shfs_rev_base(shfs_ino_t *repo)
{
  return (shfs_rev_branch_resolve(repo, "BASE"));
}
int shfs_rev_base_set(shfs_ino_t *repo, shfs_ino_t *rev)
{
  return (shfs_rev_branch(repo, "BASE", rev)); 
}

char *shfs_rev_desc_get(shfs_ino_t *rev)
{
  return (shfs_meta_get(rev, SHMETA_DESC));
}

void shfs_rev_desc_set(shfs_ino_t *rev, char *desc)
{
  shfs_meta_set(rev, SHMETA_DESC, desc);
}

int shfs_rev_read(shfs_ino_t *rev, shbuf_t *buff)
{
  shfs_ino_t *aux;
  int err;

  aux = shfs_inode(rev, NULL, SHINODE_BINARY);
  if (!aux)
    return (SHERR_IO);

  err = shfs_aux_read(aux, buff);
  if (err)
    return (err);

  return (0);
}

int shfs_rev_write(shfs_ino_t *rev, shbuf_t *buff)
{
  shfs_ino_t *aux;
  int err;

  aux = shfs_inode(rev, NULL, SHINODE_BINARY);
  if (!aux)
    return (SHERR_IO);

  err = shfs_aux_write(aux, buff);
  if (err)
    return (err);

  return (0);
}

shfs_ino_t *shfs_rev_prev(shfs_ino_t *repo, shfs_ino_t *rev)
{
  shfs_ino_t *prev;
  shkey_t *key;
  const char *str;

  str = shfs_meta_get(rev, "repository.previous");
  key = shkey_hexgen(str);
  prev = shfs_rev_get(repo, key);
  shkey_free(&key);

  return (prev);
}

int shfs_rev_delta(shfs_ino_t *file, shfs_ino_t *rev, shbuf_t *diff_buff)
{
  struct stat st;
  shbuf_t *head_buff;
  shbuf_t *work_buff;
  shbuf_t *aux;
  shfs_ino_t *repo;
  shfs_ino_t *new_rev;
  shfs_ino_t *delta;
  shfs_t *fs;
  shkey_t *key;
  int err;

  if (!file)
    return (0); /* done */

  err = shfs_fstat(file, &st);
  if (err)
    return (0); /* done */

  /* obtain repository for file */
  repo = shfs_inode(file, NULL, SHINODE_REPOSITORY);
  if (!repo)
    return (SHERR_IO);

  if (!rev) {
    /* obtain current committed revision. */
    rev = shfs_rev_base(repo);
    if (!rev)
      return (SHERR_IO);
  }

  /* obtain working-copy data */
  head_buff = NULL;
  work_buff = shbuf_init();
  err = shfs_read(file, work_buff); 
  if (err)
    goto done;

  /* obtain work-data for BASE branch revision. */
  head_buff = shbuf_init();
  err = shfs_rev_read(rev, head_buff);
  if (err)
    goto done;

  if (shbuf_size(work_buff) == shbuf_size(head_buff) &&
      0 == memcmp(shbuf_data(work_buff), shbuf_data(head_buff), shbuf_size(work_buff))) {
    /* no difference */
    err = 0;
    goto done;
  }

  err = shdelta(work_buff, head_buff, diff_buff); 

done:
  shbuf_free(&work_buff);
  shbuf_free(&head_buff);

  return (err);
}

int shfs_rev_commit(shfs_ino_t *file, char *commit_desc, shfs_ino_t **rev_p)
{
  shbuf_t *diff_buff;
  shbuf_t *work_buff;
  shfs_ino_t *repo;
  shfs_ino_t *base;
  shfs_ino_t *new_rev;
  shfs_ino_t *delta;
  shfs_t *fs;
  int err;

  work_buff = diff_buff = NULL;

  /* obtain repository for file */
  repo = shfs_inode(file, NULL, SHINODE_REPOSITORY);
  if (!repo)
    return (SHERR_IO);

  base = shfs_rev_base(repo);
  if (!base)
    return (SHERR_IO);

  diff_buff = shbuf_init();
  err = shfs_rev_delta(file, base, diff_buff); 
  if (err)
    return (err);

  if (shbuf_size(diff_buff) == 0) {
    /* no modifications. */
    shbuf_free(&diff_buff);
    return (0);
  }

  /* create a new revision using previous revision's inode name */
  new_rev = shfs_inode(repo, shkey_hex(shfs_key(base)), SHINODE_REVISION); 
  if (!new_rev) {
    err = SHERR_IO;
    goto done;
  }

  /* define revision's meta information. */
  if (commit_desc)
    shfs_meta_set(new_rev, SHMETA_DESC, commit_desc);
  shfs_meta_set(new_rev, SHMETA_USER_NAME, shpref_get(SHMETA_USER_NAME, ""));
  shfs_meta_set(new_rev, SHMETA_USER_EMAIL, shpref_get(SHMETA_USER_EMAIL, ""));
  shfs_meta_set(new_rev, "repository.previous", shfs_filename(base));

  /* save delta to new revision */
  delta = shfs_inode(new_rev, NULL, SHINODE_DELTA);
  err = shfs_aux_write(delta, diff_buff); 
  shbuf_free(&diff_buff);
  if (err)
    goto done;

  /* save work-data to new revision. */
  work_buff = shbuf_init();
  err = shfs_read(file, work_buff); 
  if (err)
    goto done;
  err = shfs_rev_write(new_rev, work_buff); 
  shbuf_free(&work_buff);
  if (err)
    goto done;

  /* save new revision as BASE branch head */
  err = shfs_rev_base_set(repo, new_rev);
  if (err)
    goto done;

  /* tag previous revision. */
  shfs_rev_tag(repo, "PREV", base);


#if 0
/* DEBUG: TODO: need to dynamically create this from shfs_rev_read() */
  /* clear work-data on previous revision. */
  aux = shfs_inode(rev, NULL, SHINODE_BINARY);
  shfs_inode_clear(aux);
#endif

  if (rev_p)
    *rev_p = new_rev;

done:
  shbuf_free(&work_buff);
  shbuf_free(&diff_buff);

  return (err);
}

int shfs_rev_cat(shfs_ino_t *file, shkey_t *rev_key, shbuf_t *buff, shfs_ino_t **rev_p)
{
  shfs_ino_t *repo;
  shfs_ino_t *rev;
  int err;

  /* obtain repository for inode. */
  repo = shfs_inode(file, NULL, SHINODE_REPOSITORY);
  if (!repo)
    return (SHERR_IO);

  if (!rev_key) {
    /* obtain current revision */
    rev = shfs_rev_base(repo);
    if (!rev)
      return (SHERR_IO);
  } else {
    rev = shfs_rev_get(repo, rev_key);
  }

  err = shfs_rev_read(rev, buff);
  if (err)
    return (err);

  if (rev_p)
    *rev_p = rev;

  return (0);
}


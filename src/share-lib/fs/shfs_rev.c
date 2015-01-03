
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

int shfs_rev_branch(shfs_ino_t *repo, char *name, shfs_ino_t *rev, shfs_ino_t **branch_p)
{
  shfs_ino_t *branch;
  shfs_ino_t *ref;
  shkey_t *key;
  char buf[SHFS_PATH_MAX];
  int err;

  memset(buf, 0, sizeof(buf));
  snprintf(buf, sizeof(buf)-1, "ref/head/%s", name);

  branch = shfs_inode(repo, buf, SHINODE_OBJECT);
  ref = shfs_inode(branch, NULL, SHINODE_OBJECT_KEY);

  key = shfs_key(rev);
  if (!key)
    key = ashkey_blank();
  err = shfs_objkey_set(ref, key);
  if (err)
    return (err);

  if (branch_p)
    *branch_p = branch;

  return (0);
}

shfs_ino_t *shfs_rev_branch_resolve(shfs_ino_t *repo, char *name)
{
  shfs_ino_t *branch;
  shfs_ino_t *ref;
  shkey_t key;
  char buf[SHFS_PATH_MAX];
  int err;

  memset(buf, 0, sizeof(buf));
  snprintf(buf, sizeof(buf)-1, "ref/head/%s", name);
  branch = shfs_inode(repo, buf, SHINODE_OBJECT);
  ref = shfs_inode(branch, NULL, SHINODE_OBJECT_KEY);

  memset(&key, 0, sizeof(key));
  err = shfs_objkey_get(ref, &key); 
  if (err)
    return (NULL);

  return (shfs_rev_get(repo, &key));
}

int shfs_rev_tag(shfs_ino_t *repo, char *name, shfs_ino_t *rev, shfs_ino_t **tag_p)
{
  shfs_ino_t *tag;
  shfs_ino_t *ref;
  char buf[SHFS_PATH_MAX];

  memset(buf, 0, sizeof(buf));
  snprintf(buf, sizeof(buf)-1, "ref/tags/%s", name);
  tag = shfs_inode(repo, buf, SHINODE_OBJECT);
  ref = shfs_inode(tag, NULL, SHINODE_OBJECT_KEY);
  shfs_objkey_set(ref, shfs_key(rev));

  if (tag_p)
    *tag_p = tag;

  return (0);
} 

shfs_ino_t *shfs_rev_tag_resolve(shfs_ino_t *repo, char *name)
{
  shfs_ino_t *tag;
  shfs_ino_t *ref;
  shkey_t key;
  char buf[SHFS_PATH_MAX];
  int err;

  memset(buf, 0, sizeof(buf));
  snprintf(buf, sizeof(buf)-1, "ref/tags/%s", name);
  tag = shfs_inode(repo, buf, SHINODE_OBJECT);
  ref = shfs_inode(tag, NULL, SHINODE_OBJECT_KEY);

  memset(&key, 0, sizeof(key));
  err = shfs_objkey_get(ref, &key); 
  if (err)
    return (NULL);

  return (shfs_rev_get(repo, &key));
}

int shfs_rev_init(shfs_ino_t *file)
{
  shfs_attr_t attr;
  shfs_ino_t *repo;
  shfs_ino_t *head;
  shfs_ino_t *tag;
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

  /* create master branch */
  err = shfs_rev_branch(repo, "master", NULL, &head);
  if (err)
    return (err);
fprintf(stderr, "DEBUG: created 'master' branch '%s'\n", shkey_hex(shfs_key(head)));

  /* assign master revision to HEAD tag. */
  err = shfs_rev_tag(repo, "HEAD", head, &tag);
  if (err)
    return (err);

  /* assign commit revision to BASE tag. */
  err = shfs_rev_tag(repo, "BASE", head, &tag);
  if (err)
    return (err);

fprintf(stderr, "DEBUG: created BASE tag '%s'\n", shkey_hex(shfs_key(tag)));

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
  return (shfs_rev_branch(repo, "BASE", rev, NULL)); 
}

unsigned char *shfs_rev_desc_get(shfs_ino_t *rev)
{
  return (shfs_meta_get(rev, SHMETA_DESC));
}

void shfs_rev_desc_set(shfs_ino_t *rev, char *desc)
{
  shfs_meta_set(rev, SHMETA_DESC, desc);
}

unsigned char *shfs_rev_read(shfs_ino_t *rev)
{
  shfs_ino_t *aux;
  shbuf_t *buff;
  int err;

  aux = shfs_inode(rev, NULL, SHINODE_BINARY);
  if (!aux)
    return (NULL);

  buff = shbuf_init();
  err = shfs_aux_read(aux, buff);
  if (err || shbuf_size(buff) == 0) {
    shbuf_free(&buff);
    return (NULL);
  }

  return (shbuf_unmap(buff));
}

int shfs_rev_write(shfs_ino_t *rev, char *data)
{
  shfs_ino_t *aux;
  shbuf_t *buff;
  int err;

  aux = shfs_inode(rev, NULL, SHINODE_BINARY);
  if (!aux)
    return (SHERR_IO);

  buff = shbuf_init();
  shbuf_cat(buff, data, strlen(data));
  err = shfs_aux_write(aux, buff);
  shbuf_free(&buff);

  return (err);
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



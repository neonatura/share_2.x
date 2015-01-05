
/*
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
 */  

#include "sharetool.h"

int share_file_revision_status(revop_t *r, shfs_ino_t *file, int pflags)
{
  shbuf_t *buff;
  int err;

  buff = shbuf_init();
  err = shfs_rev_delta(file, NULL, buff); 
  if (err)
    return (err);

  if (shbuf_size(buff) == 0)
    return (0);

  /* print contents of file with header */
  fprintf(sharetool_fout, "\t%s\n", shfs_filename(file));

  return (0);
}

int share_file_revision_revert(revop_t *r, shfs_ino_t *file, int pflags)
{
  shfs_ino_t *repo;
  shfs_ino_t *rev;
  shbuf_t *buff;
  int err;

  repo = shfs_inode(file, NULL, SHINODE_REPOSITORY);
  if (!repo)
    return (SHERR_IO);

  rev = shfs_rev_base(repo);
  if (!rev)
    return (SHERR_IO);

  buff = shbuf_init();
  err = shfs_rev_read(rev, buff);
  if (err) {
    shbuf_free(&buff);
    return (err);
  }

  err = shfs_write(file, buff);
  shbuf_free(&buff);
  if (err)
    return (err);

  /* print contents of file with header */
  fprintf(sharetool_fout, "switched %s: branch 'BASE' (%s)\n",
      shfs_filename(file), shfs_filename(rev));

  return (0);
}

/**
 * Checkout a branch by name.
 */
int share_file_revision_switch(revop_t *r, char *ref_name, shfs_ino_t *file, int pflags)
{
  shfs_ino_t *repo;
  shfs_ino_t *base;
  shfs_ino_t *branch;
  shbuf_t *head_buff;
  int err;

  if (!ref_name)
    ref_name = "master";

  /* obtain repository for inode. */
  repo = shfs_inode(file, NULL, SHINODE_REPOSITORY);
  if (!repo)
    return (SHERR_IO);

  branch = shfs_rev_branch_resolve(file, ref_name);
  if (!branch)
    return (SHERR_IO);

  err = shfs_rev_base_set(repo, branch);
  if (err)
    return (err);
  
  /* print contents of file with header */
  fprintf(sharetool_fout, "switched %s: branch '%s' (%s)\n",
      shfs_filename(file), ref_name, shkey_hex(shfs_key(base)));

  return (0);
}

int share_file_revision_checkout(revop_t *r, shfs_ino_t *file, shkey_t *key, int pflags)
{
  shfs_ino_t *repo;
  shfs_ino_t *base;
  shfs_ino_t *rev;
  shbuf_t *head_buff;
  int err;

  if (!key)
    return (share_file_revision_switch(r, NULL, file, pflags)); 

  /* obtain repository for inode. */
  repo = shfs_inode(file, NULL, SHINODE_REPOSITORY);
  if (!repo)
    return (SHERR_IO);

  rev = shfs_rev_get(repo, key);
  if (!rev)
    return (SHERR_IO);

  err = shfs_rev_base_set(repo, rev);
  if (err)
    return (err);
  
  /* print contents of file with header */
  fprintf(sharetool_fout, "switched %s: %s\n",
      shfs_filename(file), shkey_hex(shfs_key(base)));

  return (0);
}

int share_file_revision_cat(revop_t *r, shfs_ino_t *file, shkey_t *key, int pflags)
{
  shfs_ino_t *rev;
  shbuf_t *buff;
  int err;

  buff = shbuf_init();
  err = shfs_rev_cat(file, key, buff, &rev);
if (key)
fprintf(stderr, "DEBUG: share_file_revision_cat %d = shfs_rev_cat(file:%s key:%s)\n", err, shfs_filename(file), shkey_hex(key));
else
fprintf(stderr, "DEBUG: share_file_revision_cat %d = shfs_rev_cat(file:%s key:<null>)\n", err, shfs_filename(file));
  if (err) {
    shbuf_free(&buff);
    return (err);
  }
  
  /* print contents of file with header */
  fprintf(sharetool_fout, "index %s (%s)\n",
      shfs_filename(file), shfs_filename(rev));
  fwrite(shbuf_data(buff), sizeof(char), shbuf_size(buff), sharetool_fout);

  shbuf_free(&buff);
  return (0);
}

int share_file_revision_branch(revop_t *r, char *name, shfs_ino_t *file, int pflags)
{
  shfs_ino_t *repo;
  shfs_ino_t *base;
  shfs_ino_t *branch;
  int err;

  /* obtain repository for inode. */
  repo = shfs_inode(file, NULL, SHINODE_REPOSITORY);
  if (!repo)
    return (SHERR_IO);

  /* obtain current revision */
  base = shfs_rev_base(repo);
  if (!base)
    return (SHERR_IO);

  err = shfs_rev_branch(file, name, base);
  if (err)
    return (err);

  fprintf(sharetool_fout, "%s: branch '%s' (%s)\n", 
      shfs_filename(file), name, shkey_hex(shfs_key(base)));

  return (0);
}

int share_file_revision_tag(revop_t *r, char *name, shfs_ino_t *file, int pflags)
{
  shfs_ino_t *repo;
  shfs_ino_t *base;
  shfs_ino_t *rev;
  int err;

  /* obtain repository for inode. */
  repo = shfs_inode(file, NULL, SHINODE_REPOSITORY);
  if (!repo)
    return (SHERR_IO);

  /* obtain current revision */
  base = shfs_rev_base(repo);
  if (!base)
    return (SHERR_IO);

  err = shfs_rev_tag(repo, name, base);
  if (err)
    return (err);

  fprintf(sharetool_fout, "%s: tag '%s' (%s)\n", 
    shfs_filename(file), name, shfs_filename(rev));

  return (0);
}

int share_file_revision_print(revop_t *r, shfs_ino_t *rev)
{
  char rev_name[MAX_SHARE_NAME_LENGTH];
  char rev_email[MAX_SHARE_NAME_LENGTH];
  char *desc;

  /* hash signature of commit revision. */
  fprintf(sharetool_fout, "commit %s\n", shfs_filename(rev));

  /* repository credentials. */
  strncpy(rev_name, shfs_meta_get(rev, "user.name"), sizeof(rev_name) - 1);
  strncpy(rev_email, shfs_meta_get(rev, "user.email"), sizeof(rev_email) - 1);
  fprintf(sharetool_fout, "Author: %s <%s>\n", rev_name, rev_email);

  /* revision commit time-stamp */
  fprintf(sharetool_fout, "Date: %s\n", shctime64(rev->blk.hdr.ctime));

  /* a checksum to verify integrity */
  fprintf(sharetool_fout, "Checksum: %s\n", shcrcstr(rev->blk.hdr.crc));

  desc = shfs_rev_desc_get(rev);
  if (desc && *desc)
    fprintf(sharetool_fout, "\t%s\n", desc);
}

int share_file_revision_log(revop_t *r, shfs_ino_t *file, shkey_t *key, int pflags)
{
  shfs_ino_t *repo;
  shfs_ino_t *rev;
  int err;

  repo = shfs_inode(file, NULL, SHINODE_REPOSITORY);
  if (!repo)
    return (SHERR_IO);

  if (key) {
    rev = shfs_rev_get(repo, key);
    if (!rev)
      return (SHERR_NOENT);

    share_file_revision_print(r, rev);
    return (0);
  }

  rev = shfs_rev_base(repo);
  while (rev) {
    share_file_revision_print(r, rev);
    rev = shfs_rev_prev(repo, rev);
  }

  return (0);
}

int share_file_revision_commit(revop_t *r, shfs_ino_t *file, int pflags)
{
  shfs_ino_t *rev;
  int err;

  err = shfs_rev_commit(file, &rev);
  if (err)
    return (err);

  fprintf(sharetool_fout, "\tcommit %s: %s\n", 
      shfs_filename(file), shfs_filename(rev));

  return (0);
}

int share_file_revision_diff(revop_t *r, shfs_ino_t *file, shkey_t *rev_key, int pflags)
{
  struct stat st;
  shbuf_t *work_buff;
  shbuf_t *head_buff;
  shbuf_t *diff_buff;
  shbuf_t *aux;
  shfs_ino_t *repo;
  shfs_ino_t *new_rev;
  shfs_ino_t *delta;
  shfs_ino_t *rev;
  shfs_t *fs;
  int err;

  work_buff = shbuf_init();
  err = shfs_read(file, work_buff); 
  if (err) {
    shbuf_free(&work_buff);
    return (err);
  }

  /* obtain repository for file */
  repo = shfs_inode(file, NULL, SHINODE_REPOSITORY);
  if (!repo)
    return (SHERR_IO);

  if (!rev_key) {
    /* obtain current committed revision. */
    rev = shfs_rev_base(repo);
  } else {
    rev = shfs_rev_get(repo, rev_key);
  }
  if (!rev)
    return (SHERR_NOENT);

  /* obtain work-data for BASE branch revision. */
  head_buff = shbuf_init();
  err = shfs_rev_read(rev, head_buff);
  if (err)
    goto done;

  if (shbuf_size(work_buff) == shbuf_size(head_buff) &&
      0 == memcmp(shbuf_data(work_buff), shbuf_data(head_buff), shbuf_size(work_buff))) {
    /* no difference to report */
    return (0);
  }

  diff_buff = shbuf_init();
  err = shdiff(diff_buff, shbuf_data(work_buff), shbuf_data(head_buff));
  shbuf_free(&work_buff);
  shbuf_free(&head_buff);
  if (err)
    goto done;

  fwrite(shbuf_data(diff_buff), sizeof(char), 
      shbuf_size(diff_buff), sharetool_fout); 

done:
  shbuf_free(&work_buff);
  shbuf_free(&diff_buff);
  shbuf_free(&head_buff);

  return (err);
}

int share_file_revision_command(revop_t *r, char **args, int arg_cnt, int pflags)
{
#define MAX_FL_CNT 256
  shfs_ino_t **fl_spec;
  shfs_t **fl_fs;
  struct stat st;
  shfs_t *fs;
  shfs_ino_t *dir;
  shkey_t *s_hash;
  shkey_t *e_hash;
  shkey_t *key;
  char *ref_name;
  int fl_cnt;
  int err;
  int i;

fprintf(stderr, "DEBUG: share_file_revision_command()\n");

  ref_name = NULL;
  if (r->cmd == REV_BRANCH || r->cmd == REV_TAG || r->cmd == REV_CHECKOUT) {
    ref_name = args[0]; 
    args++;
    arg_cnt--;
  }

  fl_spec = (shfs_ino_t **)calloc(arg_cnt+256, sizeof(shfs_ino_t *));
  fl_fs = (shfs_t **)calloc(arg_cnt+256, sizeof(shfs_t *));

  fl_cnt = 0;
  s_hash = NULL;
  e_hash = NULL;
  for (i = 0; i < arg_cnt && fl_cnt < (MAX_FL_CNT-1); i++) {
    if (args[i][0] == '@' && strlen(args[i]) == 49) {
      key = shkey_hexgen(args[i]);
      if (!shkey_cmp(key, ashkey_blank())) {
        if (!s_hash)
          s_hash = key;
        else
          e_hash = key;
        continue;
      }
      shkey_free(&key);
    }

    fl_spec[fl_cnt] = sharetool_file(args[i], &fl_fs[fl_cnt]);
    fl_cnt++;
  }

  if (fl_cnt == 0) {
    DIR *dir;
    struct dirent *ent;
    char cwd[PATH_MAX+1];

    /* default to current directory. */
    memset(cwd, 0, sizeof(cwd));
    getcwd(cwd, sizeof(cwd) - 1);
    dir = opendir(cwd);
    if (dir) {
      while (ent = readdir(dir)) {
        if (0 == strcmp(ent->d_name, ".") ||
            0 == strcmp(ent->d_name, ".."))
          continue;

        fl_spec[fl_cnt] = sharetool_file(ent->d_name, &fl_fs[fl_cnt]);
        fl_cnt++;

        if (fl_cnt >= 256)
          break;
      }
      closedir(dir);
    }
  }

  for (i = 0; i < fl_cnt; i++) {
    err = shfs_fstat(fl_spec[i], &st);
    if (err) {
      fprintf(stderr, "%s: cannot access %s: %s.\n", process_path, args[i], sherr_str(err)); 
      fl_spec[i] = NULL;
    }
  }

  dir = NULL;
  err = SHERR_OPNOTSUPP;
  switch (r->cmd) {
    case REV_ADD:
      for (i = 0; i < fl_cnt; i++) {
        if (!fl_spec[i])
          continue;

        err = shfs_attr_set(fl_spec[i], SHATTR_VER);
        if (err) {
          fprintf(stderr, "%s: cannot add %s: %s.\n", process_path, args[i], sherr_str(err)); 
          break;
        }

        fprintf(sharetool_fout, "\tadded %s\n", shfs_filename(fl_spec[i]));
      }
      break;

    case REV_BRANCH:
      if (!ref_name) {
        /* list branches.. */
        break;
      }
      for (i = 0; i < fl_cnt; i++) {
        if (!fl_spec[i])
          continue;

        err = share_file_revision_branch(r, ref_name, fl_spec[i], pflags);
        if (err)
          break;
      }
      break;

    case REV_CAT:
      err = 0;
      for (i = 0; i < fl_cnt; i++) {
        if (!fl_spec[i])
          continue;

        err = share_file_revision_cat(r, fl_spec[i], s_hash, pflags);
        if (err)
          break;
      }
      break;

    case REV_CHECKOUT:
      for (i = 0; i < fl_cnt; i++) {
        if (!fl_spec[i])
          continue;

        err = share_file_revision_checkout(r, fl_spec[i], s_hash, pflags);
        if (err)
          break;
      }
      break;

    case REV_COMMIT:
      for (i = 0; i < fl_cnt; i++) {
        if (!fl_spec[i])
          continue;

        err = share_file_revision_commit(r, fl_spec[i], pflags);
        if (err)
          break;
      }
      break;

    case REV_DIFF:
      for (i = 0; i < fl_cnt; i++) {
        if (!fl_spec[i])
          continue;

        err = share_file_revision_diff(r, fl_spec[i], s_hash, pflags);
        if (err)
          break;
      }

      break;
    case REV_LOG:
      for (i = 0; i < fl_cnt; i++) {
        if (!fl_spec[i])
          continue;

        err = share_file_revision_log(r, fl_spec[i], s_hash, pflags);
        if (err)
          break;
      }
      break;

    case REV_REVERT:
      for (i = 0; i < fl_cnt; i++) {
        if (!fl_spec[i])
          continue;

        err = share_file_revision_revert(r, fl_spec[i], pflags);
        if (err)
          break;
      }
      break;

    case REV_STATUS:
      err = 0;
      for (i = 0; i < fl_cnt; i++) {
        if (!fl_spec[i])
          continue;

        err = share_file_revision_status(r, fl_spec[i], pflags);
        if (err)
          break;
      }
      break;

    case REV_SWITCH:
      for (i = 0; i < fl_cnt; i++) {
        if (!fl_spec[i])
          continue;

        err = share_file_revision_switch(r, ref_name, fl_spec[i], pflags);
        if (err)
          break;
      }
      break;

    case REV_TAG:
      if (!ref_name) {
        /* list tages.. */
        break;
      }
      for (i = 0; i < fl_cnt; i++) {
        if (!fl_spec[i])
          continue;

        err = share_file_revision_tag(r, ref_name, fl_spec[i], pflags);
        if (err)
          break;
      }
      break;
  }

  return (err);
}

int share_file_revision(char **args, int arg_cnt, int pflags)
{
  revop_t *r;
  int err_code;

  if (arg_cnt < 2)
    return (SHERR_INVAL);
  
  r = rev_init();
  rev_command_setstr(r, args[1]); 

  if (r->cmd == REV_NONE) {
    fprintf(stderr, "%s: unknown mode '%s'.\n", process_path, args[1]);
    return (SHERR_OPNOTSUPP);
  }

  args += 2;
  arg_cnt -= 2;

  err_code = share_file_revision_command(r, args, arg_cnt, pflags);

done:
  rev_free(&r);
  return (err_code);
}

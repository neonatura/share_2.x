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

#include "../share.h"


static apr_pool_t *svn_mempool;
svn_repos_t *shsvn_init(char *repos_path)
{
  svn_error_t *err;
  svn_repos_t *repos;

  apr_pool_initialize();

  apr_pool_create(&svn_mempool, NULL);

  INT_ERR(svn_repos_open(&repos, repos_path, svn_mempool));

}

void shsvn_free(void)
{
  apr_pool_destroy(svn_mempool);
}

svn_revnum_t shsvn_initial_revision(svn_repos_t *repos)
{
  svn_fs_t *fs;
  svn_revnum_t youngest_rev;
  svn_error_t *err;

  fs = svn_repos_fs(repos);
  INT_ERR(svn_fs_youngest_rev(&youngest_rev, fs, svn_mempool));

  return (youngest_rev);
}

int shsvn_mkdir(svn_repos_t *repos, char *path)
{
  svn_revnum_t youngest_rev = shsvn_initial_revision(repos);
  svn_fs_txn_t *txn;
  svn_fs_root_t *txn_root;
  svn_revnum_t new_rev;
  svn_error_t *err;
  const char *conflict_str;
  int ret_err;

  /* clear previous transactions */
 // apr_pool_clear(svn_mempool);

  ret_err = 0;
  INT_ERR(svn_repos_fs_begin_txn_for_commit2(&txn, repos,
        youngest_rev, apr_hash_make(svn_mempool), svn_mempool));
  INT_ERR(svn_fs_txn_root(&txn_root, txn, svn_mempool));
  INT_ERR(svn_fs_make_dir(txn_root, path, svn_mempool));

#if 0
  err = svn_repos_fs_commit_txn(&conflict_str, repos, 
      &youngest_rev, txn, svn_mempool);
//  if (err->apr_err == SVN_ERR_FS_CONFLICT)
  if (err) 
    ret_err = -1;
  INT_ERR(err);
#endif
  /* skip hooks */
  err = svn_fs_commit_txn(&conflict_str, &new_rev, txn, svn_mempool); 
  if (err)
    ret_err = -1;
  INT_ERR(err);

  return (ret_err);
}



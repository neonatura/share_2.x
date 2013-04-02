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

#ifndef __SVN__SHSVN_H__
#define __SVN__SHSVN_H__

#include <apr.h>
#include <apr_version.h>
#include <apr_pools.h>
#include <apr_hash.h>
#include <apr_file_io.h>

#include "svn_types.h"
#include "svn_string.h"
#include "svn_error.h"
#include "svn_hash.h"
#include "svn_sorts.h"
#include "svn_io.h"
#include "svn_pools.h"
#include "svn_fs.h"
#include "svn_repos.h"

/*
 * Convert a Subversion error into a simple boolean error code.
 *
 * NOTE:  Subversion errors must be cleared (using svn_error_clear())
 *        because they are allocated from the global pool, else memory
 *        leaking occurs.
 */
#define INT_ERR(expr)                           \
  do {                                          \
    svn_error_t *__temperr = (expr);            \
    if (__temperr)                              \
      {                                         \
        svn_error_clear(__temperr);             \
        return 1;                               \
      }                                         \
    return 0;                                   \
  } while (0);

/**
 * Initiate a reference to a SVN repository.
 */
svn_repos_t *shsvn_init(char *repos_path);

/**
 * Destroy a reference to a SVN repository.
 *
 * Called after the last open repository reference is no longer needed.
 */
svn_revnum_t shsvn_initial_revision(svn_repos_t *repos);

/**
 * Returns the youngest revision in a SVN repository.
 */
void shsvn_free(void);

#endif /* ndef __SVN__SHSVN_H__ */



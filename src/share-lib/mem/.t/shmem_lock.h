
/*
 * @copyright
 *
 *  Copyright 2013 Brian Burrell 
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
 */

#ifndef __MEM__SHMEM_LOCK_H__
#define __MEM__SHMEM_LOCK_H__

/**
 * Mutex and semaphore locks.
 * @ingroup libshare_mem
 * @defgroup libshare_memlock Utility functions to provide mutex thread locks and semaphores.
 * @{
 */


/**
 * A lock used by the share library in order to ensure no two threads perform a system call at the same instance.
 */
#define SHLOCK_SYSTEM -9999

/**
 * Prevents the mutex from allowing the same thread to access the lock.
 * @note Similar to a semaphore as the lock is not based on thread conditions.
 */
#define SHLK_PRIVATE (1 << 0)

/**
 * The share library lock structure is used primarily in order to prevent multiple threads from performing system calls at the same instance.
 * The lock includes both re-entrant and private access styles.
 * The functionality can be extended in order to provide custom locks.
 * @see shlock_open() shlock_close()
 */
typedef struct shlock_t shlock_t;

/**
 * The share library lock structure.
 */
struct shlock_t {
  /**
   * The number of references being made to this lock.
   */
  int ref;

  /**
   * The process thread identification number
   * @see gettid()
   */
  pid_t tid;

#if defined(HAVE_PTHREAD_MUTEX_LOCK) && defined(HAVE_PTHREAD_MUTEX_UNLOCK)
  /**
   * A linux-style mutex reference.
   */
  pthread_mutex_t mutex;
#endif

};


/**
 * Create a new lock on a mutex waiting if needed.
 * @param num A positive number identifying the lock.
 * @param flags A set of modifiers to configure the lock. (SHLK_XXX)
 * @note The libshare uses negative numbers for internal locks.
 * @see SHLOCK_SYSTEM SHLK_PRIVATE
 * @bug flags should be stored in @ref shkey_t instead of a paramter 
 */
shlock_t *shlock_open(shkey_t *key, int flags);

/**
 * Opens a lock based on a number.
 * @see shlock_open()
 */
#define shlock_open_num(_num, _flags) \
  shlock_open(ashkey_num(_num), (_flags))

/**
 * Opens a lock based on a string.
 * @see shlock_open()
 */
#define shlock_open_str(_str, _flags) \
  shlock_open(ashkey_str(_str), (_flags))

/**
 * Create a new lock on a mutex unless one has already been created.
 * @param num A positive number identifying the lock.
 * @param flags A set of modifiers to configure the lock. (SHLK_XXX)
 * @note The libshare uses negative numbers for internal locks.
 * @returns A 0 on success, a 1 when the mutex is already locked, and a -1 on error.
 * @see SHLOCK_SYSTEM SHLK_PRIVATE
 */
int shlock_tryopen(shkey_t *key, int flags, shlock_t **lock_p);

/**
 * Opens a lock based on a number.
 * @see shlock_tryopen()
 */
#define shlock_tryopen_num(_num, _flags, _keyp) \
  shlock_tryopen(ashkey_num(_num), (_flags), (_keyp))

/**
 * Opens a lock based on a string.
 * @see shlock_tryopen()
 */
#define shlock_tryopen_str(_str, _flags, _keyp) \
  shlock_tryopen(ashkey_str(_str), (_flags), (_keyp))

/**
 * Unlock a mutex.
 */ 
int shlock_close(shkey_t *key);

/**
 * Closes a lock based on a number.
 * @see shlock_close()
 */
#define shlock_close_num(_num, _flags) \
  shlock_close(ashkey_num(_num), (_flags))

/**
 * Closes a lock based on a string. 
 * @see shlock_close()
 */
#define shlock_close_str(_str, _flags) \
  shlock_close(ashkey_str(_str), (_flags))

/**
 * Destroy all system resources associated with tracking locks.
 * @note It is safe to perform lock operations after calling this function.
 */
void shlock_destroy(void);

/**
 * @}
 */

#endif /* ndef __MEM__SHMEM_LOCK_H__ */




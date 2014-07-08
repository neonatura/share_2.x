
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

#define __MEM__SHMEM_LOCK_C__
#include "share.h"

static shmeta_t *_lock_map;

static int shlock_init(void)
{

  if (!_lock_map)
    _lock_map = shmeta_init(); 

  return (_lock_map == NULL);
}


shlock_t *shlock_open(shkey_t *key, int flags)
{
  shlock_t *lk;
  pid_t tid;
#if defined(HAVE_PTHREAD_MUTEX_LOCK) && defined(HAVE_PTHREAD_MUTEX_UNLOCK)
  pthread_mutexattr_t attr;
  int err;
#endif

  shlock_init();

  tid = gettid();
  lk = shmeta_get_void(_lock_map, key);
  if (!lk) {
    lk = (shlock_t *)calloc(1, sizeof(shlock_t));
    if (!lk)
      return (NULL);
    shmeta_set_void(_lock_map, key, lk, sizeof(shlock_t));
#ifdef HAVE_PTHREAD_MUTEX_INIT
    memset(&attr, 0, sizeof(attr));
    if (!(flags & SHLK_PRIVATE)) {
      pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE);
}
    pthread_mutex_init(&lk->mutex, &attr);
#endif
  } 

#if defined(HAVE_PTHREAD_MUTEX_LOCK) && defined(HAVE_PTHREAD_MUTEX_UNLOCK)
  err = pthread_mutex_lock(&lk->mutex);
  if (err)
    return (NULL);
#else
  /* bug: acts like trylock() instead of lock() .. need to introduce waiting psuedo-lock */
  if (lk->ref) {
    if ((flags & SHLK_PRIVATE)) {
      /* mutex is already locked. */
      return (NULL);
    }
    if (tid != lk->tid)
      return (NULL); /* lock is not accessible from this thread. */
  }
#endif

  /* assign variables after mutex is locked. */
  lk->tid = tid;
  lk->ref++;

  return (lk);
}

_TEST(shlock_open)
{
  shkey_t *key;
  shlock_t *lk;

  key = ashkey_num(0);
  lk = shlock_open(key, 0);
  _TRUEPTR(lk);
  _TRUE(lk->ref == 1);
  _TRUE(!shlock_close(key));
}

int shlock_tryopen(shkey_t *key, int flags, shlock_t **lock_p)
{
  shlock_t *lk;
  pid_t tid;
#if defined(HAVE_PTHREAD_MUTEX_LOCK) && defined(HAVE_PTHREAD_MUTEX_UNLOCK)
  pthread_mutexattr_t attr;
  int err;
#endif

  shlock_init();

  tid = gettid();
  lk = shmeta_get_void(_lock_map, key);
  if (!lk) {
    lk = (shlock_t *)calloc(1, sizeof(shlock_t));
    if (!lk)
      return (-1);
    shmeta_set_void(_lock_map, key, lk, sizeof(shlock_t));
#ifdef HAVE_PTHREAD_MUTEX_INIT
    memset(&attr, 0, sizeof(attr));
    if (!(flags & SHLK_PRIVATE)) {
      pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE);
}
    pthread_mutex_init(&lk->mutex, &attr);
#endif
  } else if (lk->ref) {
    if ((flags & SHLK_PRIVATE)) {
      /* mutex is already locked. */
      return (1);
    }
    if (tid != lk->tid)
      return (1); /* lock is not accessible from this thread. */
  }

  if (!lk->ref || lk->tid != tid) {
#if defined(HAVE_PTHREAD_MUTEX_LOCK) && defined(HAVE_PTHREAD_MUTEX_UNLOCK)
    /* returns an errno */
    err = pthread_mutex_trylock(&lk->mutex);
    if (err == EBUSY)
      return (1);
    if (err)
      return (-1);
#endif
  }

  /* assign variables after mutex is locked. */
  lk->tid = tid;
  lk->ref++;
  *lock_p = lk;

  return (0);
}

_TEST(shlock_tryopen)
{
  shlock_t *lk1 = NULL;
  shlock_t *lk2 = NULL;
  shkey_t *key1;
  shkey_t *key2;

  key1 = ashkey_num(0);
  _TRUE(!shlock_tryopen(key1, 0, &lk1));
  _TRUEPTR(lk1);

  key2 = ashkey_num(0);
  _TRUE(!shlock_tryopen(key2, 0, &lk2));
  _TRUEPTR(lk2);

  key1 = ashkey_num(0);
  _TRUE(!shlock_close(key1));

  key2 = ashkey_num(0);
  _TRUE(!shlock_close(key2));
}

int shlock_close(shkey_t *key)
{
  shlock_t *lk;
  pid_t tid;
  int err;

  err = shlock_init();
  if (err) {
    return (err);
}

  lk = shmeta_get_void(_lock_map, key);
  if (!lk)
    return (0); /* all done */

  if (lk->ref == 0)
    return (0); /* the mutex is not locked. */

  tid  = gettid();
  if (tid != lk->tid)
    return (0); /* wrong thread calling. */

#if defined(HAVE_PTHREAD_MUTEX_LOCK) && defined(HAVE_PTHREAD_MUTEX_UNLOCK)
  err = pthread_mutex_unlock(&lk->mutex);
  if (err) {
    return (-1);
  }
#endif

  /* assign variables after mutex is unlocked. */
  lk->ref--;
  if (lk->ref == 0)
    lk->tid = 0;

  return (0);
}

void shlock_destroy(void)
{

  shmeta_free(&_lock_map);
}

#undef __MEM__SHMEM_LOCK_C__




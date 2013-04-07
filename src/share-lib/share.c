
/*
 * @copyright
 *
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
 *
 *  @endcopyright
 */

#include "share.h"

char *get_libshare_email(void)
{
  return (PACKAGE_BUGREPORT);
}

char *get_libshare_version(void)
{
  return (PACKAGE_VERSION);
}

char *get_libshare_title(void)
{
  return (PACKAGE_NAME);
}

/**
 * The libshare memory buffer pool allocation utilities.
 */
#define __SHBUF__

shbuf_t *shbuf_init(void)
{
  shbuf_t *buf;

  buf = (shbuf_t *)calloc(1, sizeof(shbuf_t));
  return (buf);
}

_TEST(shbuf_init)
{
  shbuf_t *buff = shbuf_init();
  CuAssertPtrNotNull(ct, buff); 
  shbuf_free(&buff);
}

void shbuf_grow(shbuf_t *buf, size_t data_len)
{
  if (!buf->data) {
    buf->data_max = MAX(4096, data_len * 2);
    buf->data = (char *)calloc(buf->data_max, sizeof(char));
  } else if (buf->data_of + data_len >= buf->data_max) {
    buf->data_max = (buf->data_max + data_len) * 2;
    buf->data = (char *)realloc(buf->data, buf->data_max);
  } 
}

_TEST(shbuf_grow)
{
  shbuf_t *buff = shbuf_init();

  shbuf_grow(buff, 10240);
  CuAssertPtrNotNull(ct, buff->data); 
  CuAssertTrue(ct, buff->data_max >= 10240);

  shbuf_free(&buff);
}

void shbuf_catstr(shbuf_t *buf, char *data)
{
  shbuf_cat(buf, (unsigned char *)data, strlen(data));
}

_TEST(shbuf_catstr)
{
  shbuf_t *buff = shbuf_init();
  char *str;
  int i;

  CuAssertPtrNotNull(ct, buff); 
  if (!buff)
    return;

  str = (char *)calloc(10240, sizeof(char));

  for (i = 0; i < 10240; i++) {
    memset(str, 'a', sizeof(str) - 1);
    shbuf_catstr(buff, str);
  }

  CuAssertPtrNotNull(ct, buff->data); 
  if (buff->data)
    CuAssertTrue(ct, strlen(buff->data) == (10240 * (sizeof(str) - 1)));
  CuAssertTrue(ct, buff->data_of == (10240 * (sizeof(str) - 1)));
  CuAssertTrue(ct, buff->data_max <= (2 * 10240 * (sizeof(str) - 1)));

  free(str);
  shbuf_free(&buff);
}

void shbuf_cat(shbuf_t *buf, void *data, size_t data_len)
{

  if (!buf)
    return;

  shbuf_grow(buf, data_len);
  memcpy(buf->data + buf->data_of, data, data_len);
  buf->data_of += data_len;

}

_TEST(shbuf_cat)
{
  shbuf_t *buff = shbuf_init();
  char *str;
  int i;

  CuAssertPtrNotNull(ct, buff); 
  if (!buff)
    return;

  str = (char *)calloc(10240, sizeof(char));

  for (i = 0; i < 10240; i++) {
    memset(str, (char)rand(), sizeof(str) - 1);
    shbuf_cat(buff, str, sizeof(str));
  }

  CuAssertPtrNotNull(ct, buff->data); 
  CuAssertTrue(ct, buff->data_of == (10240 * sizeof(str)));
  CuAssertTrue(ct, buff->data_max <= (2 * 10240 * sizeof(str)));

  free(str);
  shbuf_free(&buff);
}

size_t shbuf_size(shbuf_t *buf)
{

  if (!buf)
    return (0);

  return (buf->data_of);
}

_TEST(shbuf_size)
{
  shbuf_t *buf;

  _TRUEPTR(buf = shbuf_init());
  if (!buf)
    return;
  
  shbuf_catstr(buf, "shbuf_size");
  _TRUE(shbuf_size(buf) == strlen("shbuf_size"));
  shbuf_free(&buf);
}

/**
 * May consider a hook here to trim contents of maximum buffer size or swap in/out of a cache pool.
 */
void shbuf_clear(shbuf_t *buf)
{

  if (!buf)
    return;

  shbuf_trim(buf->data_of);
}

_TEST(shbuf_clear)
{
  shbuf_t *buf;

  _TRUEPTR(buf = shbuf_init());
  if (!buf)
    return;
  shbuf_catstr(buf, "shbuf_clear");
  shbuf_clear(buf);
  _TRUE(shbuf_size(buf) == 0);
  shbuf_free(&buf);
}

void shbuf_trim(shbuf_t *buf, size_t len)
{
  if (!buf || !buf->data)
    return;

  len = MIN(len, buf->data_of);
  if (len == 0)
    return;

  if (buf->data_of == len) {
    buf->data_of = 0;
    return;
  }

  memmove(buf->data, buf->data + len, buf->data_of - len);
  buf->data_of -= len;
}

_TEST(shbuf_trim)
{
  shbuf_t *buff = shbuf_init();
  char *str;

  CuAssertPtrNotNull(ct, buff); 
  if (!buff)
    return;

  str = (char *)calloc(10240, sizeof(char));
  memset(str, (char)rand(), 10240);
  shbuf_cat(buff, str, 10240);
  CuAssertTrue(ct, buff->data_of == 10240);
  shbuf_trim(buff, 5120);
  CuAssertTrue(ct, buff->data_of == 5120);

  free(str);
  shbuf_free(&buff);
}

void shbuf_free(shbuf_t **buf_p)
{
  shbuf_t *buf = *buf_p;
  if (!buf)
    return;
  free(buf->data);
  free(buf);
  *buf_p = NULL;
}

#undef __SHBUF__




#define __SHCRC__
const int MOD_SHCRC = 65521;
uint64_t shcrc(unsigned char *data, int32_t len)
{
    uint64_t a = 1, b = 0;
    int32_t adl_idx;
 
    for (adl_idx = 0; adl_idx < len; ++adl_idx) {
        a = (a + data[adl_idx]) % MOD_SHCRC;
        b = (b + a) % MOD_SHCRC;
    }
 
    return ((b << 16) | a);
}
#undef __SHCRC__




#define __SHKEY__
shkey_t shkey_init_str(char *kvalue)
{
  return ((shkey_t)shcrc(kvalue, strlen(kvalue) + 1));
}

_TEST(shkey_init_str)
{
  shkey_t keys[26];
  char buf[256];
  int i, j;

  memset(buf, 0, sizeof(buf));
  for (i = 0; i < 26; i++) {
    memset(buf, 'a' + i, sizeof(buf) - 1);
    keys[i] = shkey_init_str(buf);
  }

  for (i = 0; i < 26; i++) {
    for (j = (i + 1); j < 26; j++) {
      CuAssertTrue(ct, keys[i] != keys[j]);
    }
  }

}

shkey_t shkey_init_num(long kvalue)
{
  static shkey_t ret_key;
  char buf[256];
  
  memset(buf, 0, sizeof(buf));
  memcpy(buf, &kvalue, sizeof(kvalue));

  return (*((shkey_t *)buf));
}

_TEST(shkey_init_num)
{
  shkey_t keys[256];
  long num;
  int i, j;

  num = 0;
  for (i = 0; i < 256; i++) {
    num += (long)(rand() % 256);
    keys[i] = shkey_init_num(num);
  }

  for (i = 0; i < 256; i++) {
    for (j = (i + 1); j < 256; j++) {
      CuAssertTrue(ct, keys[i] != keys[j]);
    }
  }

}

shkey_t shkey_init_unique(void)
{
  static uniq_off;
  return ((shkey_t)(shtime64() + uniq_off++));
}

_TEST(shkey_init_unique)
{
  shkey_t key1 = shkey_init_unique();
  shkey_t key2 = shkey_init_unique();
  CuAssertTrue(ct, key1 != key2);
}
#undef __SHKEY__



#define __SHTIME__
double shtime(void)
{
  struct timeval tv;
  double stamp;

  gettimeofday(&tv, NULL);
  tv.tv_sec -= 1325397600; /* 2012 */ 
  stamp = (double)(tv.tv_sec * 1000) + (double)(tv.tv_usec / 1000);

  return (stamp);
}

_TEST(shtime)
{
  CuAssertTrue(ct, shtime() > 31622400); /* > 1 year */
}

shtime_t shtime64(void)
{
  return ((shtime_t)shtime());
}

_TEST(shtime_64)
{
  double ftime = fabs(shtime()); 
  uint64_t ltime = shtime64();
  CuAssertTrue(ct, (uint64_t)ftime == ltime);
}
#undef __SHTIME__




#define __SHPREF__
/**
 * Specifies the list of available preferences to set.
 */
static char *shpref_list[SHPREF_MAX] =
{
  SHPREF_BASE_DIR,
  SHPREF_OVERLAY,
  SHPREF_TRACK
};

/**
 * Private instances of runtime configuration options.
 */
static shmeta_t *_local_preferences; 
static char *_local_preferences_data;

char *shpref_path(void)
{
  static char ret_path[PATH_MAX+1];
  char *ptr;

  ptr = getenv("HOME");
  if (!ptr) ptr = ".";
  sprintf(ret_path, "%s/.share/", ptr);
  mkdir(ret_path, 0777);
  strcat(ret_path, "pref.map");

  return ((char *)ret_path);
}

int shpref_init(void)
{
  shmeta_t *h;
  struct stat st;
  char path[PATH_MAX+1];
  char *data;
  shkey_t key;
  size_t data_len;
  size_t len;
  int err;
  int b_of;

  if (_local_preferences)
    return (0);
  

  h = shmeta_init();
  if (!h)
    return (-1);

  err = shfs_read_mem(path, &data, &data_len);
  if (!err) { /* file may not have existed. */
    while (b_of < data_len) {
      /* obtain key */
      key = *((shkey_t *)(data + b_of));
      b_of += sizeof(shkey_t);

      /* obtain null-terminated string value */
      len = strlen(data + b_of) + 1;
      shmeta_set(h, key, shmeta_str(data + b_of));
      b_of += len;
    }
  }

  _local_preferences = h;
  _local_preferences_data = data;

  return (0);
}

_TEST(shpref_init)
{
  _TRUE(!shpref_init());
}

void shpref_free(void)
{
  shmeta_free(&_local_preferences);

  free(_local_preferences_data);
  _local_preferences_data = NULL;
}

int shpref_save(void)
{
  shbuf_t *buff;
  char *path;
  int err;

  err = shpref_init();
  if (err)
    return (err);

  buff = shbuf_init();
  shmeta_print(_local_preferences, buff);

  path = shpref_path();
  err = shfs_write_mem(path, buff->data, buff->data_of);
  shbuf_free(&buff);
  if (err == -1)
    return (err);

  return (0);
}

_TEST(shpref_save)
{
  _TRUE(!shpref_save());
}

char *shpref_get(char *pref, char *default_value)
{
  shmeta_value_t *val;
  int err;

  err = shpref_init();
  if (err)
    return (default_value);

  val = shmeta_get(_local_preferences, shkey_init_str(pref));
  if (!val)
    return (default_value);

  return ((char *)val->raw);
}

_TEST(shpref_get)
{
  int i;

  for (i = 0; i < SHPREF_MAX; i++) {
    _TRUEPTR(shpref_get(shpref_list[i], "shpref_get"));
  }
}

int shpref_set(char *pref, char *value)
{
  shmeta_t *t_local_preferences;
  char *t_local_preferences_data;
  int err;

  t_local_preferences = _local_preferences;
  t_local_preferences_data = _local_preferences_data;
  _local_preferences = NULL;
  _local_preferences_data = NULL;
  
  /* create a new instance with permanent settings. */
  err = shpref_init();
  if (!err) {
    /* set permanent configuration setting. */
    shmeta_set(_local_preferences, shkey_init_str(pref), shmeta_str(value));
  }

  err = shpref_save();
  if (err)
    return (err);

  /* release instance of permanent settings. */
  shpref_free();

  _local_preferences = t_local_preferences;
  _local_preferences_data = t_local_preferences_data; 

  /* set in current process session settings. */
  shmeta_set(_local_preferences, shkey_init_str(pref), shmeta_str(value));

  return (0);
}

_TEST(shpref_set)
{
  int i;

  for (i = 0; i < SHPREF_MAX; i++) {
    _TRUE(!shpref_set(shpref_list[i], shpref_get(shpref_list[i], NULL)));
  } 
}

char *shpref_base_dir(void)
{
  static char ret_path[PATH_MAX+1];

  if (!*ret_path) {
    char *path = shpref_get(SHPREF_BASE_DIR, NULL);
    if (path) {
      strncpy(ret_path, path, sizeof(ret_path) - 1);
    }

    if (!*ret_path) {
      path = getenv("HOME");
      if (!path) path = getenv("HOMEDIR");
      if (!path) path = getenv("APPDATA");
      if (path)
        sprintf(ret_path, "%s/.share/", path);
    }

    if (!*ret_path) {
      getcwd(ret_path, sizeof(ret_path) - 1);
    }
  }

  return (ret_path);
}
#undef __SHPREF__





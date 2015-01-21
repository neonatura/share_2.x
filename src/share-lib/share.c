
/*
 * @copyright
 *
 *  Copyright 2013 Neo Natura 
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

#include "share.h"
#include <pwd.h>

static const char *_crc_str_map = "-ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+";

static shpeer_t _default_peer;


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
 * Unix: ~/.shlib
 * Windows: C:\Users\Username\AppData\Roaming\shlib
 * Mac: ~/Library/Application Support/shlib
 * @returns The directory where share library persistent data is stored.
 * @note This value can be overwritten with the 
 * @note This value can be overwritten with the shared preference "base-dir".
 */
const char *get_libshare_path(void)
{
  static char ret_path[PATH_MAX+1];
  struct stat st;
  char pathbuf[PATH_MAX+1];
  const char *path;
  int err;

  if (!*ret_path) {
    /* check global setting */
    path = shpref_get(SHPREF_BASE_DIR, NULL);
    if (path && *path) {
      mkdir(path, 0777);
      if (0 == stat(path, &st) && S_ISDIR(st.st_mode)) {
        strncpy(ret_path, path, sizeof(ret_path) - 1);
        return ((const char *)ret_path);      
      }
    }
  }

  if (!*ret_path) {
    /* check app-home dir */
    memset(pathbuf, 0, sizeof(pathbuf));
    path = getenv("SHLIB_PATH");
    if (path && *path) {
      strncpy(ret_path, path, sizeof(ret_path) - 1);
    }
  }

#ifdef linux
  if (!*ret_path) {
    mkdir("/var/lib/share/", 0777);
    err = stat("/var/lib/share/", &st);
    if (!err && S_ISDIR(st.st_mode)) {
      strcpy(ret_path, "/var/lib/share/");
    }
  }
#endif

  if (!*ret_path) {
#ifdef _WIN32
    path = GetSpecialFolderPath(CSIDL_APPDATA);
#else
    path = getenv("HOME");
#endif
    if (path && *path) {
#if MAC_OSX
      sprintf(pathbuf, "%s/Library/Application Support", path);
      mkdir(pathbuf, 0777);
#else
      strncpy(pathbuf, path, sizeof(pathbuf) - 1);
#endif
    } else {
      getcwd(pathbuf, sizeof(pathbuf) - 1);
    }
    sprintf(ret_path, "%s/.shlib/", pathbuf);
    mkdir(ret_path, 0777);
  }

  return ((const char *)ret_path);
}

const char *get_libshare_account_name(void)
{
  static char username[MAX_SHARE_NAME_LENGTH];
  char *str;

  memset(username, 0, sizeof(username));
  str = shpref_get(SHPREF_USER_NAME, "");
#ifdef HAVE_GETLOGIN_R
  if (!*str) {
    static char user_buf[1024];

    memset(user_buf, 0, sizeof(user_buf));
    getlogin_r(user_buf, sizeof(user_buf) - 1);
    str = user_buf;
  }
#endif
  strncpy(username, str, MAX_SHARE_NAME_LENGTH-1);

  return (username);
}
shkey_t *get_libshare_account_pass(void)
{
  shkey_t user_key;
  char *username = get_libshare_account_name();
  char *pass = shpref_get(SHPREF_USER_PASS, "");

#ifdef HAVE_GETPWNAM
  if (!*pass) {
    struct passwd *pw = getpwnam(username);
    if (pw)
      pass = pw->pw_passwd;
  }
#endif

  /* generate pass seed */
  memcpy(&user_key, shpam_user_gen(username), sizeof(shkey_t));
  return (shpam_seed_gen(&user_key, pass));
}
const char *get_libshare_account_email(void)
{
  static char def_str[1024];
  char hostname[MAXHOSTNAMELEN+1];
  char username[MAX_SHARE_NAME_LENGTH];

  memset(hostname, 0, sizeof(hostname));
  gethostname(hostname, sizeof(hostname)-1);
  memset(username, 0, sizeof(username));
  getlogin_r(username, sizeof(username) - 1);
  sprintf(def_str, "%s@%s", username, hostname); 
  return (shpref_get(SHPREF_USER_EMAIL, def_str));
}

/**
 * The libshare memory buffer pool allocation utilities.
 */
#ifdef __INLINE__SHBUF__

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

  shbuf_trim(buf, buf->data_of);
}

_TEST(shbuf_clear)
{
  shbuf_t *buf = shbuf_init();

  _TRUEPTR(buf);
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

#endif /* def __INLINE__SHBUF__ */




#define __SHCRC__
const int MOD_SHCRC = 65521;
uint64_t shcrc(void *data_p, int len)
{
  unsigned char *data = (unsigned char *)data_p;
  uint64_t b = 0;
  uint64_t d = 0;
  uint32_t a = 1, c = 1;
  uint64_t ret_val;
  char num_buf[8];
  int *num_p;
  int idx;

  if (data) {
    num_p = (int *)num_buf;
    for (idx = 0; idx < len; idx += 4) {
      memset(num_buf, 0, 8);
      memcpy(num_buf, data + idx, MIN(4, len - idx)); 
      a = (a + *num_p);
      b = (b + a);
      c = (c + data[idx]) % MOD_SHCRC;
      d = (d + c) % MOD_SHCRC;
    }
  }

  ret_val = ((d << 16) | c);
  ret_val += ((b << 32) | a);
  return (ret_val);
}
_TEST(shcrc)
{
  char buf[256];
  uint64_t val1;
  uint64_t val2;

  memset(buf, 'a', sizeof(buf));
  val1 = shcrc(buf, sizeof(buf));
  _TRUE(3978706384553603202 == val1);

  buf[128] = 'b';
  val2 = shcrc(buf, sizeof(buf));
  _TRUE(3978706521994653828 == val2);
}
char *shcrcstr(uint64_t crc)
{
  static char ret_str[256];
  uint64_t tcrc;
  uint8_t *cptr;
  int idx;
  char ch;
int i;

  idx = -1;
  tcrc = crc;
  memset(ret_str, 0, sizeof(ret_str));
  while (tcrc && idx < 64) {
    ch = (tcrc % 64);
    ret_str[++idx] = _crc_str_map[ch];
    tcrc = tcrc >> 6;
  }

  return (ret_str);
}
_TEST(shcrcstr)
{
  char *str;
  char buf[4096];
  uint64_t crc;
  int i, j;

  for (j = 0; j < 256; j++) {
    memset(buf, j, sizeof(buf));
    crc = shcrc(buf, sizeof(buf));
    str = shcrcstr(crc);
    _TRUEPTR(str);

    _TRUE(strlen(str));
    for (i = 0; i < strlen(str); i++) {
      _TRUEPTR( strchr(_crc_str_map, str[i]) );
    }
  }

}
int stridx(const char *str, char ch)
{
  int i, len;
  len =strlen(str);
  for (i = 0; i < len; i++)
    if (str[i] == ch)
      return (i);
  return (-1);
}
uint64_t shcrcgen(char *str)
{
  uint64_t crc;
  int idx;
  int i;

  crc = 0;
  for (i = (strlen(str)-1); i >= 0; i--) {
    idx = stridx(_crc_str_map, str[i]);
    if (idx == -1)
      return (0);

    crc += idx;

    if (i != 0)
      crc = crc << 6;
  }

  return (crc);
}
_TEST(shcrcgen)
{
  char *str;
  char buf[4096];
  uint64_t crc;
  uint64_t ncrc;
  int j;

  for (j = 0; j < 256; j++) {
    memset(buf, j, sizeof(buf));
    crc = shcrc(buf, sizeof(buf));
    str = shcrcstr(crc);
    ncrc = shcrcgen(str);
    _TRUE(ncrc == crc);
  }

}
#undef __SHCRC__









#define __SHTIME__
double shtime(void)
{
  struct timeval tv;
  double stamp;

  gettimeofday(&tv, NULL);
  tv.tv_sec -= 1325397600; /* 2012 */ 
  stamp = (double)tv.tv_sec + ((double)tv.tv_usec / 1000000);

  return (stamp);
}
_TEST(shtime)
{
  _TRUE(shtime() > 31622400); /* > 1 year */
}
shtime_t shtime64(void)
{
  struct timeval tv;
  shtime_t stamp;

  memset(&tv, 0, sizeof(tv));
  gettimeofday(&tv, NULL);
  tv.tv_sec -= 1325397600; /* 2012 */
  stamp = (shtime_t)(tv.tv_sec * 10) + (shtime_t)(tv.tv_usec / 100000);

  return (stamp);
}
_TEST(shtime64)
{
  uint64_t d  = (uint64_t)fabs(shtime() / 2);
  uint64_t n = (shtime64() / 20);
  _TRUE(n == d);
}
time_t shutime64(shtime_t t)
{
  time_t conv_t;
  conv_t = (time_t)(t / 10) + 1325397600;
  return (conv_t);
}
char *shctime64(shtime_t t)
{
  static char ret_str[256];

  memset(ret_str, 0, sizeof(ret_str));

  if (t != 0) {
    time_t conv_t = shutime64(t);
    ctime_r(&conv_t, ret_str); 
  }
  
  return (ret_str);
}
char *shstrtime64(shtime_t t, char *fmt)
{
  static char ret_str[256];
  time_t utime;

  if (!fmt)
    fmt = "%x %X"; /* locale-specific format */

  utime = shutime64(t);
  memset(ret_str, 0, sizeof(ret_str));
  strftime(ret_str, sizeof(ret_str) - 1, fmt, localtime(&utime)); 

  return (ret_str);
}
_TEST(shctime64)
{
  shtime_t s_time;
  time_t u_time;
  char s_buf[64];
  char u_buf[64];

  s_time = shtime64();
  u_time = time(NULL);

  strncpy(s_buf, shctime64(s_time), sizeof(s_buf) - 1);
  strncpy(u_buf, ctime(&u_time), sizeof(u_buf) - 1);

  _TRUE(0 == strcmp(s_buf, u_buf));
}
shtime_t shtime64_adj(shtime_t stamp, double secs)
{
  stamp += (shtime_t)(secs/10);
  return (stamp);
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
  SHPREF_TRACK,
  SHPREF_USER_NAME,
  SHPREF_USER_EMAIL
};

/**
 * Private instances of runtime configuration options.
 */
static shmeta_t *_local_preferences; 
static char *_local_preferences_data;

char *shpref_path(void)
{
  static char ret_path[PATH_MAX+1];
  struct stat st;
  char pathbuf[PATH_MAX+1];
  char *path;
  int err;

  if (!*ret_path) {
    memset(pathbuf, 0, sizeof(pathbuf));

    if (!*pathbuf) {
      /* check app-home dir */
      memset(pathbuf, 0, sizeof(pathbuf));
      path = getenv("SHLIB_PATH");
      if (path && *path) {
        strncpy(pathbuf, path, sizeof(pathbuf) - 1);
      }
    }

#ifdef linux
    if (!*pathbuf) {
      mkdir("/var/lib/share/", 0777);
      err = stat("/var/lib/share/", &st);
      if (!err && S_ISDIR(st.st_mode)) {
        strcpy(pathbuf, "/var/lib/share/");
      }
    }
#endif

    if (!*pathbuf) {
#ifdef _WIN32
      path = GetSpecialFolderPath(CSIDL_APPDATA);
#else
      path = getenv("HOME");
#endif
      if (path && *path) {
#if MAC_OSX
        sprintf(pathbuf, "%s/Library/Application Support", path);
        mkdir(pathbuf, 0777);
#else
        strncpy(pathbuf, path, sizeof(pathbuf) - 1);
#endif
      } else {
        getcwd(pathbuf, sizeof(pathbuf) - 1);
      }
      sprintf(pathbuf, "%s/.shlib/", pathbuf);
      mkdir(pathbuf, 0777);
    }

    sprintf(ret_path, "%s/.pref.%lu",
        pathbuf, (unsigned long)getuid());
  }

  return ((char *)ret_path);
}

int shpref_init(void)
{
  shmeta_t *h;
  struct stat st;
  char *path;
  char *data;
  shkey_t *key;
  size_t data_len;
  size_t len;
  int err;
  int b_of;

  if (_local_preferences)
    return (0);

  h = shmeta_init();
  if (!h)
    return (-1);

  key = (shkey_t *)calloc(1, sizeof(shkey_t));
  path = shpref_path();
  err = shfs_read_mem(path, &data, &data_len);
  if (!err) { /* file may not have existed. */
    b_of = 0;
    while (b_of < data_len) {
	    shmeta_value_t *hdr = (shmeta_value_t *)(data + b_of);
			memcpy(key, &hdr->name, sizeof(shkey_t)); 
      shmeta_set_str(h, key, data + b_of + sizeof(shmeta_value_t));

      b_of += sizeof(shmeta_value_t) + hdr->sz;
    }
  }

  free(key);

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

  chmod(path, 0700);

  return (0);
}

_TEST(shpref_save)
{
  _TRUE(!shpref_save());
}

const char *shpref_get(char *pref, char *default_value)
{
  static char ret_val[SHPREF_VALUE_MAX+1];
  char tok[SHPREF_NAME_MAX + 16];
  shmeta_value_t *val;
  shkey_t *key;
  int err;

  err = shpref_init();
  if (err)
    return (default_value);

  memset(tok, 0, sizeof(tok));
  strncpy(tok, pref, SHPREF_NAME_MAX);
  key = ashkey_str(tok);
  val = shmeta_get(_local_preferences, key);

  memset(ret_val, 0, sizeof(ret_val));
  if (!val) {
    if (default_value)
      strncpy(ret_val, default_value, sizeof(ret_val) - 1);
  } else {
    strncpy(ret_val, (char *)val->raw, sizeof(ret_val) - 1); 
  }

  return (ret_val);
}

_TEST(shpref_get)
{
  int i;

  for (i = 0; i < SHPREF_MAX; i++) {
    _TRUEPTR((char *)shpref_get(shpref_list[i], "shpref_get"));
  }
}

int shpref_set(char *pref, char *value)
{
  char tok[SHPREF_NAME_MAX+16];
  shkey_t *key;
  int err;

  err = shpref_init();
  if (err)
    return (err);

  memset(tok, 0, sizeof(tok));
  strncpy(tok, pref, SHPREF_NAME_MAX);
  key = ashkey_str(tok);
  if (value) {
    /* set permanent configuration setting. */
    shmeta_set_str(_local_preferences, key, value);
  } else {
    shmeta_unset_str(_local_preferences, key);
  }

  err = shpref_save();
  if (err)
    return (err);

  return (0);
}

_TEST(shpref_set)
{
  int i;

  for (i = 0; i < SHPREF_MAX; i++) {
    char *ptr = (char *)shpref_get(shpref_list[i], NULL);
    if (ptr) {
      ptr = strdup(ptr);
      _TRUE(0 == shpref_set(shpref_list[i], ptr)); 
      free(ptr);
    } else { 
      _TRUE(0 == shpref_set(shpref_list[i], NULL)); 
    }
  } 
}
#undef __SHPREF__


#define __SHPEER__
static void shpeer_set_app(shpeer_t *peer, char *app_name)
{
  shkey_t *key;
  struct hostent *ent;
  char pref[512];
  char *ptr;
  int idx;

  if (!app_name || !*app_name) {
#ifdef PACKAGE
    app_name = PACKAGE;
#else
    app_name = "libshare";
#endif
  }

  idx = stridx(app_name, ':');
  if (idx == -1) {
    strncpy(peer->label, app_name, sizeof(peer->label) - 1);
  } else {
    strncpy(peer->label, app_name, MIN(sizeof(peer->label) - 1, idx));
  }
}
static void shpeer_set_hwaddr(shpeer_t *peer)
{
  struct ifreq buffer;
  int i;
  int s;

  s = socket(PF_INET, SOCK_DGRAM, 0);

  memset(&buffer, 0, sizeof(buffer));
  strcpy(buffer.ifr_name, "eth0");
/* bug: check error code. loop for ethXX. */
  ioctl(s, SIOCGIFHWADDR, &buffer);

  close(s);

  for (i = 0; i < 6; i++) {
    peer->addr.hwaddr[i] = (unsigned char)buffer.ifr_hwaddr.sa_data[i];
  }
}
static void shpeer_set_group(shpeer_t *peer, char *name)
{
  char *ptr;

  if (!name)
    return;

  ptr = strchr(name, ':');
  if (ptr)
    strncpy(peer->group, ptr + 1, sizeof(peer->group) - 1);
}
static void shpeer_set_host(shpeer_t *peer, char *hostname)
{
  struct servent *serv;
  struct hostent *ent;
  char peer_host[MAXHOSTNAMELEN+1];
  char *ptr;
  int port;
  int proto;

  port = 0;
  ent = NULL;
  memset(peer_host, 0, sizeof(peer_host));

  if (hostname) {
    strncpy(peer_host, hostname, sizeof(peer_host) - 1);
    ptr = strchr(peer_host, ':');
    if (ptr) {
      port = atoi(ptr+1);
      *ptr = '\0';
    }

    ent = shnet_peer(peer_host);
  }

  /* lookup service port */
  if (!port && *peer->label 
#ifdef PACKAGE
        && 0 != strcmp(peer->label, PACKAGE)
#endif
      ) {
    serv = getservbyname(peer->label, "tcp");
    if (serv)
      port = ntohs(serv->s_port);
    endservent();
  }

  if (!ent) {
    peer->type = SHNET_PEER_LOCAL;
    peer->addr.sin_addr[0] = (uint32_t)htonl(INADDR_LOOPBACK);
    peer->addr.sin_port = htons((uint16_t)port);
  } else if (ent->h_addrtype == AF_INET6) {
    peer->type = SHNET_PEER_IPV6;
    memcpy((uint32_t *)peer->addr.sin_addr, ent->h_addr, ent->h_length);
    peer->addr.sin_port = htons((uint16_t)port);
  } else if (ent->h_addrtype == AF_INET) {
    peer->type = SHNET_PEER_IPV4;
    memcpy(&peer->addr.sin_addr[0], ent->h_addr, ent->h_length);
    peer->addr.sin_port = htons((uint16_t)port);
  }

}
static void shpeer_set_arch(shpeer_t *peer)
{
#if defined(LINUX)
  peer->arch |= SHARCH_LINUX;
#elif defined(FREEBSD)
  peer->arch |= SHARCH_BSD;
#else
  peer->arch |= SHARCH_WIN;
#endif
#ifdef I386
  peer->arch |= SHARCH_32BIT;
#endif
}
static void shpeer_set_key(shpeer_t *peer, shkey_t *out_key)
{
  shkey_t *key;

  key = shkey_bin((char *)peer, sizeof(shpeer_t));
  memcpy(out_key, key, sizeof(shkey_t));
  shkey_free(&key);
}
static void shpeer_set_priv(shpeer_t *peer)
{
  struct passwd *pwd = NULL;

#ifdef HAVE_GETPWUID
  pwd = getpwuid(getuid());
#endif

  if (pwd) {
    peer->uid = shcrc(pwd->pw_name, strlen(pwd->pw_name));
  } else {
#ifndef _WIN32
    peer->uid = getuid();
#endif
  }
  shpeer_set_hwaddr(peer);
  shpeer_set_arch(peer);
}
shpeer_t *shpeer_init(char *appname, char *hostname)
{
  shpeer_t *peer;

  peer = (shpeer_t *)calloc(1, sizeof(shpeer_t));
  if (!peer)
    return (NULL);

  /* pub info */
  shpeer_set_app(peer, appname);
  shpeer_set_key(peer, &peer->key.pub);

  /* priv info */
  shpeer_set_host(peer, hostname);
  shpeer_set_group(peer, appname);
  if (!*peer->group)
    shpeer_set_priv(peer);
  shpeer_set_key(peer, &peer->key.priv);

  return (peer);
}
/** establish default peer */
void shpeer_set_default(shpeer_t *peer)
{
  shpeer_t *def_peer;

  def_peer = NULL;
  if (!peer) {
    def_peer = shpeer_init(NULL, NULL);
    peer = def_peer;
  }
  memcpy(&_default_peer, peer, sizeof(shpeer_t));

  if (def_peer)
    shpeer_free(&def_peer);
}
shpeer_t *shpeer(void)
{
  shpeer_t *peer;

  if (shkey_is_blank(shpeer_kpub(&_default_peer))) {
    shpeer_set_default(NULL);
  }

  peer = (shpeer_t *)calloc(1, sizeof(shpeer_t));
  memcpy(peer, &_default_peer, sizeof(_default_peer));

  return peer;
}
shpeer_t *ashpeer(void)
{
  static shpeer_t ret_peer;

  if (shkey_is_blank(shpeer_kpub(&_default_peer))) {
    /* initialize default peer */
    shpeer_t *peer = shpeer_init(NULL, NULL);
    shpeer_free(&peer);
  }

  memset(&ret_peer, 0, sizeof(ret_peer));
  memcpy(&ret_peer, &_default_peer, sizeof(_default_peer));

  return (&ret_peer);
}
void shpeer_free(shpeer_t **peer_p)
{
  shpeer_t *peer;

  if (!peer_p)
    return;

  peer = *peer_p;
  *peer_p = NULL;

  free(peer);
}
char *shpeer_print(shpeer_t *peer)
{
  static char ret_buf[4096];
  struct in_addr in_addr;
  int i;

  memset(ret_buf, 0, sizeof(ret_buf));

  if (!peer)
    return (ret_buf);

  if (*peer->label)
    sprintf(ret_buf+strlen(ret_buf), "%s", peer->label);
  if (*peer->group)
    sprintf(ret_buf+strlen(ret_buf), ":%s", peer->group);
  strcat(ret_buf, " ");

  switch (peer->type) {
    case SHNET_PEER_LOCAL:
    case SHNET_PEER_IPV4:
      memcpy(&in_addr, &peer->addr.sin_addr, sizeof(struct in_addr));
      strcat(ret_buf, inet_ntoa(in_addr));
      if (peer->addr.sin_port)
        sprintf(ret_buf+strlen(ret_buf), ":%u",
            (unsigned int)ntohs(peer->addr.sin_port)); 
      break;
    case SHNET_PEER_IPV6:
      for (i = 0; i < 4; i++) {
        uint32_t *in6_addr = (uint32_t *)peer->addr.sin_addr;
        if (i != 0)
          strcat(ret_buf, ":");
        sprintf(ret_buf+strlen(ret_buf), "%x", in6_addr + i);
      }
      if (peer->addr.sin_port)
        sprintf(ret_buf+strlen(ret_buf), ":%u", 
            (unsigned int)ntohs(peer->addr.sin_port)); 
      break;
  }

  sprintf(ret_buf+strlen(ret_buf), " (%s)", shkey_print(shpeer_kpub(peer)));

  return (ret_buf);
}
#undef __SHPEER__

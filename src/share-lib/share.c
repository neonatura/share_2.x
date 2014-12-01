
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

#include "share.h"
#include <pwd.h>

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
  char *path;
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
  uint64_t a = 1, b = 0;
  uint32_t c = 1, d = 0;
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
  ret_val += ((b << 16) | a);
  return (ret_val);
}
_TEST(shcrc)
{
  char buf[256];
  uint64_t val1;
  uint64_t val2;

  memset(buf, 'a', sizeof(buf));
  val1 = shcrc(buf, sizeof(buf));
  _TRUE(222707452733649026 == val1);

  buf[128] = 'b';
  val2 = shcrc(buf, sizeof(buf));
  _TRUE(222707452742037636 == val2);
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
  time_t conv_t;

  memset(ret_str, 0, sizeof(ret_str));

  if (t != 0) {
    conv_t = shutime64(t);//(time_t)(t / 10) + 1325397600;
    strcpy(ret_str, ctime(&conv_t)); 
  }
  
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

    sprintf(ret_path, "%s/pref.map", pathbuf);
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

  return (0);
}

_TEST(shpref_save)
{
  _TRUE(!shpref_save());
}

const char *shpref_get(char *pref, char *default_value)
{
  static char ret_val[SHPREF_VALUE_MAX+1];
  shmeta_value_t *val;
  shkey_t *key;
  int err;

  err = shpref_init();
  if (err)
    return (default_value);

  key = ashkey_str(pref);
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
  shkey_t *key;
  int err;

  err = shpref_init();
  if (err)
    return (err);

  key = ashkey_str(pref);
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
    const char *ptr = shpref_get(shpref_list[i], NULL);
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

  if (!app_name || !*app_name) {
#ifdef PACKAGE
    app_name = PACKAGE;
#else
    app_name = "libshare";
#endif
  }

  strncpy(peer->label, app_name, sizeof(peer->label) - 1);
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
  peer->type = SHNET_GROUP;
  peer->addr.sin_addr[0] = shcrc(name, strlen(name));
}
static void shpeer_set_host(shpeer_t *peer, char *hostname)
{
  struct hostent *ent;
  char peer_host[MAXHOSTNAMELEN+1];
  char *ptr;
  int port;

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

  if (!ent) {
    peer->type = SHNET_PEER_LOCAL;
    peer->addr.sin_addr[0] = (uint32_t)htonl(INADDR_LOOPBACK);
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
static void shpeer_set_name(shpeer_t *peer)
{
  shkey_t *key;

  memset(&peer->name, 0, sizeof(peer->name));
  key = shkey_bin((char *)peer, sizeof(shpeer_t));
  memcpy(&peer->name, key, sizeof(shkey_t));
  shkey_free(&key);
}
shpeer_t *shpeer_init(char *appname, char *hostname, int flags)
{
  shpeer_t *peer;

  peer = (shpeer_t *)calloc(1, sizeof(shpeer_t));
  if (!peer)
    return (NULL);

  if (flags & PEERF_PRIVATE) {
    struct passwd *pwd = getpwuid(getuid());
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

  shpeer_set_app(peer, appname);
  if ((flags & PEERF_PUBLIC)) {
    peer->type = SHNET_BROADCAST; 
  } else if ((flags & PEERF_GROUP)) {
    shpeer_set_group(peer, hostname);
  } else {
    shpeer_set_host(peer, hostname);
  }
  shpeer_set_name(peer);

  peer->pid = (uint16_t)getpid();
  peer->flags = (uint32_t)flags;

  return (peer);
}
/** establish default peer */
void shpeer_set_default(shpeer_t *peer)
{
  shpeer_t *def_peer;

  def_peer = NULL;
  if (!peer) {
    def_peer = shpeer_init(NULL, NULL, 0);
    peer = def_peer;
  }
  memcpy(&_default_peer, peer, sizeof(shpeer_t));

  if (def_peer)
    shpeer_free(&def_peer);
}
shpeer_t *shpeer(void)
{
  shpeer_t *peer;

  if (shkey_is_blank(&_default_peer.name)) {
    shpeer_set_default(NULL);
  }

  peer = (shpeer_t *)calloc(1, sizeof(shpeer_t));
  memcpy(peer, &_default_peer, sizeof(_default_peer));

  return peer;
}
shpeer_t *ashpeer(void)
{
  static shpeer_t ret_peer;

  if (shkey_is_blank(&_default_peer.name)) {
    /* initialize default peer */
    shpeer_t *peer = shpeer_init(NULL, NULL, 0);
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
  static char ret_buf[1024];
  struct in_addr in_addr;
  int i;

  memset(ret_buf, 0, sizeof(ret_buf));

  if (!peer)
    return (ret_buf);

  sprintf(ret_buf+strlen(ret_buf), "[%s] ", shkey_print(&peer->name));

  if (*peer->label)
    sprintf(ret_buf+strlen(ret_buf), "%s ", peer->label);

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

  return (ret_buf);
}
#undef __SHPEER__

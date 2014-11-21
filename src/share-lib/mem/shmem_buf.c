
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

#define __MEM__SHMEM_BUF_C__
#include "share.h"
#include <sys/mman.h>


shbuf_t *shbuf_map(unsigned char *data, size_t data_len)
{
  shbuf_t *buf;

  buf = (shbuf_t *)calloc(1, sizeof(shbuf_t));
  buf->data = data;
  buf->data_of = data_len;
  buf->data_max = data_len;

  return (buf);
}

int shbuf_growmap(shbuf_t *buf, size_t data_len)
{
  struct stat st;
  size_t block_size;
  size_t of;
  char *data;
  void *map_data;
  size_t map_len;
  size_t map_newlen;
  int err;

  map_data = NULL;
  map_len = 0;
  if (buf->data) {
    map_data = buf->data;
    map_len = buf->data_of;
    //map_len = buf->data_max;
  }

  if (map_len >= data_len)
    return (0); /* bufmap exceeds allocation requested */

  buf->data = NULL;
  buf->data_max = buf->data_of = 0;

  block_size = sysconf(_SC_PAGE_SIZE);
  map_newlen = data_len / block_size * block_size;

  memset(&st, 0, sizeof(st));
  fstat(buf->fd, &st);
  if (st.st_size < map_newlen) {
    lseek(buf->fd, st.st_size, SEEK_SET);

    data = (char *)calloc(block_size, sizeof(char));
    for (of = st.st_size; of < map_newlen; of += block_size) {
      write(buf->fd, data, block_size); /* error willbe caught on mmap */
}
    free(data);

memset(&st, 0, sizeof(st));
    fstat(buf->fd, &st);
  }

  if (map_data) {
    munmap(map_data, map_len); /* ignore EINVAL return */
  }

/* shouldn't this happen before write()? */
  lseek(buf->fd, 0L, SEEK_SET);
  map_data = mmap(NULL, map_newlen, PROT_READ | PROT_WRITE, MAP_SHARED, buf->fd, 0); 
  if (map_data == MAP_FAILED)
    return (SHERR_NOBUFS);

  buf->data = map_data;
  buf->data_of = buf->data_max = map_newlen;

  return (0);
}

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

int shbuf_grow(shbuf_t *buf, size_t data_len)
{
  int block_len;

#if 0
/* shbuf_growmap now calls shbuf_grow */
  if (buf->fd)
    return (shbuf_growmap(buf, data_len));
#endif


  block_len = ((data_len + 1) / 8192) + 1;
  if ((block_len * 8192) <= buf->data_max)
    return (0); /* already allocated */

  buf->data_max = block_len * 8192;
  if (!buf->data) {
    buf->data = (char *)calloc(buf->data_max, sizeof(char));
  } else {// if (buf->data_of + data_len >= buf->data_max) {
    size_t orig_len = buf->data_max;
    buf->data = (char *)realloc(buf->data, buf->data_max);
    if (!buf->data) { /* realloc not gauranteed */
      char *data = (char *)calloc(buf->data_max, sizeof(char));
      if (data) {
        memcpy(data, buf->data, orig_len);
        free(buf->data);
        buf->data = data;
      }
    }
  } 

  if (!buf->data) {
    buf->data_max = buf->data_of = 0;
    return (SHERR_NOBUFS);
  }

  return (0);
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
  shbuf_t *buff;
  char str[4096];
  int i;

  buff = shbuf_init();
  _TRUEPTR(buff);
  if (!buff)
    return;

  memset(str, 0, sizeof(str));
  memset(str, 'a', sizeof(str) - 1);
  shbuf_catstr(buff, str);
  _TRUEPTR(buff->data);
  if (buff->data)
    _TRUE((size_t)strlen(buff->data) == (sizeof(str) - 1));
  _TRUE((size_t)buff->data_of == (sizeof(str) - 1));
  _TRUE((size_t)buff->data_max >= (sizeof(str) - 1));

  shbuf_free(&buff);
}

void shbuf_cat(shbuf_t *buf, void *data, size_t data_len)
{

  if (!buf)
    return;

  shbuf_grow(buf, buf->data_of + data_len + 1);
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

size_t shbuf_idx(shbuf_t *buf, unsigned char ch)
{
  int i;

  for (i = 0; i < buf->data_of; i++) {
    if (buf->data[i] == ch)
      return (i);   
  }

  return (-1);
}

_TEST(shbuf_idx)
{
  shbuf_t *buf;

  _TRUEPTR(buf = shbuf_init());
  if (!buf)
    return;
  
  shbuf_catstr(buf, "shbuf_size");
  _TRUE(shbuf_idx(buf, 'b') == 2);
  _TRUE(shbuf_idx(buf, 'Z') == -1);
  shbuf_free(&buf);
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

unsigned char *shbuf_data(shbuf_t *buf)
{

  if (!buf)
    return (NULL);

  return (buf->data);
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
  memset(buf->data + len, 0, buf->data_max - buf->data_of);
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
  if (buf->fd) {
    munmap(buf->data, buf->data_max);
    close(buf->fd);
  } else {
    free(buf->data);
  }
  free(buf);
  *buf_p = NULL;
}

/* recursive dir generation for relative paths. */
static void shbuf_mkdir(char *path)
{
  char hier[PATH_MAX+1];
  char dir[PATH_MAX+1];
  char *save_ptr;
  char *n_tok;
  char *tok;

  memset(dir, 0, sizeof(dir));
  if (*path == '/')
    strcat(dir, "/");

  save_ptr = NULL;
  memset(hier, 0, sizeof(hier));
  strncpy(hier, path, sizeof(hier) - 1);
  tok = strtok_r(hier, "/", &save_ptr);
  while (tok) {
    n_tok = strtok_r(NULL, "/", &save_ptr);
    if (!n_tok)
      break;

    strcat(dir, tok);
    strcat(dir, "/");
    mkdir(dir, 0777);
    tok = n_tok;
  }

}

shbuf_t *shbuf_file(char *path)
{
  struct stat st;
  shbuf_t *buff;
  size_t block_size;
  size_t len;
  size_t block_len = 2;
  void *data;
  char *blank;
  int err;
  int fd;

  if (!path)
    return (NULL);

  shbuf_mkdir(path);
  fd = open(path, O_RDWR | O_CREAT, S_IRWXU);
  if (fd == -1) {
    PRINT_ERROR(-errno, "shbuf_file [open]");
    return (NULL);
  }

  err = fstat(fd, &st);
  if (err) {
    close(fd);
    return (NULL);
  }

  block_size = sysconf(_SC_PAGE_SIZE);
  block_len = MAX(1, MAX(st.st_size / block_size, block_len));
  len = (block_size * block_len);

#if 0
  if (st.st_size != len) {
    ftell(fd, st.st_size, SEEK_SET);
    blank = (char *)calloc(block_size, sizeof(char));
    for (of = st.st_size; of < len; of += block_size)
      write(fd, blank, block_size);
    free(blank);
  }
#endif

  buff = shbuf_init();
  if (!buff)
    return (NULL);

  buff->fd = fd;
  err = shbuf_growmap(buff, len);
  if (err) {
    PRINT_ERROR(err, "shbuf_file [growmap]");
    shbuf_free(&buff);
    return (NULL);
  }

  return (buff);
}

void shbuf_append(shbuf_t *from_buff, shbuf_t *to_buff)
{
  if (!from_buff || !to_buff)
    return;
  shbuf_cat(to_buff, shbuf_data(from_buff), shbuf_size(from_buff));
}

shbuf_t *shbuf_clone(shbuf_t *buff)
{
  shbuf_t *ret_buf;

  ret_buf = shbuf_init();
  shbuf_append(buff, ret_buf);

  return (ret_buf);
}

int shbuf_sprintf(shbuf_t *buff, char *fmt, ...)
{
  va_list ap;
  char *str = NULL;
  char tfmt[256];
  int ret_len;
  int is_escape;
  int len;
  int i, j;

  shbuf_clear(buff);

  if (!fmt)
    return (0);

  ret_len = 0;
  is_escape = FALSE;
  va_start(ap, fmt);
  for(i=0;i<strlen(fmt);i++) {
    if (!is_escape) {
      if (fmt[i] == '%') {
        strcpy(tfmt, "%");
        is_escape = TRUE;
        continue;
      }
      shbuf_cat(buff, fmt + i, 1);
      ret_len++;
      continue;
    } 

    sprintf(tfmt+strlen(tfmt), "%c", fmt[i]);
    switch(fmt[i]) {
      case '%':
        shbuf_catstr(buff, "%");
        is_escape = FALSE;
        ret_len++;
        break;

      case 's':
        str = (char *)va_arg(ap, char *);
        len = strlen(str) * 2;
        shbuf_growmap(buff, ret_len + len);
        len = vsnprintf(buff->data + buff->data_of, len, tfmt, ap);
        ret_len += len;
        buff->data_of += len;
        is_escape = FALSE;
        break;

      case 'd':
      case 'u':
      case 'x':
      case 'c':
        shbuf_growmap(buff, ret_len + 64);
        len = vsnprintf(buff->data + buff->data_of, 64, tfmt, ap);
        ret_len += len;
        buff->data_of += len;
        is_escape = FALSE;
        break;
    }
  }
  va_end(ap);

  return (ret_len);
}
 


#undef __MEM__SHMEM_BUF_C__








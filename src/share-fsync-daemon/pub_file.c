
#include "pub_server.h"



shmeta_t *_pubd_file_map;
shfs_t *_pubd_fs;

void pubd_file_init(void)
{
  _pubd_fs = shfs_init(NULL);
  _pubd_file_map = shmeta_init();
}
void pubd_file_free(void)
{
  shmeta_free(&_pubd_file_map);
  shfs_free(&_pubd_fs);
}

char *pubd_shfs_path(pubuser_t *u, char *path)
{
  static char sh_path[PATH_MAX+1];
  char suffix[PATH_MAX+1];

  if (shkey_cmp(&u->id, ashkey_blank()))
    return (NULL);

  memset(suffix, 0, sizeof(suffix));
  strncpy(suffix, path + strlen(u->root_path), sizeof(suffix)-1);
  sprintf(sh_path, "/pub/identity/%s/%s", shkey_hex(&u->id), suffix);

  return (sh_path);
}

int pubd_file_sync(pubuser_t *u, pubfile_t *f, SHFL *fl)
{
  struct utimbuf ti;
  struct stat st;
  int err;

  err = shfs_fstat(fl, &st);
fprintf(stderr, "DEBUG: pubd_file_sync: %d = shfs_fstat(): <%d bytes>\n", err, st.st_size);
  if (err)
    return (err);

  ti.actime = st.st_atime;
  ti.modtime = st.st_mtime;
  err = utime(f->path, &ti);
  if (err)
    return (err);

  f->stamp = st.st_mtime;
  f->size = st.st_size;
  f->stat.sync_tot++;

  return (0);
}

int pubd_file_upload(pubuser_t *u, pubfile_t *f, char *path, time_t stamp)
{
  struct stat st;
  SHFL *fl;
  shbuf_t *buff;
  char *sh_path;
  unsigned char *data;
  size_t data_len;
  int err;

fprintf(stderr, "DEBUG: pubd_file_upload: %s\n", path);

  sh_path = pubd_shfs_path(u, path);
fprintf(stderr, "DEBUG: pubd_file_upload; %x = pubd_shfs_path()\n", sh_path);
  if (!sh_path)
    return (SHERR_AGAIN);

  fl = shfs_file_find(_pubd_fs, sh_path);
  err = shfs_fstat(fl, &st);
  if (err)
    return (err); 
  
#if 0
  if (st.st_mtime > stamp) {
/* DEBUG: todo: convert to shfs_dir_list.. */
    f->stamp = st.st_mtime;
    return (0); /* shnet has fresher data */
  }
#endif

  err = shfs_read_mem(path, (char **)&data, &data_len);
  if (err)
    return (err);

  buff = shbuf_init();
  shbuf_cat(buff, data, data_len);
  err = shfs_write(fl, buff); 
  shbuf_free(&buff);
  if (err)
    return (err);

  pubd_file_sync(u, f, fl);

fprintf(stderr, "DEBUG: pubd_file_upload: wrote %d bytes to shfs '%s'\n", data_len, sh_path);

  return (0);
}

int pubd_file_download(pubuser_t *u, pubfile_t *f, char *path)
{
  struct stat st;
  SHFL *fl;
  shbuf_t *buff;
  char *sh_path;
  unsigned char *data;
  size_t data_len;
  int err;

fprintf(stderr, "DEBUG: pubd_file_download: %s\n", path);

  sh_path = pubd_shfs_path(u, path);
  if (!sh_path)
    return (SHERR_AGAIN); /* no identity established */

  fl = shfs_file_find(_pubd_fs, sh_path);
  err = shfs_fstat(fl, &st);
  if (err)
    return (err);

  if (f->stamp == st.st_mtime)
    return (0); /* nothing changed */

  buff = shbuf_init();
  err = shfs_read(fl, buff);
  if (err) {
    shbuf_free(&buff);
    return (err);
  }

  err = shfs_write_mem(path, shbuf_data(buff), shbuf_size(buff));
  shbuf_free(&buff);
  if (err)
    return (err);

fprintf(stderr, "DEBUG: pubd_file_upload: wrote %d bytes to disk path '%s'\n", data_len, path);

  pubd_file_sync(u, f, fl);

  return (0);
}


void pubd_file_verify(pubuser_t *u, char *path)
{
  pubfile_t *f;
  struct stat st;
  shkey_t *key;
  int err;
  
  err = stat(path, &st);
fprintf(stderr, "DEBUG: pubd_file_verify: %d = stat(%s)\n", err, path);
  if (err)
    return;

  if (!S_ISREG(st.st_mode))
    return;

  key = shkey_str(path);

  f = (pubfile_t *)shmeta_get_ptr(_pubd_file_map, key);
  if (!f) {
    f =  (pubfile_t *)calloc(1, sizeof(pubfile_t));
    strncpy(f->path, path, sizeof(f->path) - 1);
    shmeta_set_ptr(_pubd_file_map, key, f);
  }

  if (f->stamp != st.st_mtime) {
    /* file changed in home dir */
    pubd_file_upload(u, f, path, st.st_mtime); 
  } else {
    time_t now = time(NULL);
    if (f->scan_t < (now - MAX_PUBFILE_REFRESH_TIME)) {
      pubd_file_download(u, f, path);
      f->scan_t = now;
    }
  }

  shkey_free(&key);

}


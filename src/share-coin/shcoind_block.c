
#include "shcoind.h"

shfs_t *block_fs;
//static shpeer_t *block_peer;




char *block_load(int block_height)
{
  shfs_ino_t *file;
  char path[PATH_MAX+1];
  char *data;
  size_t data_len;
  int err;

  /* save entire block using hash as reference */
  sprintf(path, "/block/index/%d", block_height);
  file = shfs_file_find(block_fs, path);
  if (!file) {
    printf ("DEBUG: JSON ERROR[no file]: %s\n", path);
    return (NULL);
  }

  err = shfs_file_read(file, &data, &data_len);
  if (err) {
    printf ("DEBUG: JSON ERROR[err %d, readfile]: %s\n", err, path);
    return (NULL);
  }

  printf ("[READ : stat %d] Block index %d is hash '%s'.\n", err, block_height, data);

  return (data);
}



void block_init(void)
{
  shpeer_t *peer;
  shfs_ino_t *file;
int err;

  if (!block_fs) {
fprintf(stderr, "DEBUG: block_init()\n");
    /* public database 
    block_peer = shpeer_pub();
    block_fs = shfs_init(block_peer);
*/
    block_fs = shfs_init(NULL);
  }

#if 0
  file = shfs_file_find(block_fs, "/init/stamp");
  err = shfs_file_write(file, "stamp", strlen("stamp"));
fprintf(stderr, "DEBUG: %d = shfs_file_write()\n", err);
#endif

fprintf(stderr, "DEBUG: [FILE WRITE START] 'stamp'\n");
  file = shfs_file_find(block_fs, "/stamp");
  err = shfs_file_write(file, "stamp", strlen("stamp"));
fprintf(stderr, "DEBUG: %d = shfs_file_write()\n", err);
fprintf(stderr, "DEBUG: [FILE WRITE END] 'stamp'\n");

file = shfs_file_find(block_fs, "/");
if (file->blk.hdr.type == SHINODE_DIRECTORY) {
    shbuf_t *buff;
    buff = shbuf_init();
    err = shfs_link_list(file, buff);
fprintf(stderr, "DEBUG; %d = shfs_link_list: %s\n", err, shbuf_data(buff));
    shbuf_free(&buff);
  }

/*
  shfs_free(&block_fs);
    block_fs = shfs_init(NULL);
*/
}

void block_close(void)
{
  if (block_fs) {
    shfs_free(&block_fs);
fprintf(stderr, "DEBUG: closed block fs\n");
  }

}



int block_save(int block_height, const char *json_str)
{
  char path[PATH_MAX+1];
  shfs_ino_t *file;
  int err;

  /* save entire block using hash as reference */
  sprintf(path, "/block/%d.json", block_height);
  file = shfs_file_find(block_fs, path);
  if (!file) {
    printf ("DEBUG: JSON ERROR[no file]: '%s'.\n", json_str);
    return (SHERR_INVAL);
  }

  err = shfs_file_write(file, json_str, strlen(json_str));

  printf ("[WRITE : stat %d] Block index %d (%d bytes): %s\n", err, block_height, strlen(json_str), shfs_inode_print(file));

  return (err);
}




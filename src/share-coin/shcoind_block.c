
#include "shcoind.h"

shfs_t *block_fs;




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
    peer = shpeer_init("shcoind", NULL);
    block_fs = shfs_init(peer);
    shpeer_free(&peer);
  }

}

void block_close(void)
{
  if (block_fs) {
    shfs_free(&block_fs);
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




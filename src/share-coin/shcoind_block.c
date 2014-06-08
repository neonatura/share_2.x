
#include "shcoind.h"

static shfs_t *block_fs;
static shpeer_t *block_peer;

static void block_init(void);

int block_save_index(int height, char *block_hash)
{
  char path[PATH_MAX+1];
  shfs_ino_t *file;
  int err;

  sprintf(path, "/block/index/%d", height);
  file = shfs_file_find(block_fs, path);
  if (!file) {
    printf ("DEBUG: JSON ERROR[no file]: %s\n", path);
    return (SHERR_INVAL);
  }

  err = shfs_file_write(file, block_hash, strlen(block_hash));
  printf ("[WRITE : stat %d] Mapped index %d to hash '%s'.\n", err, height, block_hash);

  return (err);
}

int block_save(const char *json_str)
{
  shjson_t *tree;
  shjson_t *block;
  shfs_ino_t *file;
  shbuf_t *buf;
  int block_height;
  char path[PATH_MAX+1];
  char *block_hash;
  long height;
  int err;

  block_init();
 
  tree = shjson_init((char *)json_str);
  if (!tree) {
    printf ("DEBUG: JSON ERROR[unable to parse]: '%s'.\n", json_str);
    return (SHERR_INVAL);
  }
 
  block = shjson_obj(tree, "result");

  block_hash = shjson_astr(block, "hash", NULL);
  if (!block_hash) {
    printf ("DEBUG: JSON ERROR[no hash]: '%s'.\n", json_str);
    shjson_free(&tree);
    return (SHERR_INVAL);
  }

  /* map block index to hash for lookup */
  block_height = (long)shjson_num(block, "height", -1);
  if (block_height == -1) {
    printf ("DEBUG: JSON ERROR[no height]: '%s'.\n", json_str);
    shjson_free(&tree);
    return (SHERR_INVAL);
  }

  err = block_save_index(block_height, block_hash);
  if (err) {
    printf ("DEBUG: JSON ERROR[%d - save height]: '%s'.\n", err, json_str);
    shjson_free(&tree);
  }

  /* save entire block using hash as reference */
  sprintf(path, "/block/%s.json", block_hash);
  file = shfs_file_find(block_fs, path);
  if (!file) {
    printf ("DEBUG: JSON ERROR[no file]: '%s'.\n", json_str);
    shjson_free(&tree);
    return (SHERR_INVAL);
  }

  err = shfs_file_write(file, json_str, strlen(json_str));
  shjson_free(&tree);

  printf ("[WRITE : stat %d] Block index %d (%d bytes): %s\n", err, block_height, strlen(json_str), shfs_inode_print(file));
 
  return (err);
}

char *block_load_hash(int block_height)
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


int block_load(int block_height)
{
  shjson_t *block;
  shfs_ino_t *file;
  shbuf_t *buf;
  char path[PATH_MAX+1];
  char *block_hash;
  char *data;
  size_t data_len;
  int err;

  block_init();
 
  /* load block hash from index. */
  block_hash = block_load_hash(block_height);
  if (!block_hash) {
    printf ("DEBUG: JSON ERROR[no index file]: %d\n", block_height);
    return (SHERR_INVAL);
  }

  /* save entire block using hash as reference */
  sprintf(path, "/block/%s.json", block_hash);
  free(block_hash);
  file = shfs_file_find(block_fs, path);
  if (!file) {
    printf ("DEBUG: JSON ERROR[no file]: %s\n", path);
    return (SHERR_INVAL);
  }

  data = NULL;
  err = shfs_file_read(file, &data, &data_len);

  printf ("[READ : stat %d] Block index %d (%d bytes): %s\n", err, block_height, data_len, shfs_inode_print(file));
  if (data)
    free(data);
 
  return (err);
}

static void block_init(void)
{
  shpeer_t *peer;

  if (!block_fs) {
fprintf(stderr, "DEBUG: block_init()\n");
    /* public database */
    block_peer = shpeer_pub();
    block_fs = shfs_init(block_peer);
  }

}

void block_close(void)
{
  if (block_fs) {
    shfs_free(&block_fs);
  }

}

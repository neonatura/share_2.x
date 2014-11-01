
#include "shcoind.h"

#ifdef __cplusplus
extern "C" {
#endif

void get_rpc_cred(char *username, char *password)
{
  shpeer_t *peer;
  shfs_t *tree;
  shfs_ino_t *fl;  
  shkey_t *app_key;
  size_t data_len;
  int err;

  peer = shpeer_app("shcoind");

  tree = shfs_init(peer);
  fl = shfs_file_find(tree, "/config/cred");
  err = shfs_file_read(fl, &app_key, &data_len);

  if (err || data_len != sizeof(shkey_t)) {
    if (app_key)
      free (app_key);

    /* generate new password */
    app_key = shkey_uniq();
    shfs_file_write(fl, app_key, sizeof(shkey_t));

  }

  shfs_free(&tree);

  strcpy(username, shkey_print(&peer->name));
  strcpy(password, shkey_print(app_key));


  shkey_free(&app_key);

  if (err || data_len != sizeof(shkey_t)) {
    char path[PATH_MAX+1];
    char buf[1024];

    sprintf(path, "%s/usde/", get_libshare_path());
    mkdir(path, 0777);
    strcat(path, "usde.conf");
    sprintf(buf, "rpcuser=%s\nrpcpassword=%s\n", username, password); 
    shfs_write_mem(path, buf, strlen(buf));
  }

}

#ifdef __cplusplus
}
#endif

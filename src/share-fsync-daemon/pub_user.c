

#include "pub_server.h"
#include "bits.h"

pubuser_t *_pubd_users;
int MAX_PUBUSER_NAME_LENGTH;

static int _pubd_msgqid;

pubuser_t *pubd_user_add(char *uname, char *upass, char *path)
{
  pubuser_t *u;

  /* ensure this is unique path */
  for (u = _pubd_users; u; u = u->next) {
    if (0 == strcmp(u->root_path, path))
      break;
  }
  if (u)
    return (NULL); /* initial user owns path */

  for (u = _pubd_users; u; u = u->next) {
    if (0 == strcmp(u->name, uname))
      break;
  }
  if (!u) {
    u = (pubuser_t *)calloc(1, sizeof(pubuser_t));
    if (!u)
      return (NULL);

    strncpy(u->name, uname, sizeof(u->name) - 1);
  }

  strncpy(u->pass, upass, sizeof(u->pass) - 1);
  strncpy(u->root_path, path, sizeof(u->root_path) - 1);

  pubd_user_generate(u);

  u->next = _pubd_users;
  _pubd_users = u;

fprintf(stderr, "DEBUG: pubd_user_add[%x]: uname(%s) upass(%s) path(%s)\n", u, uname, upass, path);

  return (u);
}

int pubd_user_validate(pubuser_t *u, char *pass)
{
  char *enc;

  if (!u)
    return (SHERR_INVAL);

  if (!pass)
    pass = "";

  if (strlen(u->pass) >= 2) {
    enc = crypt(pass, u->pass);
    if (0 != strcmp(enc, u->pass))
      return (SHERR_ACCESS);
  }

  return (0);
}

/** Generate account identity with shared server */
int pubd_user_generate(pubuser_t *u)
{
  shkey_t *id_key;
  shkey_t *pass_key;
  uint32_t mode;
  int err;

  err = shapp_account(u->name, u->pass, &pass_key);
  if (err)
    return (err);

  err = shapp_ident(pass_key, u->name, &id_key); 
  shkey_free(&pass_key);
  if (err)
    return (err);

  /* retain identity name key */
  memcpy(&u->id, id_key, sizeof(shkey_t)); 
  shkey_free(&id_key);

  return (0);
}




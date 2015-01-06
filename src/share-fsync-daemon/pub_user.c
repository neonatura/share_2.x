

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
  tx_account_msg_t m_acc;
  tx_account_t acc;
  tx_id_msg_t m_id;
  tx_id_t id;
  shbuf_t *buff;
  shkey_t *name_key;
  char acc_name[MAX_SHARE_NAME_LENGTH];
  char acc_pass[MAX_SHARE_PASS_LENGTH];
  uint32_t mode;
  int err;

  memset(acc_name, 0, sizeof(acc_name));
  memset(acc_pass, 0, sizeof(acc_pass));

  if (!_pubd_msgqid)
    _pubd_msgqid = shmsgget(NULL);

  /* generate account */
  memset(&acc, 0, sizeof(acc));
  strncpy(acc.acc_label, acc_name, sizeof(acc.acc_label) - 1);
  name_key = shkey_bin((char *)&acc, sizeof(acc));
  memcpy(&acc.acc_name, name_key, sizeof(shkey_t));
  shkey_free(&name_key);
  if (*acc_pass)
    memcpy(&acc.acc_key, ashkey_str(acc_pass), sizeof(shkey_t));

  memset(&m_acc, 0, sizeof(m_acc));
  memcpy(&m_acc, &acc.acc_key, sizeof(shkey_t));
  strncpy(m_acc.acc_label, acc.acc_label, sizeof(m_acc.acc_label) - 1);

  /* notify server of account */
  mode = TX_ACCOUNT;
  buff = shbuf_init();
  shbuf_cat(buff, &mode, sizeof(mode));
  shbuf_cat(buff, &m_acc, sizeof(m_acc));
  err = shmsg_write(_pubd_msgqid, buff, NULL);
  shbuf_free(&buff);
  if (err)
    return (err);

  /* generate identity */
  memset(&id, 0, sizeof(id));
  memcpy(&id.id_acc, &acc.acc_name, sizeof(shkey_t));
  strncpy(id.id_label, u->name, sizeof(id.id_label) - 1);
  memcpy(&id.id_app, shpeer_kpub(_pubd_peer), sizeof(shkey_t));
  name_key = shkey_bin(&id, sizeof(id));
  memcpy(&id.id_name, name_key, sizeof(shkey_t));
  shkey_free(&name_key);

  memset(&m_id, 0, sizeof(m_id));
  memcpy(&m_id.id_peer, _pubd_peer, sizeof(shpeer_t));
  memcpy(&m_id.id_acc, &acc.acc_name, sizeof(shkey_t));
  strcpy(m_id.id_label, id.id_label);
  strncpy(m_id.id_hash, u->pass, sizeof(m_id.id_hash) - 1);

  /* notify server of identity. */
  mode = TX_IDENT;
  buff = shbuf_init();
  shbuf_cat(buff, &mode, sizeof(mode));
  shbuf_cat(buff, &m_id, sizeof(m_id));
  err = shmsg_write(_pubd_msgqid, buff, NULL);
  if (err)
    return (err);

  /* retain identity name key */
  memcpy(&u->id, &id.id_name, sizeof(shkey_t)); 

  return (0);
}




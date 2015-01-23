
/*
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
*/  

#include "share.h"

/**
 * @param flags Bitvector flags such as SHAPP_LOCAL.
 */
shpeer_t *shapp_init(char *exec_path, char *host, int flags)
{
  shpeer_t *peer;
  char *app_name;
  char hostbuf[MAXHOSTNAMELEN+1];
  char ebuf[4096];

  app_name = shfs_app_name(exec_path);
  peer = shpeer_init(app_name, NULL);
  if (!peer)
    return (NULL);
  shpeer_set_default(peer);

  if (!(flags & SHAPP_LOCAL)) {
    if (!host) {
      memset(hostbuf, 0, sizeof(hostbuf));
      gethostname(hostbuf, sizeof(hostbuf) - 1);
      if (gethostbyname(hostbuf) != NULL)
        host = hostbuf;
    }

    shpeer_t *priv_peer = shpeer_init(app_name, host);
    shapp_register(priv_peer);
    shpeer_free(&priv_peer);
  }

  if (0 != strcasecmp(app_name, PACKAGE)) {
#ifdef linux
    FILE *fl;
    char path[PATH_MAX+1];

    sprintf(path, "/var/run/%s.pid", app_name); 
    fl = fopen(path, "wb");
    fprintf(fl, "%u\n", (unsigned)getpid());
    fclose(fl);
#endif
  }

  sprintf(ebuf, "initialized '%s' as peer %s", exec_path, shpeer_print(peer));
  shinfo(ebuf);

  return (peer);
}

int shapp_register(shpeer_t *peer)
{
  shbuf_t *buff;
  char data[256];
  size_t data_len;
  uint32_t mode;
  int qid;
  int err;

  if (!peer)
    peer = ashpeer();

  /* open message queue to share daemon */
  qid = shmsgget(NULL);
  if (qid < 0)
    return (qid);

  /* send a 'peer transaction' operation request. */
  mode = TX_APP;
  buff = shbuf_init();
  shbuf_cat(buff, &mode, sizeof(mode));
  shbuf_cat(buff, peer, sizeof(shpeer_t));
  err = shmsg_write(qid, buff, NULL);
  shbuf_free(&buff);
  if (err)
    return (err);

  /* close message queue */
  shmsgctl(qid, SHMSGF_RMID, TRUE);

  return (0);
}

int shapp_listen(int tx, shpeer_t *peer)
{
  shbuf_t *buff;
  uint32_t mode;
  uint32_t listen_tx;
  int qid;

  mode = TX_LISTEN;
  listen_tx = (uint32_t)tx;

  buff = shbuf_init();
  shbuf_cat(buff, &mode, sizeof(mode));
  shbuf_cat(buff, &listen_tx, sizeof(uint32_t));
  if (peer)
    shbuf_cat(buff, peer, sizeof(shpeer_t));

  /* open message queue to share daemon. */
  qid = shmsgget(NULL);
  shmsg_write(qid, buff, NULL);
  shbuf_free(&buff);
}

int shapp_account(const char *username, const char *passphrase,
    shkey_t **user_key_p, shkey_t **pass_key_p)
{
  shbuf_t *buff;
  shkey_t *user_key;
  shkey_t *pass_key;
  uint32_t mode;
  int qid;
  int err;

  user_key = shpam_user_gen(username);
  pass_key = shpam_seed_gen(user_key, passphrase);

  mode = TX_ACCOUNT;
  buff = shbuf_init();
  shbuf_cat(buff, &mode, sizeof(uint32_t));
  shbuf_cat(buff, user_key, sizeof(shkey_t));
  shbuf_cat(buff, pass_key, sizeof(shkey_t));
  /* open message queue to share daemon. */
  qid = shmsgget(NULL);
  err = shmsg_write(qid, buff, NULL);
  shbuf_free(&buff);

  if (user_key_p)
    memcpy(*user_key_p, user_key, sizeof(shkey_t));
  if (pass_key_p)
    memcpy(*pass_key_p, pass_key, sizeof(shkey_t));
  shkey_free(&user_key);
  shkey_free(&pass_key);

  return (err);
}

int shapp_ident(shkey_t *id_seed, char *id_label, char *id_hash, shkey_t **id_key_p)
{
  tx_id_msg_t m_id;
  shpeer_t id_peer;
  shkey_t *id_key;
  shbuf_t *buff;
  uint32_t mode;
  int q_id;
  int err;

  if (id_key_p) {
    *id_key_p = NULL;
  }

  /* generate identity */
  memcpy(&id_peer, ashpeer(), sizeof(shpeer_t));
  id_key = shpam_ident_gen(&id_peer, id_seed, id_label); 
  if (!id_key)
    return (SHERR_INVAL);

  memset(&m_id, 0, sizeof(m_id));
  memcpy(&m_id.id_seed, &id_seed, sizeof(shkey_t));
  strncpy(m_id.id_label, id_label, sizeof(m_id.id_label) - 1);
  strncpy(m_id.id_hash, id_hash, sizeof(m_id.id_hash) - 1);

  /* notify server of identity. */
  mode = TX_IDENT;
  buff = shbuf_init();
  shbuf_cat(buff, &mode, sizeof(mode));
  shbuf_cat(buff, &m_id, sizeof(m_id));
  q_id = shmsgget(NULL);
  err = shmsg_write(q_id, buff, NULL);
  if (err)
    return (err);

  if (id_key_p) {
    memcpy(*id_key_p, id_key, sizeof(shkey_t));
  }

  return (0);
}

int shapp_session(shkey_t *seed_key, shkey_t *id_key, shkey_t **sess_key_p)
{

  return (0);
}

int shapp_account_create(char *acc_name, char *acc_pass, char *id_label)
{

  return (0);
}

int shapp_account_login(char *acc_name, char *acc_pass, shkey_t **sess_key_p)
{

  return (0);
}

int shapp_account_remove(char *acc_name, char *acc_pass)
{

  return (0);
}


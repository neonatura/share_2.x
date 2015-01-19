
/*
 * @copyright
 *
 *  Copyright 2014 Neo Natura
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

#include "config.h"
#include "share.h"
#include "bits.h"

#define RUN_NONE 0
#define RUN_LIST 1
#define RUN_GENERATE 2
#define RUN_VERIFY 3
#define RUN_LIST_IDENT 4

#define PROGRAM_NAME "shacc"


static int run_mode;
static char prog_name[PATH_MAX+1];
static shfs_t *fs_tree;
static int _auth_msgqid;


/**
 * Displays the program's version information to the command console.
 */
void program_version(void)
{
  printf ("%s version %s (%s)\n"
      "\n"
      "Copyright 2014 Neo Natura\n"
      "Licensed under the GNU GENERAL PUBLIC LICENSE Version 3\n"
      "Visit 'https://github.com/neonatura/share' for more information.\n",
      prog_name, PACKAGE_VERSION, PACKAGE_NAME);
}

/**
 * Displays the program's usage information to the command console.
 */
void program_usage(void)
{
  printf (
    "%s version %s (%s)\n"
    "usage: %s [-h|--help] [-v|--version] [-g|--gen] [-c <name>[@host:port]] [-u <name>] [-p <phrase>] [-i <name>] [-a <hash>\n"
    "\n"
    "Command-line arguments:\n"
    "  [-g|--gen]\t\t\tGenerate a new account identity.\n"
    "  -u <name>\t\tThe account username.\n"
    "  -p <phrase>\t\tThe account password.\n"
    "  -c <name>\t\tThe application name used by the identity.\n"
    "  -i <name>\t\tThe identity's alias name.\n"
    "  -a <hash>\t\tAuxillary hash code for identity.\n"
    "\n"
    "Example of storing, verifying, and retrieiving account identities.\n"
    "\n"
    "Visit 'https://github.com/neonatura/share' for more information.\n",
    prog_name, PACKAGE_VERSION, PACKAGE_NAME, prog_name);
}

tx_account_t **shacc_account_load_all(void)
{
  return (NULL);
}

#if 0
tx_account_t *shacc_account_load(char *username)
{
  SHFL *file;
  char path[PATH_MAX+1];
  unsigned char *data;
  size_t data_len;
  int err;

  if (!username)
    username = "";
  
  sprintf(path, "/account/%s", username);
  file = shfs_file_find(fs_tree, path);
  err = shfs_file_read(file, &data, &data_len);
  if (!err && data_len == sizeof(tx_account_t))
    return ((tx_account_t *)data);

  return (NULL);
}
#endif

tx_account_t *shacc_account(char *user, char *pass)
{
  shpeer_t *self_peer = ashpeer();
  tx_account_t *acc;
  shkey_t *name_key;
  shkey_t *pass_key;
  shbuf_t *buff;
  SHFL *file;
  char path[PATH_MAX+1];;
  int err;

  acc = (tx_account_t *)calloc(1, sizeof(tx_account_t));
  if (!acc)
    return (NULL);

  if (*user)
    strncpy(acc->acc_label, user, sizeof(acc->acc_label) - 1);

  name_key = shkey_bin((char *)acc, sizeof(tx_account_t));
  memcpy(&acc->acc_name, name_key, sizeof(shkey_t));
  shkey_free(&name_key);

  if (*pass) {
    pass_key = shkey_bin(pass, strlen(pass));
    memcpy(&acc->acc_key, pass_key, sizeof(shkey_t));
    shkey_free(&pass_key);
  }

  /* write to disk for listing */
  sprintf(path, "/account/%s", shkey_hex(&acc->acc_name));
  file = shfs_file_find(fs_tree, path);
  buff = shbuf_init();
  shbuf_cat(buff, acc, sizeof(tx_account_t));
  shfs_write(file, buff);
  shbuf_free(&buff);

  return (acc); 
}

/** Request an account be confirmed with the shared server. */
void shacc_account_request(tx_account_t *acc)
{
  tx_account_msg_t m_acc;
  shbuf_t *buff;
  uint32_t mode;

  memset(&m_acc, 0, sizeof(m_acc));
  memcpy(&m_acc.acc_key, &acc->acc_key, sizeof(shkey_t));
  strncpy(m_acc.acc_label, acc->acc_label, sizeof(m_acc.acc_label) - 1);

  mode = TX_ACCOUNT;
  buff = shbuf_init();
  shbuf_cat(buff, &mode, sizeof(mode));
  shbuf_cat(buff, &m_acc, sizeof(m_acc));

  if (!_auth_msgqid) 
    _auth_msgqid = shmsgget(NULL);

  /* ship it to the server */
  shmsg_write(_auth_msgqid, buff, NULL);
  shbuf_free(&buff);
}

void shacc_account_print(tx_account_t *acc)
{
  char acc_key[256];
  char acc_name[256];

  strcpy(acc_key, shkey_print(&acc->acc_key));
  strcpy(acc_name, shkey_print(&acc->acc_name));
  printf("[ACCOUNT %s] '%s' (pass '%s')\n",
      *acc->acc_label ? acc->acc_label : "<empty>",
      acc_name, acc_key);
}

void shacc_identity_fill(tx_id_t *id, 
    tx_account_t *acc, shpeer_t *peer, char *id_name)
{
  shkey_t *key;

  memset(id, 0, sizeof(tx_id_t));

  memcpy(&id->id_acc, &acc->acc_name, sizeof(shkey_t));
  strncpy(id->id_label, id_name, sizeof(id->id_label) - 1); 
  memcpy(&id->id_peer, peer, sizeof(shpeer_t));
  
  key = shkey_bin((char *)id, sizeof(tx_id_t)); 
  memcpy(&id->id_name, key, sizeof(shkey_t));
  shkey_free(&key);

}

tx_id_t **shacc_identity_load_all(tx_account_t *acc)
{
  return (NULL);
}

tx_id_t *shacc_identity_load(shkey_t *app_key, char *id_name)
{
  SHFL *file;
  shbuf_t *buff;
  char path[PATH_MAX+1];
  int err;

  if (!id_name)
    id_name = "";
  
  buff = shbuf_init();
  sprintf(path, "/identity/%s/%s", shkey_hex(app_key), id_name);
  file = shfs_file_find(fs_tree, path);
  err = shfs_read(file, buff);
  if (!err && shbuf_size(buff) == sizeof(tx_id_t)) {
    return ((tx_id_t *)shbuf_unmap(buff));
  }
  shbuf_free(&buff);

  return (NULL);
}

int shacc_identity_save(tx_id_t *id)
{
  SHFL *file;
  char path[PATH_MAX+1];
  shbuf_t *buff;
  int err;
  
  sprintf(path, "/identity/%s/%s", shkey_hex(shpeer_kpub(&id->id_peer)), id->id_label);
  file = shfs_file_find(fs_tree, path);
  buff = shbuf_init();
  shbuf_cat(buff, id, sizeof(tx_id_t));
  err = shfs_write(file, buff);
  shbuf_free(&buff);
  if (err)
    return (err);

  return (0);
}

void shacc_identity_request(tx_id_t *id, shpeer_t *peer)
{
  tx_id_msg_t m_id;
  shbuf_t *buff;
  uint32_t mode;

  memset(&m_id, 0, sizeof(m_id));
  memcpy(&m_id.id_peer, peer, sizeof(shpeer_t));
  memcpy(&m_id.id_acc, &id->id_acc, sizeof(shkey_t));
  strncpy(m_id.id_label, id->id_label, sizeof(m_id.id_label) - 1);
  strncpy(m_id.id_hash, id->id_hash, sizeof(m_id.id_hash) - 1);

  mode = TX_IDENT;
  buff = shbuf_init();
  shbuf_cat(buff, &mode, sizeof(mode));
  shbuf_cat(buff, &m_id, sizeof(m_id));

  if (!_auth_msgqid) 
    _auth_msgqid = shmsgget(NULL);

  /* ship it to the server */
  shmsg_write(_auth_msgqid, buff, NULL);
  shbuf_free(&buff);

  shacc_identity_save(id);
}

tx_id_t *shacc_identity_gen(tx_account_t *acc, shpeer_t *peer, char *id_name, char *id_hash) 
{
  tx_id_t *id;
  shkey_t *sig_key;
  long crc = 0; /* server uses perm id tx hash */

  id = (tx_id_t *)calloc(1, sizeof(tx_id_t));
  if (!id)
    return (NULL);

  shacc_identity_fill(id, acc, peer, id_name); 

#if 0
  /* generate signature (sans tx hash) */
  id->id_sig.sig_stamp = shtime64();
  sig_key = shkey_cert(shpeer_kpriv(peer), crc, id->id_sig.sig_stamp);
  memcpy(&id->id_sig.sig_key, sig_key, sizeof(shkey_t));
#endif

  /* fill info */
  strncpy(id->id_hash, id_hash, sizeof(id->id_hash) - 1);

  shacc_identity_request(id, peer);

  return (id);
}

tx_id_t *shacc_identity(tx_account_t *acc, shpeer_t *peer, char *id_name)
{
  SHFL *file;
  tx_id_t t_id;
  tx_id_t *id;
  shkey_t *id_key;
  shbuf_t *buff;
  unsigned char *data;
  char path[PATH_MAX+1];
  size_t data_len;
  long crc = 0;
  int err;

  shacc_identity_fill(&t_id, acc, peer, id_name);
  sprintf(path, "/id/%s", shkey_hex(&t_id.id_name));
  file = shfs_file_find(fs_tree, path);
  err = shfs_read(file, buff);
  if (err || shbuf_size(buff) < sizeof(tx_id_t)) {
    shbuf_free(&buff);
    return (NULL);
  }

  return ((tx_id_t *)shbuf_unmap(buff));
}

void shacc_identity_print(tx_id_t *id)
{
  char hash_sig[256];
  char hash_app[256];
  char hash_name[256];

  strcpy(hash_sig, shkey_print(&id->id_sig.sig_key));
  strcpy(hash_app, shkey_print(shpeer_kpub(&id->id_peer)));
  strcpy(hash_name, shkey_print(&id->id_name));
  printf("[IDENT %s] '%s' (sig '%s', app '%s', hash '%s', stamp '%20.20s')\n",
      *id->id_label ? id->id_label : "<empty>",
      hash_name, hash_sig, hash_app, 
      id->id_hash, shctime64(id->id_sig.sig_stamp)+4);

}

/** Creates an example template, excluding permanent transaction reference, of how the server generates a session for an identity. */
void fill_session(tx_session_t *sess, tx_id_t *id, double secs)
{
  shkey_t *id_key = &id->id_name;
  shkey_t *sig_key = &id->id_sig.sig_key;
  shkey_t *key;

  memset(sess, 0, sizeof(tx_session_t));

  sess->sess_stamp = shtime64();
  sess->sess_expire = shtime64_adj(sess->sess_stamp, secs); 
  memcpy(&sess->sess_id, id_key, sizeof(shkey_t));
  memcpy(&sess->sess_cert, sig_key, sizeof(shkey_t));

#if 0
  key = shkey_bin(sess, sizeof(tx_session_t));
  memcpy(&sess->sess_tok, key, sizeof(shkey_t));
  shkey_free(&key);
#endif

}

/** Parse key returned from server TX_SESSION operation. */
void parse_session(tx_session_t *sess, shkey_t *sess_key)
{
  memcpy(&sess->sess_tok, sess_key, sizeof(shkey_t)); 
  printf("shacc: generated session token '%s'.\n", shkey_print(sess_key));
}

void shacc_session_request(tx_session_t *sess)
{
  shbuf_t *buff;
  uint32_t mode;

  mode = TX_SESSION;
  buff = shbuf_init();
  shbuf_cat(buff, &mode, sizeof(mode));
  shbuf_cat(buff, &sess->sess_id, sizeof(shkey_t)); 

  if (!_auth_msgqid) 
    _auth_msgqid = shmsgget(NULL);

  /* ship it to the server */
  shmsg_write(_auth_msgqid, buff, NULL);
  shbuf_free(&buff);
}

tx_session_t *shacc_session_load(shkey_t *id_key)
{
  SHFL *file;
  char path[PATH_MAX+1];
  unsigned char *data;
  size_t data_len;
  shbuf_t *buff;
  int err;

  if (!shkey_cmp(id_key, ashkey_blank())) {
    buff = shbuf_init();
    sprintf(path, "/session/%s", shkey_hex(id_key));
    file = shfs_file_find(fs_tree, path);
    err = shfs_read(file, buff);
    if (!err && shbuf_size(buff) == sizeof(tx_session_t))
      return ((tx_session_t *)shbuf_unmap(buff));
    shbuf_free(&buff);
  }

  return (NULL);
}

int shacc_session_save(tx_session_t *sess)
{
  SHFL *file;
  shbuf_t *buff;
  char path[PATH_MAX+1];
  int err;
  
  sprintf(path, "/session/%s", shkey_hex(&sess->sess_id));
  file = shfs_file_find(fs_tree, path);
  buff = shbuf_init();
  shbuf_cat(buff, sess, sizeof(tx_session_t));
  err = shfs_write(file, buff);
  shbuf_free(&buff);
  return (err);
}

tx_session_t *shacc_session(tx_id_t *id, double secs)
{
  tx_session_t *sess;

  sess = shacc_session_load(&id->id_name);
  if (!sess || sess->sess_expire <= shtime64()) {
    if (sess)
      free(sess);

    sess = (tx_session_t *)calloc(1, sizeof(tx_session_t));
    if (!sess)
      return (NULL);

    fill_session(sess, id, secs);
    shacc_session_request(sess);
  }

  return (sess);
}

void shacc_msg_read(void)
{
  tx_account_t *acc;
  tx_session_t *sess;
  tx_id_t *id;
  shbuf_t *buff;
  shkey_t *acc_key;
  shkey_t *id_app;
  shsig_t *id_sig;
  shkey_t *id_key;
  shkey_t *tok_key;
  char *str;
  int err;

  if (!_auth_msgqid) 
    _auth_msgqid = shmsgget(NULL);

  buff = shbuf_init();
  err = shmsg_read(_auth_msgqid, NULL, buff);
  if (!err) {
    unsigned char *data = shbuf_data(buff);
    uint32_t mode = *((uint32_t *)data);

    switch (mode) {
      case TX_ACCOUNT:
        acc_key = (shkey_t *)(data + sizeof(uint32_t));
        printf ("Info: Server confirmed account '%s' (%s)\n", (data + sizeof(uint32_t) + sizeof(shkey_t)), shkey_print(acc_key)); 
#if 0
        acc_key = (shkey_t *)(data + sizeof(uint32_t));
        str = (char *)(data + sizeof(uint32_t) + sizeof(shkey_t));
        acc = shacc_account_load(str);
        memcpy(&acc->acc_name, &acc_key, sizeof(shkey_t));
#endif
        break;

      case TX_IDENT:
        id_app = (shkey_t *)(data + sizeof(uint32_t));
        id_sig = (shsig_t *)(data + sizeof(uint32_t) + sizeof(shkey_t));
        str = (char *)(data + sizeof(uint32_t) + 
            sizeof(shkey_t) + sizeof(shsig_t));
        id = shacc_identity_load(id_app, str);
        if (!id) {
          break;
        }

        memcpy(&id->id_sig, id_sig, sizeof(shsig_t));
        shacc_identity_save(id);

        printf ("Info: Server confirmed identity '%s' (%s)\n", str, shkey_print(&id->id_name));
        break;

      case TX_SESSION:
        if (shbuf_size(buff) < sizeof(uint32_t) + sizeof(shkey_t))
          break;

        id_key = (shkey_t *)(data + sizeof(uint32_t));
        tok_key = (shkey_t *)(data + sizeof(uint32_t) + sizeof(shkey_t));
        sess = shacc_session_load(id_key);
        if (sess) {
          memcpy(&sess->sess_tok, tok_key, sizeof(shkey_t));
          shacc_session_save(sess);
        }

        printf ("Info: Server confirmed session for identity '%s'.\n", shkey_print(id_key));
        break;
    }
  }

  shbuf_free(&buff);

}

void shacc_session_print(tx_session_t *sess)
{
  char sess_stamp[256]; 
  char sess_expire[256];

  if (!sess || shkey_cmp(&sess->sess_tok, ashkey_blank()))
    return;

  strcpy(sess_stamp, shctime64(sess->sess_stamp)+4);
  strcpy(sess_expire, shctime64(sess->sess_expire)+4);

  printf("[SESSION %s] birth(%-20.20s) expire(%20.20s)\n",
    shkey_print(&sess->sess_tok), sess_stamp, sess_expire);

}

/**
 * Performs key-store List, Generate, and Verify operations.
 */
int main(int argc, char **argv)
{
  tx_account_t *acc;
  tx_session_t *tok;
  tx_id_t *id;
  shfs_ino_t *file;
  unsigned char *k_data;
  size_t k_len;
  shpeer_t *peer;
  shpeer_t *proc_peer;
  shkey_t *key;
  shbuf_t *buff;
  char acc_user[256];
  char acc_pass[256];
  char acc_key[256];
  char acc_name[256];
  char acc_app[256];
  char id_app[256];
  char id_name[256];
  char id_hash[256];
  char id_host[256];
  char hash_app[256];
  char hash_sig[256];
  char hash_name[256];
  char path[PATH_MAX+1];
  char aux_hash[1024];
  char *ptr;
  int err;
  int i;

  memset(prog_name, 0, sizeof(prog_name));
  strncpy(prog_name, argv[0], sizeof(prog_name) - 1);

  run_mode = RUN_LIST;

  memset(acc_user, 0, sizeof(acc_user));
  memset(acc_pass, 0, sizeof(acc_pass));
  memset(id_app, 0, sizeof(id_app));
  memset(id_name, 0, sizeof(id_name));
  memset(id_host, 0, sizeof(id_host));
  memset(id_hash, 0, sizeof(id_hash));
  for (i = 1; i < argc; i++) {
    if (0 == strcmp(argv[i], "-h") ||
        0 == strcmp(argv[i], "--help")) {
      program_usage();
      return (0);
    }
    if (0 == strcmp(argv[i], "-v") ||
       0 == strcmp(argv[i], "--version")) {
      program_version();
      return (0);
    }
    if (0 == strcmp(argv[i], "-g") ||
        0 == strcmp(argv[i], "--gen")) {
      run_mode = RUN_GENERATE;
      continue;
    }
    if (0 == strcmp(argv[i], "-u")) {
      if ((i+1) < argc && argv[i+1][0] != '-') {
        strncpy(acc_user, argv[i+1], sizeof(acc_user) - 1);
        i++;
      }
      continue;
    }
    if (0 == strcmp(argv[i], "-p")) {
      if ((i+1) < argc && argv[i+1][0] != '-') {
        strncpy(acc_pass, argv[i+1], sizeof(acc_pass) - 1);
        i++;
      }
      continue;
    }
  
    if (0 == strcmp(argv[i], "-c")) {
      if ((i+1) < argc && argv[i+1][0] != '-') {
        strncpy(id_app, argv[i+1], sizeof(id_app) - 1);
        ptr = strchr(id_app, '@');
        if (ptr) {
          *ptr++ = '\0';
          strncpy(id_host, ptr, sizeof(id_host) - 1);
        }
        i++;
      }
      continue;
    }
    if (0 == strcmp(argv[i], "-i")) {
      if ((i+1) < argc && argv[i+1][0] != '-') {
        strncpy(id_name, argv[i+1], sizeof(id_name) - 1);
        i++;
      }
      continue;
    }
    if (0 == strcmp(argv[i], "-a")) {
      if ((i+1) < argc && argv[i+1][0] != '-') {
        strncpy(id_hash, argv[i+1], sizeof(id_hash) - 1);
        i++;
      }
    }
  }

  proc_peer = shapp_init(prog_name, NULL, 0);
  fs_tree = shfs_init(proc_peer);
  shpeer_free(&proc_peer);

  if (run_mode != RUN_GENERATE) {
    /* read pending message queue */
    shacc_msg_read();
  }

  if (run_mode == RUN_LIST && *acc_user) {
    run_mode = RUN_LIST_IDENT;
  }

  if (run_mode == RUN_LIST) {
    tx_account_t **acc_list = shacc_account_load_all();

    /* list all acounts */
    printf ("Accounts:\n");
    if (acc_list) {
      for (i = 0; acc_list[i]; i++) {
        shacc_account_print(acc);
        printf("\n");
      }
    }
    return (0);
  } 

  /* generate base account */
  acc = shacc_account(acc_user, acc_pass); 
  if (!acc) {
    printf ("No account found.\n");
    return (1);
  }

  if (run_mode == RUN_GENERATE) {
    /* notify server of account */
    shacc_account_request(acc);
  }

  /* print account*/
  shacc_account_print(acc);

  if (run_mode == RUN_LIST_IDENT) {
    tx_id_t **id_list = shacc_identity_load_all(acc);

    /* list all identity for account */
    printf ("Identities:\n");
    if (id_list) {
      for (i = 0; id_list[i]; i++) {
        shacc_identity_print(id);

        tok = shacc_session(id, 1440); /* 24 mins */
        shacc_session_print(tok);

        printf("\n");
      }
    }

    exit (0);
  }

  /* obtain identity */
  if (!*id_app)
    strncpy(id_app, PACKAGE_NAME, sizeof(id_app) - 1);
  peer = shpeer_init(id_app, id_host);
  if (run_mode == RUN_GENERATE) {
    /* generate new identity */
    id = shacc_identity_gen(acc, peer, id_name, id_hash); 
  } else {
    /* display current identity */
    id = shacc_identity(acc, peer, id_name);
  }
  if (!id) {
    printf ("No identity found.\n");
    return (1);
  }

  /* print identity */
  shacc_identity_print(id);

  tok = shacc_session(id, 1440); /* 24 mins */
  shacc_session_print(tok);

  return (0);
}

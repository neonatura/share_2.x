
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
    "usage: %s [-h|--help] [-v|--version] [-u <name>] [-p <phrase>]\n"
    "\n"
    "Command-line arguments:\n"
    "\t-u <name>\t\tThe account username.\n"
    "\t-p <phrase>\t\tThe account password.\n"
    "\n"
    "Example of storing, verifying, and retrieiving account identities in an auxillary file.\n"
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
  tx_account_t *acc;
  shseed_t *seed;
  int err;

  acc = (tx_account_t *)calloc(1, sizeof(tx_account_t));
  if (!acc)
    return (NULL);

  seed = shpam_pass_gen(user, pass, 0);
  memcpy(&acc->pam_seed, seed, sizeof(shseed_t));

  return (acc); 
}

/** Request an account be confirmed with the shared server. */
void shacc_account_request(tx_account_t *acc)
{
  tx_account_msg_t m_acc;
  shbuf_t *buff;
  uint32_t mode;

  memset(&m_acc, 0, sizeof(m_acc));
  memcpy(&m_acc.pam_seed, &acc->pam_seed, sizeof(shseed_t));

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

  printf("[ACCOUNT %llu]\n", acc->pam_seed.seed_uid);

}

void shacc_identity_fill(tx_id_t *id, 
    tx_account_t *acc, shpeer_t *peer, char *id_name)
{
  shkey_t *key;

  memset(id, 0, sizeof(tx_id_t));
  memcpy(&id->id_peer, peer, sizeof(shpeer_t));

  key = shpam_ident_gen(id->id_uid, peer);
  memcpy(&id->id_key, key, sizeof(shkey_t));
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
  
#if 0
  buff = shbuf_init();
  sprintf(path, "/identity/%s/%s", shkey_hex(app_key), id_name);
  file = shfs_file_find(fs_tree, path);
  err = shfs_read(file, buff);
  if (!err && shbuf_size(buff) == sizeof(tx_id_t)) {
    return ((tx_id_t *)shbuf_unmap(buff));
  }
  shbuf_free(&buff);
#endif

  return (NULL);
}

int shacc_identity_save(tx_id_t *id)
{
  SHFL *file;
  char path[PATH_MAX+1];
  shbuf_t *buff;
  int err;
  
#if 0
  sprintf(path, "/identity/%s/%s", shkey_hex(shpeer_kpub(&id->id_peer)), id->id_label);
  file = shfs_file_find(fs_tree, path);
  buff = shbuf_init();
  shbuf_cat(buff, id, sizeof(tx_id_t));
  err = shfs_write(file, buff);
  shbuf_free(&buff);
  if (err)
    return (err);
#endif

  return (0);
}

void shacc_identity_request(tx_id_t *id, shpeer_t *peer)
{
  shkey_t *id_key;
  int err;

  err = shapp_ident(id->id_uid, &id_key);
  if (err) {
    shkey_free(&id_key);
    return;
  }

  memcpy(&id->id_key, id_key, sizeof(shkey_t));
  shkey_free(&id_key);
  shacc_identity_save(id);
}

tx_id_t *shacc_identity_gen(tx_account_t *acc, shpeer_t *peer, char *id_name)
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
  id->id_sig.sig_stamp = shtime();
  sig_key = shkey_cert(shpeer_kpriv(peer), crc, id->id_sig.sig_stamp);
  memcpy(&id->id_sig.sig_key, sig_key, sizeof(shkey_t));
#endif

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

#if 0
  shacc_identity_fill(&t_id, acc, peer, id_name);
  sprintf(path, "/id/%s", shkey_hex(&t_id.id_key));
  file = shfs_file_find(fs_tree, path);
  err = shfs_read(file, buff);
  if (err || shbuf_size(buff) < sizeof(tx_id_t)) {
    shbuf_free(&buff);
    return (NULL);
  }

  return ((tx_id_t *)shbuf_unmap(buff));
#endif
  return (NULL);
}

void shacc_identity_print(tx_id_t *id)
{
  char hash_app[256];
  char hash_name[256];

  strcpy(hash_app, shkey_print(shpeer_kpub(&id->id_peer)));
  strcpy(hash_name, shkey_print(&id->id_key));
  printf("[IDENT] '%s' (app '%s')\n", hash_name, hash_app);

}

/** generate session key */
void fill_session(tx_account_t *acc, tx_session_t *sess, tx_id_t *id, double secs)
{
  shkey_t *id_key = &id->id_key;
  shkey_t *key;

  memset(sess, 0, sizeof(tx_session_t));

  sess->sess_stamp = shtime_adj(shtime(), secs); 
  memcpy(&sess->sess_id, id_key, sizeof(shkey_t));

  key = shpam_sess_gen(&acc->pam_seed.seed_key, sess->sess_stamp, &id->id_key);
  memcpy(&sess->sess_key, key, sizeof(shkey_t));
  shkey_free(&key);

}

/** Parse key returned from server TX_SESSION operation. */
void parse_session(tx_session_t *sess, shkey_t *sess_key)
{
  memcpy(&sess->sess_key, sess_key, sizeof(shkey_t)); 
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

#if 0
  if (!shkey_cmp(id_key, ashkey_blank())) {
    buff = shbuf_init();
    sprintf(path, "/session/%s", shkey_hex(id_key));
    file = shfs_file_find(fs_tree, path);
    err = shfs_read(file, buff);
    if (!err && shbuf_size(buff) == sizeof(tx_session_t))
      return ((tx_session_t *)shbuf_unmap(buff));
    shbuf_free(&buff);
  }
#endif

  return (NULL);
}

int shacc_session_save(tx_session_t *sess)
{
  SHFL *file;
  shbuf_t *buff;
  char path[PATH_MAX+1];
  int err;
  
#if 0
  sprintf(path, "/session/%s", shkey_hex(&sess->sess_id));
  file = shfs_file_find(fs_tree, path);
  buff = shbuf_init();
  shbuf_cat(buff, sess, sizeof(tx_session_t));
  err = shfs_write(file, buff);
  shbuf_free(&buff);
  return (err);
#endif

  return (0);
}

tx_session_t *shacc_session(tx_account_t *acc, tx_id_t *id, double secs)
{
  tx_session_t *sess;

  sess = shacc_session_load(&id->id_key);
  if (!sess || sess->sess_stamp <= shtime()) {
    if (sess)
      free(sess);

    sess = (tx_session_t *)calloc(1, sizeof(tx_session_t));
    if (!sess)
      return (NULL);

    fill_session(acc, sess, id, secs);
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

#if 0
        memcpy(&id->id_sig, id_sig, sizeof(shsig_t));
#endif
        shacc_identity_save(id);

        printf ("Info: Server confirmed identity '%s' (%s)\n", str, shkey_print(&id->id_key));
        break;

      case TX_SESSION:
        if (shbuf_size(buff) < sizeof(uint32_t) + sizeof(shkey_t))
          break;

        id_key = (shkey_t *)(data + sizeof(uint32_t));
#if 0
        tok_key = (shkey_t *)(data + sizeof(uint32_t) + sizeof(shkey_t));
        sess = shacc_session_load(id_key);
        if (sess) {
          memcpy(&sess->sess_key, tok_key, sizeof(shkey_t));
          shacc_session_save(sess);
        }
#endif

        printf ("Info: Server confirmed session for identity '%s'.\n", shkey_print(id_key));
        break;
    }
  }

  shbuf_free(&buff);

}

void shacc_session_print(tx_session_t *sess)
{
  char sess_stamp[256]; 

  if (!sess)
    return;

  strcpy(sess_stamp, shctime(sess->sess_stamp)+4);
  printf("[SESSION %s] expire(%20.20s)\n",
    shkey_print(&sess->sess_key), sess_stamp);

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
  
#if 0
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
#endif

#if 0
    if (0 == strcmp(argv[i], "-i")) {
      if ((i+1) < argc && argv[i+1][0] != '-') {
        strncpy(id_name, argv[i+1], sizeof(id_name) - 1);
        i++;
      }
      continue;
    }
#endif

  }

  /* only operation - generate identity */
  run_mode = RUN_GENERATE;

  proc_peer = shapp_init(prog_name, NULL, 0);
  fs_tree = shfs_init(proc_peer);
  shpeer_free(&proc_peer);

  if (run_mode != RUN_GENERATE) {
    /* read pending message queue */
    shacc_msg_read();
  }

  if (!*acc_user) {
    const char *def_user = get_libshare_account_name();
    printf("info: Using default account username '%s'.\n", def_user);
    strcpy(acc_user, def_user);
  }

#if 0
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
#endif

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

  if (run_mode == RUN_LIST) { //_IDENT) {
    tx_id_t **id_list = shacc_identity_load_all(acc);

    /* list all identity for account */
    printf ("Identities:\n");
    if (id_list) {
      for (i = 0; id_list[i]; i++) {
        shacc_identity_print(id);

        tok = shacc_session(acc, id, 1440); /* 24 mins */
        shacc_session_print(tok);

        printf("\n");
      }
    }

    exit (0);
  }

  /* obtain identity */
  peer = shpeer();
  if (run_mode == RUN_GENERATE) {
    /* generate new identity */
    id = shacc_identity_gen(acc, peer, id_name);
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

  tok = shacc_session(acc, id, 1440); /* 24 mins */
  shacc_session_print(tok);

  shfs_free(&fs_tree);

  return (0);
}

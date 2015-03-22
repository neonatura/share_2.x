
/*
 *  Copyright 2015 Neo Natura
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

#include <stdio.h>
#include "share.h"
#include "sharetool.h"

static int _info_msgqid;
static shbuf_t *_info_msgbuff;
static shfs_t *_share_info_fs;

typedef struct info_t
{
  int mode;
  shtime_t stamp;
  shkey_t name;
  union {
    shsig_t sig; /* TX_SIGNATURE */
    tx_id_msg_t id; /* TX_IDENT */
    tx_app_msg_t app; /* TX_APP */
    tx_account_msg_t acc; /* TX_ACCOUNT */
    tx_session_msg_t sess;
    char raw[0];
  } data;
} info_t;
static struct info_t *info_list;
static int info_list_max;


int share_info_peer_store(shkey_t *peer_key, info_t *info)
{
  shfs_ino_t *file;
  shbuf_t *buff;
  shkey_t *key;
  info_t *t_info;
  info_t *t_list;
  size_t max;
  char path[SHFS_PATH_MAX];
  int err;
  int i;

  /* base reference soley on input */
  info->stamp = 0;
  memset(&info->name, 0, sizeof(shkey_t));

  if (info->mode == TX_APP) {
    memcpy(&info->name, shpeer_kpriv(&info->data.app.app_peer), sizeof(shkey_t));
  } else if (info->mode == TX_SESSION) {
    memcpy(&info->name, &info->data.sess.sess_id, sizeof(shkey_t));
  } else {
    /* create key reference based on content. */
    key = shkey_bin((char *)info, sizeof(info_t));
    memcpy(&info->name, key, sizeof(info->name));
    shkey_free(&key);
  }

  /* read in public info */
  sprintf(path, "/peer/%s", shkey_hex(peer_key));
  file = shfs_file_find(_share_info_fs, path);
  if (!file) {
    return (SHERR_IO);
  }

  buff = shbuf_init();
  shfs_read(file, buff);

  max = shbuf_size(buff) / sizeof(info_t);
  t_list = (info_t *)shbuf_data(buff);
  for (i = 0; i < max; i++) {
    t_info = (info_t *)(t_list + i);
    if (shkey_cmp(&t_info->name, &info->name)) {
      /* found identical content. */
      break;
    }
  }

  if (i == max) { /* fresh */
    info->stamp = shtime64();
    shbuf_cat(buff, info, sizeof(info_t));
  } else { /* retained */ 
    if (0 == memcmp(t_info, info, sizeof(info_t))) {
      /* redundant */
      return (0);
    } 

    memcpy(t_info, info, sizeof(info_t));
  }

  err = shfs_write(file, buff);
  shbuf_free(&buff);
  if (err)
    return (err);

  return (0);
}

static char *_share_info_tx_label(int tx_op)
{
  static char label[256];

  memset(label, 0, sizeof(label));
  switch (tx_op) {
    case TX_APP:
      strcpy(label, "app");
      break;
    case TX_ACCOUNT:
      strcpy(label, "account");
      break;
    case TX_IDENT:
      strcpy(label, "ident");
      break;
    case TX_SESSION:
      strcpy(label, "session");
      break;
    case TX_BOND:
      strcpy(label, "bond");
      break;
    case TX_EVENT:
      strcpy(label, "event");
      break;
    default:
      sprintf(label, "type %d", tx_op); 
      break;
  }

  return (label);
}
static void _share_info_msg_parse(shkey_t *peer_key)
{
  shpeer_t *peer;
  uint32_t mode;  
  unsigned char *data;
  size_t data_len;
  info_t info;

fprintf(stderr, "DEBUG: _share_info_msg_parse()\n");

  if (shbuf_size(_info_msgbuff) > sizeof(uint32_t)) {
    mode = *((uint32_t *)shbuf_data(_info_msgbuff));
    data = shbuf_data(_info_msgbuff) + sizeof(uint32_t);
    data_len = shbuf_size(_info_msgbuff) - sizeof(uint32_t);
    if (data_len > 0) {
      memset(&info, 0, sizeof(info));
      info.mode = mode;
      memcpy((char *)info.data.raw, data, data_len);
      share_info_peer_store(peer_key, &info);
      if (run_flags & PFLAG_VERBOSE) {
        fprintf(sharetool_fout, "[shared] pulled %s '%s' (%d bytes).\n",
            _share_info_tx_label(info.mode), shkey_print(peer_key),
            shbuf_size(_info_msgbuff));
      } 
    }
  }

  shbuf_clear(_info_msgbuff);
}

static void _share_info_msg_read(void)
{
  shkey_t src_key;

  while (0 == shmsg_read(_info_msgqid, &src_key, _info_msgbuff)) {
    /* parse message from server. */
    _share_info_msg_parse(&src_key);
  }

}

#define PMODE_NONE 0
#define PMODE_PREFIX 1
#define PMODE_GROUP 2
#define PMODE_PASS 3
#define PMODE_HOST 4
#define PMODE_PORT 5
#define PMODE_PATH 6
shpeer_t *share_info_peer(char *path)
{
  shfs_t *fs;
  shfs_ino_t *dir;
  shfs_ino_t *file;
  shpeer_t *peer;
  char p_prefix[PATH_MAX+1];
  char p_group[PATH_MAX+1];
  char p_pass[PATH_MAX+1];
  char p_host[PATH_MAX+1];
  char p_dir[PATH_MAX+1];
  char p_path[PATH_MAX+1];
  char *peer_name;
  char *peer_host;
  char *cptr;
  char *ptr;
  int p_port;
  int pmode;
  int idx;
  int err;

  memset(p_prefix, 0, sizeof(p_prefix));
  memset(p_group, 0, sizeof(p_group));
  memset(p_pass, 0, sizeof(p_pass));
  memset(p_host, 0, sizeof(p_host));
  memset(p_dir, 0, sizeof(p_dir));
  memset(p_path, 0, sizeof(p_path));
  p_port = 0;

  /* default to server */
  strncpy(p_prefix, "shared", sizeof(p_prefix) - 1);

  if (0 == strncmp(path, "~", 1)) {
    shkey_t *id_key;
    shpeer_t *peer;

    id_key = shpam_ident_gen(shpam_uid(get_libshare_account_name()), ashpeer());
    peer = shfs_home_peer(id_key);
    shkey_free(&id_key);

    return (peer);
  }
  
  pmode = PMODE_NONE;
  ptr = path;
  while (*ptr) {
    idx = strcspn(ptr, ":@");
    cptr = ptr;
    ptr += idx;

    if (pmode == PMODE_NONE) {
      if (0 == strncmp(ptr, ":", 1)) {
        pmode = PMODE_GROUP;
        memset(p_prefix, 0, sizeof(p_prefix));
        strncpy(p_prefix, cptr, idx);
        ptr += 1;
      } else if (0 == strncmp(ptr, "@", 1)) {
        pmode = PMODE_HOST;
        memset(p_prefix, 0, sizeof(p_prefix));
        strncpy(p_prefix, cptr, idx);
        ptr += 1;
      } else {
        pmode = PMODE_PATH;
        memset(p_prefix, 0, sizeof(p_prefix));
        strncpy(p_prefix, cptr, idx);
      }
    } else if (pmode == PMODE_GROUP) {
      if (*ptr == ':') {
        pmode = PMODE_PASS;
        ptr++;
      } else if (*ptr == '@') {
        pmode = PMODE_HOST;
        ptr++;
      } else {
        pmode = PMODE_PATH;
      }
      strncpy(p_group, cptr, idx);
    } else if (pmode == PMODE_PASS) {
      if (*ptr == '@') {
        pmode = PMODE_HOST;
        ptr++;
      } else {
        pmode = PMODE_PATH;
      }
      strncpy(p_pass, cptr, idx);
    } else if (pmode == PMODE_HOST) {
      if (*ptr == ':') {
        pmode = PMODE_PORT;
        ptr++;
      } else {
        pmode = PMODE_PATH;
      }
      strncpy(p_host, cptr, idx);
    } else if (pmode == PMODE_PORT) {
      pmode = PMODE_PATH;
      p_port = atoi(cptr);
    } else if (pmode == PMODE_PATH) {
      break;
    }

  }

  peer_name = NULL;
  if (*p_prefix) {
    peer_name = p_prefix;
    if (*p_pass) {
      strcat(peer_name, ":");
      strcat(peer_name, p_pass);
    }
  }
  peer_host = NULL;
  if (*p_host) {
    peer_host = p_host; 
    if (p_port)
      sprintf(peer_host+strlen(peer_host), ":%d", p_port);
  }

  peer = shpeer_init(peer_name, peer_host);
  return (peer);
}

void share_info_peer_scan(shpeer_t *peer, int pflags)
{
  shfs_ino_t *file;
  shbuf_t *buff;
  char path[SHFS_PATH_MAX];
  int err;

  buff = shbuf_init();

  /* read in public info */
  sprintf(path, "/peer/%s", shkey_hex(shpeer_kpub(peer)));
  file = shfs_file_find(_share_info_fs, path);
  err = shfs_read(file, buff);

  /* read in private info */
  sprintf(path, "/peer/%s", shkey_hex(shpeer_kpriv(peer)));
  file = shfs_file_find(_share_info_fs, path);
  err = shfs_read(file, buff);

  if (shbuf_size(buff) == 0) {
    info_list_max = 0;
    shbuf_free(&buff);
    return;
  }

  info_list_max = shbuf_size(buff) / sizeof(info_t);
  info_list = (info_t *)shbuf_unmap(buff);
}

void share_info_app_print(info_table_t *table, info_t *info)
{
  char buf[256];

  info_table_add_row(table, "APP", info->data.app.app_stamp);
  info_table_add_peer(table, "app", &info->data.app.app_peer);
  info_table_add_key(table, "context", info->data.app.app_context);
  info_table_add_int(table, "trust", info->data.app.app_trust);
  info_table_add_int(table, "hop", info->data.app.app_hop);

}

void share_info_account_print(info_table_t *table, info_t *info)
{

  info_table_add_row(table, "ACCOUNT", 0);
  info_table_add_key(table, "priv", info->data.acc.pam_seed.seed_key);
  if (info->stamp)
    info_table_add_str(table, "time", shstrtime64(info->stamp, NULL));
}

void share_info_id_print(info_table_t *table, info_t *info)
{
  char uid_str[256];

  sprintf(uid_str, "%llu", info->data.id.id_uid);
  info_table_add_row(table, "IDENT", 0);
  info_table_add_str(table, "UID", uid_str);
  if (info->stamp)
    info_table_add_str(table, "time", shstrtime64(info->stamp, NULL));
}

void share_info_session_print(info_table_t *table, info_t *info)
{

  info_table_add_row(table, "SESSION", 0);
  info_table_add_key(table, "token", &info->data.sess.sess_id);
  if (info->stamp)
    info_table_add_str(table, "time", shstrtime64(info->stamp, NULL));
}

void share_info_list_print(shpeer_t *peer, int pflags)
{
  info_table_t *table;
  char header[256];
  char *arch_str;
  int pid;
  int i;

  pid = share_info_pid(peer->label); 
  sprintf(header, "[%s", shpeer_print(peer));
  if (pid)
    sprintf(header+strlen(header), " pid(%d)", pid);
  arch_str = share_info_arch(peer->arch);
  if (*arch_str)
    sprintf(header+strlen(header), " arch(%s)", arch_str);
  strcat(header, "]");
  fprintf(sharetool_fout, "%s\n", header);

  table = info_table_init();
  for (i = 0;i < info_list_max; i++) {
    switch (info_list[i].mode) {
      case TX_APP:
        share_info_app_print(table, &info_list[i]);
        break;
      case TX_ACCOUNT:
        share_info_account_print(table, &info_list[i]);
        break;
      case TX_IDENT:
        share_info_id_print(table, &info_list[i]);
        break;
      case TX_SESSION:
        share_info_session_print(table, &info_list[i]);
        break;
      default:
        fprintf(stderr, "DEBUG: share_info_list_print[%d]: mode %d\n", i, info_list[i].mode);
        break;
    }
  }
  info_table_print(table, sharetool_fout);
}

void share_info_print(char *peer_str, int pflags)
{
  shpeer_t *peer;

  peer = NULL;
  if (0 == strcasecmp(peer_str, "self")) {
    peer = shpeer();
  } else if (0 == strcasecmp(peer_str, "sexe")) {
    peer = shpeer_init("sexe", NULL);
  } else {
    peer = share_info_peer(peer_str);
  }
  if (!peer) {
    fprintf(stderr, "%s: error parsing '%s': invalid syntax.\n", process_path, peer_str);
    return;
  }

  info_list_max = 0;
  share_info_peer_scan(peer, pflags);
  share_info_list_print(peer, pflags);

  /* ask for info on app */
  shapp_listen(TX_APP, peer);
  shapp_listen(TX_ACCOUNT, peer);
  shapp_listen(TX_IDENT, peer);
  shapp_listen(TX_SESSION, peer);

  shpeer_free(&peer);
  if (info_list) free(info_list);
  info_list = NULL;

}

void share_info_poll(void)
{
  _share_info_msg_read();
}

int share_info(char **args, int arg_cnt, int pflags)
{
  shpeer_t *peer;
  int err;
  int i;

  /* register application. */ 
  peer = shapp_init(args[0], NULL, 0);
  if (!peer)
    return (SHERR_IO);

  /* open message queue to server */
  _info_msgqid = shmsgget(NULL); 
  _info_msgbuff = shbuf_init();

  /* open file-system for app's partition. */
  _share_info_fs = shfs_init(NULL);

  /* read in pending messages. */
  _share_info_msg_read();

  shpeer_free(&peer);

  if (arg_cnt > 1) {
    for (i = 1; i < arg_cnt; i++) {
      share_info_print(args[i], pflags);
    }
  } else {
    share_info_print("", pflags);
  }

  /* check again for fresh data */
  share_info_poll();

  return (0);
}

int share_info_pid(char *app_name)
{
  int pid = 0;

#ifdef LINUX
  if (0 != strcasecmp(app_name, PACKAGE)) {
    FILE *fl;
    char path[PATH_MAX+1];
    char str[256];

    sprintf(path, "/var/run/%s.pid", app_name);
    fl = fopen(path, "rb");
    memset(str, 0, sizeof(str));
    if (fl) {
      fgets(str, sizeof(str)-1, fl);
      fclose(fl);
    }

    pid = atoi(str);
    if (0 != kill(pid, 0))
      pid = 0;
  }
#endif

  return (pid);
}

char *share_info_arch(int arch)
{
  static char ret_str[256];

  memset(ret_str, 0, sizeof(ret_str));
  if (arch & SHARCH_LINUX && arch & SHARCH_32BIT)
    strcpy(ret_str, "LIN32");
  else if (arch & SHARCH_WIN && arch & SHARCH_32BIT)
    strcpy(ret_str, "WIN32");
  else if (arch & SHARCH_LINUX)
    strcpy(ret_str, "LIN");
  else if (arch & SHARCH_WIN)
    strcpy(ret_str, "WIN");

  return (ret_str);
}


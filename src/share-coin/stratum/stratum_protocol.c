

#define __PROTO__PROTOCOL_C__
#include "shcoind.h"




char *stratum_runtime_session(void)
{
  static char buf[32];

  if (!*buf) {
    sprintf(buf, "%-8.8x", time(NULL));
  }

  return (buf);
}



int stratum_request_id(void)
{
  static int idx;
  return (++idx);
}

int stratum_send_difficulty(user_t *user)
{
  shjson_t *reply;
  shjson_t *data;
  int err;

  reply = shjson_init(NULL);
  shjson_null_add(reply, "id");
  shjson_str_add(reply, "method", "mining.set_difficulty");
  data = shjson_array_add(reply, "params");
  shjson_num_add(data, NULL, user->work_diff);
  err = stratum_send_message(user, reply);
  shjson_free(&reply);

  return (err);
}

int stratum_send_client_ver(user_t *user)
{
  shjson_t *reply;
  shjson_t *data;
  int err;

  user->cli_id = stratum_request_id();

  reply = shjson_init(NULL);
  shjson_num_add(reply, "id", user->cli_id);
  shjson_str_add(reply, "method", "client.get_version");
  err = stratum_send_message(user, reply);
  shjson_free(&reply);

  return (err);
}



int stratum_session_nonce(void)
{
  char buf[256];
  int *val;

  strcpy(buf, "SOCK");
  val = (int *)buf;
  
  return (*val);
}

int stratum_validate_submit(user_t *user, int req_id, shjson_t *json)
{
  shjson_t *block;
  task_t *task;
  shkey_t *key;
shfs_ino_t *file;
  char *worker = shjson_array_astr(json, "params", 0); 
  char *job_id = shjson_array_astr(json, "params", 1); 
  char *extranonce2 = shjson_array_astr(json, "params", 2); 
  char *ntime = shjson_array_astr(json, "params", 3); 
  char *nonce = shjson_array_astr(json, "params", 4); 
  char *dup;
  char buf[64];
    char path[PATH_MAX+1];
  char xn_hex[256];
  char cur_hash[512];
  char cb1[512];
  char share_hash[128];
  double last_diff;
  uint32_t be_ntime;
  uint32_t be_nonce;
  uint32_t *data32;
  uint32_t last_nonce;
  unsigned int task_id;
  int err;
  int i;

  task_id = (unsigned int)strtol(job_id, NULL, 16);
  fprintf(stderr, "DEBUG: stratum_validate_submit: task id %lu\n", task_id);
  task = stratum_task(task_id);
  if (!task) {
    return (stratum_send_error(user, req_id, BLKERR_INVALID_JOB));
  }

  /* generate new cb1 */
  fprintf(stderr, "DEBUG: [SUBMIT] cb1 \"%s\"\n", task->cb1);
  memset(buf, 0, sizeof(buf));
  be_ntime = strtol(ntime, NULL, 16);
  fprintf(stderr, "%u = strtol(ntime)\n", be_ntime);
  // be_ntime = htobe32(be_ntime);
  //fprintf(stderr, "%u = htobe3(ntime)\n", be_ntime);
  bin2hex(buf, &be_ntime, 4);
  fprintf(stderr, "bin2hex(be_ntime) \"%s\"\n", buf);
  memset(cb1, 0, sizeof(cb1) - 1);
  strncpy(cb1, task->cb1, sizeof(cb1) - 1);
  strncpy(cb1 + strlen(cb1) - 10, buf, 8);
  fprintf(stderr, "DEBUG: [SUBMIT] cb1 \"%s\"\n", cb1);


  fprintf(stderr, "DEBUG: [SUBMIT] cb2 \"%s\"\n", task->cb2);
  fprintf(stderr, "DEBUG: [SUBMIT] prev_hash \"%s\"\n", task->prev_hash);
  fprintf(stderr, "DEBUG: [SUBMIT] nbits \"%s\"\n", task->nbits);
  fprintf(stderr, "DEBUG: [SUBMIT] version \"%d\"\n", task->version);

  /* set worker name */
  stratum_user(user, worker);

  strncpy(task->work.xnonce2, extranonce2, sizeof(task->work.xnonce2) - 1);
  task->work.nonce = strtol(nonce, NULL, 16); 

  fprintf(stderr, "DEBUG: [SUBMIT] xnonce1 \"%s\" (n1 len: %d)\n", user->peer.nonce1, user->peer.n1_len);
  fprintf(stderr, "DEBUG: [SUBMIT] xnonce2 \"%s\" (n2 len: %d)\n", task->work.xnonce2, user->peer.n2_len);
  fprintf(stderr, "DEBUG: [SUBMIT] ntime %lu [%s]\n", strtol(ntime, NULL, 16), ntime); 
  fprintf(stderr, "DEBUG: [SUBMIT] nonce %lu [%s]\n", task->work.nonce, nonce); 


  /* generate block hash */
  shscrypt_work(&user->peer, &task->work, task->merkle, task->prev_hash, cb1, task->cb2, task->nbits, ntime);

  hex2bin(&task->work.data[76], nonce, 4);

  task->work.nonce = strtol(nonce, NULL, 16); 
  memset(task->work.hash, 0, sizeof(task->work.hash));

#if 0
  err = !scanhash_scrypt(task->work.midstate, task->work.data, task->work.hash, task->work.target, task->work.nonce+1, &last_nonce, task->work.nonce-2, &last_diff);
  fprintf(stderr, "DEBUG: err %d = scanhash_scrypt(%d)\n", err, task->work.nonce);
#endif

  be_nonce =  htobe32(strtol(nonce, NULL, 16));
  err = !scanhash_scrypt(task->work.midstate, task->work.data, task->work.hash, task->work.target, be_nonce+1, &last_nonce, be_nonce-2, &last_diff);
  fprintf(stderr, "DEBUG: err %d = scanhash_scrypt(%d)\n", err, be_nonce);
  if (err)
    return (BLKERR_LOW_DIFFICULTY);

  key = shkey_bin(task->work.data, 80);
  dup = shmeta_get_str(task->share_list, key);
  memset(share_hash, 0, sizeof(share_hash));
  bin2hex(share_hash, task->work.hash, 32);
  if (dup) {
fprintf(stderr, "DEBUG: duplicate share hash '%s' submitted [key %s].\n", share_hash, shkey_print(key));
    if (0 == strcmp(dup, share_hash))
      return (BLKERR_DUPLICATE_BLOCK);
  }
fprintf(stderr, "DEBUG: unique share hash '%s' submitted [key %s].\n", share_hash, shkey_print(key));
  shmeta_set_str(task->share_list, key, share_hash);
  sprintf(path, "/share/%s", share_hash);
  file = shfs_file_find(block_fs, path); /* file */
  if (file) {
char *tdata;
size_t tdata_len;
    err = shfs_file_write(file, user->worker, strlen(user->worker));
fprintf(stderr, "DEBUG: %d = shfs_file_write('%s', '%s')\n", err, path, user->worker);

/*test */
tdata = NULL;
err = shfs_file_read(file, &tdata, &tdata_len);
fprintf(stderr, "DEBUG: %d = shfs_file_read('%s') = '%s'\n", err, path, tdata); 
if (!err)
  free(tdata);

  }
  shkey_free(&key);

  task->work.pool_diff = last_diff;
#if 0
  err = shscrypt_verify(&task->work);
  fprintf(stderr, "DEBUG: %d = shscrypt_verify() [sdiff %f, diff %f]\n", err, task->work.sdiff, shscrypt_hash_diff(&task->work));
#endif


  stratum_user_block(user, task);

  /* if (user->peer.diff > task->target) */

  sprintf(xn_hex, "%s%s", user->peer.nonce1, task->work.xnonce2); 
  err = submitblock(task->task_id, strtol(ntime, NULL, 16), task->work.nonce, xn_hex);
  fprintf(stderr, "DEBUG: %d = stratum_validate_submit: submitblock(task %u, ntime %s, nonce %u, xn %s)\n", err, task->task_id, ntime, task->work.nonce, xn_hex);
#if 0
  err = submitblock(task->task_id, strtol(ntime, NULL, 16), task->work.hash_nonce, xn_hex);
  fprintf(stderr, "DEBUG: %d = stratum_validate_submit: submitblock(task %u, ntime %s, nonce %u, xn %s)\n", err, task->task_id, ntime, be_nonce, xn_hex);
#endif

  return (0);
}

int stratum_subscribe(user_t *user, int req_id)
{
  int err;

  err = stratum_send_subscribe(user, req_id);
  if (!err) 
    user->flags |= USER_SUBSCRIBE;

  return (err);
}

int stratum_set_difficulty(user_t *user, int diff)
{
  int err;

  user->work_diff = diff;
  err = stratum_send_difficulty(user);
fprintf(stderr, "DEBUG: %d = stratum_send_difficulty(diff %d)", user->work_diff);
  return (err);
}

int stratum_request_message(user_t *user, shjson_t *json)
{
  shjson_t *reply;
  user_t *t_user;
  char uname[256];
  char *method;
  long idx;
  int err;


  idx = (int)shjson_num(json, "id", -1);
fprintf(stderr, "DEBUG: stratum_request_message(%d, %x): req_id %ld\n", user->fd, json, idx);
  if (idx != -1 && idx == user->cli_id && shjson_strlen(json, "result")) {
fprintf(stderr, "DEBUG: response from client.get_version\n");
    /* response from 'client.get_version' method. */ 
    strncpy(user->cli_ver, shjson_astr(json, "result", ""), sizeof(user->cli_ver));
    return (0);
  }

  method = shjson_astr(json, "method", NULL);
  if (!method) {
    /* no operation method specified. */
fprintf(stderr, "DEBUG: no 'method' specified.\n");
    return (SHERR_INVAL);
  }

fprintf(stderr, "DEBUG: REQUEST '%s' [idx %d].\n", method, idx);
  if (0 == strcmp(method, "mining.ping")) {
    reply = shjson_init(NULL);
    shjson_num_add(reply, "id", idx);
    shjson_null_add(reply, "error");
    shjson_null_add(reply, "result");
    err = stratum_send_message(user, reply);
    shjson_free(&reply);
    return (err);
  } 

  if (0 == strcmp(method, "mining.subscribe")) {
    err = stratum_subscribe(user, idx);
#if 0
    shjson_t *data;
    shjson_t *data2;
    char nonce_str[64];

    sprintf(nonce_str, "%x", stratum_session_nonce());

    reply = shjson_init(NULL);
    shjson_num_add(reply, "id", idx);
    shjson_null_add(reply, "error");
    data = shjson_array_add(reply, "result");
    data2 = shjson_array_add(data, NULL);
    shjson_str_add(data2, NULL, "mining.notify");
    shjson_str_add(data2, NULL, "00000000");
    shjson_str_add(data, NULL, nonce_str);
    shjson_num_add(data, NULL, 4);
    shjson_str_add(data, NULL, stratum_runtime_session());
    err = stratum_send_message(user, reply);
    shjson_free(&reply);

    user->active = TRUE;
    return (err);
#endif
    if (!err)
      stratum_set_difficulty(user, 32);
    return (err);
  } 

  if (0 == strcmp(method, "mining.authorize")) {
    shjson_t *param;
    char *username;
    char *password;

    username = shjson_array_astr(json, "params", 0);
    password = shjson_array_astr(json, "params", 1);
    user = stratum_user(user, username);
    if (!user) {
      reply = shjson_init(NULL);
      shjson_str_add(reply, "error", "unknown user"); 
      shjson_bool_add(reply, "result", FALSE);
      err = stratum_send_message(user, reply);
      shjson_free(&reply);
      return (err);
    }

    reply = shjson_init(NULL);
    shjson_bool_add(reply, "result", TRUE);
    shjson_null_add(reply, "error"); 
    err = stratum_send_message(user, reply);
    shjson_free(&reply);

    stratum_set_difficulty(user, 32);
    //stratum_set_difficulty(user, MAX(32, atoi(password)));
    stratum_send_client_ver(user);
    return (err);
  }

  if (0 == strcmp(method, "mining.resume")) {
    char *sess_id;

    sess_id = shjson_array_astr(json, "params", 0);

    reply = shjson_init(NULL);
    shjson_num_add(reply, "id", idx);

    /* compare previous session hash */
    if (0 != strcmp(sess_id, stratum_runtime_session()))
      return (stratum_send_error(user, idx, BLKERR_BAD_SESSION));

    shjson_bool_add(reply, "result", TRUE);
    shjson_null_add(reply, "error"); 
    err = stratum_send_message(user, reply);
    shjson_free(&reply);
    return (err);
  }

  if (0 == strcmp(method, "mining.submit")) {
    err = stratum_validate_submit(user, idx, json);
    if (err = SHERR_INVAL) {
/* error already sent */
#if 0
      reply = shjson_init(NULL);
      shjson_num_add(reply, "id", idx);
      shjson_str_add(reply, "error", "unknown-work");
      shjson_bool_add(reply, "result", FALSE);
      err = stratum_send_message(user, reply);
      shjson_free(&reply);
#endif
      return (err);
    }

    reply = shjson_init(NULL);
    shjson_num_add(reply, "id", idx);
    if (!err) {
      shjson_array_add(reply, "result");
      //shjson_bool_add(reply, "result", TRUE);
      shjson_null_add(reply, "error");
    } else {
      shjson_bool_add(reply, "result", FALSE);
      if (err == SHERR_ALREADY) {
        shjson_str_add(reply, "error", "duplicate");
      } else if (err == SHERR_TIME) {
        shjson_str_add(reply, "error", "slightly stale");
      } else if (err == SHERR_PROTO) {
        shjson_str_add(reply, "error", "H-not-zero");
      } else {
        shjson_str_add(reply, "error", "unknown");
      }
    }
    err = stratum_send_message(user, reply);
    shjson_free(&reply);

    if (err == SHERR_PROTO) {
      stratum_send_difficulty(user);
    }

    return (err);
  }

  if (0 == strcmp(method, "mining.shares")) {
    shjson_t *data;
    shjson_t *udata;

    reply = shjson_init(NULL);
    data = shjson_array_add(reply, "result");
    for (t_user = client_list; t_user; t_user = t_user->next) {
      if (t_user->block_tot == 0 && t_user->block_avg <= 0.00000)
        continue;

      memset(uname, 0, sizeof(uname));
      strncpy(uname, t_user->worker, sizeof(uname) - 1);
      strtok(uname, ".");

      udata = shjson_array_add(data, NULL);
      shjson_str_add(udata, NULL, t_user->worker);
      shjson_num_add(udata, NULL, t_user->round_stamp);
      shjson_num_add(udata, NULL, t_user->block_cnt);
      shjson_num_add(udata, NULL, t_user->block_tot);
      shjson_num_add(udata, NULL, t_user->block_avg);
      shjson_num_add(udata, NULL, stratum_user_speed(t_user)); /* khs */
      shjson_str_add(udata, NULL, getaddressbyaccount(uname));
      shjson_num_add(udata, NULL, getaccountbalance(uname));
    }
    shjson_null_add(reply, "error");
    shjson_num_add(reply, "id", idx);
    err = stratum_send_message(user, reply);
    shjson_free(&reply);
    return (err);
  }

  return (0);
}






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
  shjson_num_add(data, NULL, user->peer.diff);
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
  char *worker = shjson_array_astr(json, "params", 0); 
  char *job_id = shjson_array_astr(json, "params", 1); 
  char *extranonce2 = shjson_array_astr(json, "params", 2); 
  char *ntime = shjson_array_astr(json, "params", 3); 
  char *nonce = shjson_array_astr(json, "params", 4); 
  int err;

  task = stratum_task(strtol(job_id, NULL, 16));
  if (!task) {
    return (stratum_send_error(user, req_id, ERR_INVALID_JOB));
  }

  /* set worker name */
  stratum_user(user, worker);

  strncpy(task->work.xnonce2, extranonce2, sizeof(task->work.xnonce2) - 1);
  task->work.hash_nonce = strtol(nonce, NULL, 16); 

  /* generate block hash */
  shscrypt_work(&user->peer, &task->work, task->merkle, task->prev_hash, task->cb1, task->cb2, task->nbits);
  err = shscrypt_verify(&task->work);
  if (err)
    return (err);

  err = submitblock(task->prev_hash, task->work.merkle_root, strtol(ntime, NULL, 16), strtol(task->nbits, NULL, 16), task->work.hash_nonce);
  if (err)
    return (SHERR_INVAL);

  stratum_user_block(user, task);
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

int stratum_set_difficulty(user_t *user, double diff)
{
  int err;

  user->peer.diff = diff;
  err = stratum_send_difficulty(user);
  return (err);
}
int stratum_request_message(user_t *user, shjson_t *json)
{
  shjson_t *reply;
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

    stratum_set_difficulty(user, MAX(32, atoi(password)));
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
      return (stratum_send_error(user, idx, ERR_BAD_SESSION));

    shjson_bool_add(reply, "result", TRUE);
    shjson_null_add(reply, "error"); 
    err = stratum_send_message(user, reply);
    shjson_free(&reply);
    return (err);
  }

  if (0 == strcmp(method, "mining.submit")) {
    err = stratum_validate_submit(user, idx, json);
    if (err = SHERR_INVAL) {
      reply = shjson_init(NULL);
      shjson_num_add(reply, "id", idx);
      shjson_str_add(reply, "error", "unknown-work");
      shjson_bool_add(reply, "result", FALSE);
      err = stratum_send_message(user, reply);
      shjson_free(&reply);
      return (err);
    }

    reply = shjson_init(NULL);
    shjson_num_add(reply, "id", idx);
    if (!err) {
      shjson_bool_add(reply, "result", TRUE);
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

  return (0);
}




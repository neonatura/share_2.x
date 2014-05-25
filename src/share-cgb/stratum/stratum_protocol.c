

#define __PROTO__PROTOCOL_C__
#include "shcgbd.h"


extern int c_submitblock(char *hashPrevBlock, char *hashMerkleRoot, unsigned int nTime, unsigned int nBits, unsigned int nNonce);


char *stratum_runtime_session(void)
{
  static char buf[32];

  if (!*buf) {
    sprintf(buf, "%x", time(NULL));
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
  shjson_num_add(data, NULL, user->difficulty);
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


int stratum_send_template(user_t *user, int clean)
{
  shjson_t *reply;
  shjson_t *param;
  shjson_t *block;
  char proto_str[64];
  char job_id[64];
  char time_str[64];
  char *coinbase1;
  char *coinbase2;
  int err;

  if (!user->active)
    return;

  block = shjson_init(c_getblocktemplate());
  if (!block)
    return (SHERR_INVAL);

  coinbase1 = shjson_astr(block, "coinbaseaux", "0000000000000000000000000000000000000000000000000000000000000000");
  coinbase2 = shjson_astr(block, "coinbasevalue", "0000000000000000000000000000000000000000000000000000000000000000");

  user->job_id++;

  sprintf(proto_str, "%u", PROTOCOL_VERSION);
  sprintf(job_id, "%u", user->job_id);
  sprintf(time_str, "%-8.8x", shjson_num(block, "curtime", 0));

  reply = shjson_init(NULL);
  shjson_null_add(reply, "id");
  shjson_str_add(reply, "method", "mining.notify");
  param = shjson_array_add(reply, "params");
  shjson_str_add(param, NULL, job_id);
  shjson_str_add(param, NULL, shjson_str(block, "previousblockhash", "0000000000000000000000000000000000000000000000000000000000000000"));
  shjson_str_add(param, NULL, coinbase1); /* hex */
  shjson_str_add(param, NULL, coinbase2); /* hex */
  shjson_str_add(param, NULL, shjson_str(block, "merkleroot", "0000000000000000000000000000000000000000000000000000000000000000"));
  shjson_str_add(param, NULL, proto_str);
  shjson_str_add(param, NULL, shjson_str(block, "bits", "00000000"));
  shjson_str_add(param, NULL, time_str);
  shjson_bool_add(param, NULL, clean);
  
  err = stratum_send_message(user, reply);
  shjson_free(&reply);

  shjson_free(&block);

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

int stratum_validate_submit(user_t *user, shjson_t *json)
{
  shjson_t *block;
  char *job_id = shjson_array_astr(json, "params", 1); 
  char *extranonce2 = shjson_array_astr(json, "params", 2); 
  char *ntime = shjson_array_astr(json, "params", 3); 
  char *nonce = shjson_array_astr(json, "params", 4); 
  char *merkleroot;
  char *prevhash;
  char *bits;
  int err;

  block = shjson_init(c_getblocktemplate());
  if (!block)
    return (SHERR_INVAL);
  
  merkleroot = shjson_str(block, "merkleroot", "0000000000000000000000000000000000000000000000000000000000000000");
  prevhash = shjson_str(block, "previousblockhash", "0000000000000000000000000000000000000000000000000000000000000000");
  bits = shjson_str(block, "bits", "00000000");

  err = c_submitblock(prevhash, merkleroot, strtol(ntime, NULL, 16), strtol(bits, NULL, 16), strtol(nonce, NULL, 16)); 
atoi(ntime),  

  shjson_free(&block);

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
    shjson_str_add(data2, NULL, "hhtt");
    shjson_str_add(data, NULL, nonce_str);
    shjson_num_add(data, NULL, 4);
    shjson_str_add(data, NULL, stratum_runtime_session());
    err = stratum_send_message(user, reply);
    shjson_free(&reply);

    user->active = TRUE;
    return (err);
  } 

  if (0 == strcmp(method, "mining.authorize")) {
    shjson_t *param;
    char *username;
    char *password;

    username = shjson_array_str(json, "params", 0);
    password = shjson_array_str(json, "params", 1);
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

    stratum_send_difficulty(user);
    stratum_send_client_ver(user);
    stratum_send_template(user, FALSE);
    return (err);
  }

  if (0 == strcmp(method, "mining.resume")) {
    char *sess_id;

    sess_id = shjson_array_str(json, "params", 0);

    reply = shjson_init(NULL);
    shjson_num_add(reply, "id", idx);
    if (0 != strcmp(sess_id, stratum_runtime_session())) {
      shjson_str_add(reply, "error", "bad session id"); 
      shjson_bool_add(reply, "result", FALSE);
      err = stratum_send_message(user, reply);
      shjson_free(&reply);
      return (err);
    }

    shjson_bool_add(reply, "result", TRUE);
    shjson_null_add(reply, "error"); 
    err = stratum_send_message(user, reply);
    shjson_free(&reply);
    return (err);
  }

  if (0 == strcmp(method, "mining.submit")) {
    err = stratum_validate_submit(user, json);
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




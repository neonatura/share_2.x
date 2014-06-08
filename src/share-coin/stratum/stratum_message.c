

#include "shcoind.h"

/**
 * Sends textual JSON reply message to a stratum client.
 */
int stratum_send_message(user_t *user, shjson_t *msg)
{
  char *text;
  int err;

  text = shjson_print(msg);
  err = shnet_write(user->fd, text, strlen(text));
  if (err == -1)
    return (-errno);
  err = shnet_write(user->fd, "\n", 1);
  if (err == -1)
    return (-errno);
  free(text);

  return (0);
}

int stratum_send_error(user_t *user, int req_id, int err_code)
{
  shjson_t *reply;
  shjson_t *error;
  char *err_msg;
  int err;

  if (err_code == 21)
    err_msg = "Job not found";
  else if (err_code == 22)
    err_msg = "Duplicate share";
  else if (err_code == 23)
    err_msg = "Low difficulty";
  else if (err_code == 24)
    err_msg = "Unauthorized worker";
  else if (err_code == 25)
    err_msg = "Not subscribed";
  else if (err_code == 61)
    err_msg = "Bad session id";
  else /* if (err_code == 20) */
    err_msg = "Other/Unknown";

  reply = shjson_init(NULL);
  shjson_num_add(reply, "id", req_id); 
  error = shjson_array_add(reply, "error");
  shjson_num_add(error, NULL, err_code);
  shjson_str_add(error, "error", err_msg);
  shjson_null_add(error, NULL);
  shjson_bool_add(reply, "result", FALSE);
  err = stratum_send_message(user, reply);
  shjson_free(&reply);

  return (err);
}

int stratum_send_subscribe(user_t *user, int req_id)
{
  shjson_t *reply;
  shjson_t *data;
  shjson_t *data2;
  char nonce_str[64];
  char key_str[64];
  int err;


  reply = shjson_init(NULL);
  shjson_num_add(reply, "id", req_id);
  shjson_null_add(reply, "error");
  data = shjson_array_add(reply, "result");
  data2 = shjson_array_add(data, NULL);
  shjson_str_add(data2, NULL, "mining.notify");
  shjson_str_add(data2, NULL, shkey_print(ashkey_str(user->peer.nonce1)));
  shjson_str_add(data, NULL, user->peer.nonce1);
  shjson_num_add(data, NULL, 4);
  shjson_str_add(data, NULL, stratum_runtime_session());
  err = stratum_send_message(user, reply);
  shjson_free(&reply);
 
  return (err);
}

int stratum_send_task(user_t *user, task_t *task, int clean)
{
  shjson_t *reply;
  shjson_t *param;
  shjson_t *merk_ar;
  char proto_str[32];
  char time_str[32];
  char task_id[32];
  uint64_t cb1;
  int err;
  int i;

  if (!(user->flags & USER_SUBSCRIBE))
    return;

  if (task->height < user->height)
    return (SHERR_INVAL);

  sprintf(proto_str, "%u", task->version);
  sprintf(time_str, "%-8.8x", task->curtime);
  sprintf(task_id, "%-8.8x", task->task_id);

  reply = shjson_init(NULL);
  shjson_null_add(reply, "id");
  shjson_str_add(reply, "method", "mining.notify");
  param = shjson_array_add(reply, "params");
  shjson_str_add(param, NULL, task_id);
  shjson_str_add(param, NULL, task->prev_hash);
  shjson_str_add(param, NULL, task->cb1);
  shjson_str_add(param, NULL, task->cb2);
  merk_ar = shjson_array_add(param, NULL);
  for (i = 0; i < task->merkle_len; i++) {
    shjson_str_add(merk_ar, NULL, task->merkle[i]);
  }
  shjson_str_add(param, NULL, proto_str);
  shjson_str_add(param, NULL, task->nbits);
  shjson_str_add(param, NULL, time_str); /* ntime */
  shjson_bool_add(param, NULL, (user->height != task->height)); /* clean */

  err = stratum_send_message(user, reply);
  shjson_free(&reply);


  return (err);
}

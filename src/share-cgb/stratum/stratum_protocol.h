







#ifndef __STRATUM__PROTOCOL_H__
#define __STRATUM__PROTOCOL_H__

typedef struct block_t
{
  char cb_1[128];
  char cb_2[128];
  char prev_hash[128];
  char merk_root[128];
  char bits[128];
  char job_id[128];
  time_t curtime;
} block_t;

int stratum_request_message(user_t *user, shjson_t *json);
int stratum_send_template(user_t *user, int clean);
block_t *stratum_block_template(user_t *user, shjson_t *json);

#endif /* __STRATUM__PROTOCOL_H__ */


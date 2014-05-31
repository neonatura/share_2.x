







#ifndef __STRATUM__PROTOCOL_H__
#define __STRATUM__PROTOCOL_H__


int stratum_request_message(user_t *user, shjson_t *json);
int stratum_send_template(user_t *user, int clean);
int stratum_set_difficulty(user_t *user, double diff);
int stratum_validate_submit(user_t *user, int req_id, shjson_t *json);

#endif /* __STRATUM__PROTOCOL_H__ */


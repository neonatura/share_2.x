
#define ERR_BAD_SESSION 61
#define ERR_INVALID_JOB 62

int stratum_send_message(user_t *user, shjson_t *msg);
int stratum_send_difficulty(user_t *user);
int stratum_send_error(user_t *user, int req_id, int err_code);


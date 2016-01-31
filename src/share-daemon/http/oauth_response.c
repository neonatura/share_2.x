
#include "sharedaemon.h"
#include "oauth_favicon.h"

int oauth_response_token(shd_t *cli, shbuf_t *buff, char *client_id, char *redirect_url, char *scope_str)
{
  shmap_t *sess;
  char text[1024];
  char url[1024];
  char key_str[256];
  char *token;
  int i;

  sess = oauth_sess_load(cli, client_id);
  if (!sess)
    return (SHERR_INVAL);

  if (client_id) {
    oauth_sess_redirect_url_set(sess, client_id, redirect_url);
    oauth_sess_scope_set(sess, client_id, scope_str);
  }

  if (oauth_sess_login(sess)) {
    int bits = oauth_scope_bits(scope_str);
    for (i = 0; i < MAX_OAUTH_SCOPE; i++) { 
      if (!(bits & (1 << i)))
        continue;

fprintf(stderr, "DEBUG: oauth_response_token: access %s = %s\n", oauth_scope_label(1 << i), oauth_sess_access(sess, client_id, (1 << i))?"true":"false"); 
      if (!oauth_sess_access(sess, client_id, (1 << i))) {
        /* show access template */
        oauth_response_access_template(sess, buff, client_id);
        return (0);
      }
    }

    /* successful login. */
    oauth_response_app_template(sess, buff, client_id);
    return (0);
  }

  /* show user/pass login template */
  oauth_response_login_template(sess, buff, client_id, NULL);

  return (0);
}

/**
 * A response to a login html template.
 */
int oauth_response_password(shd_t *cli, char *client_id, char *username, char *password, int enable_2fa)
{
  oauth_user_t *user;
  shmap_t *sess;
  shbuf_t *buff = cli->buff_out;
  char text[1024];
  char *uri;
  char *c_id;
  int err;

  if (!username || !password)
    return (SHERR_INVAL);

  sess = oauth_sess_load(cli, NULL);
  if (!sess)
    return (SHERR_ACCESS);

  err = oauth_sess_login_verify(cli, sess, username, password);
  if (err) {
    /* re-login */
    oauth_response_login_template(sess, buff, client_id,
        "Warning: Incorrect username or password.");
    return (err);
  }

  if (shmap_get_str(sess, ashkey_str("2fa"))) {
    /* show user/pass login template */
    oauth_response_2fa_template(sess, buff, client_id);
    return (0);
  }

  if (enable_2fa) {
    oauth_register_2fa_template(sess, buff, client_id);
    return (0);
  }

  /* successful login.. move to next step. */
  oauth_response_token_template(sess, buff, client_id);
  return (0);
}


/**
 * A response to a app access template.
 */
int oauth_response_access(shd_t *cli, char *client_id, char *client_token)
{
  shmap_t *sess;
  shbuf_t *buff = cli->buff_out;
  char text[1024];
  char *sys_token;
  int scope;
  int err;
  int idx;
  int ok;

  if (!cli)
    return (SHERR_INVAL);

fprintf(stderr, "DEBUG: oauth_response_access()\n");

  sess = oauth_sess_load(cli, NULL);
  sys_token = http_token_decode(oauth_sess_token(sess));
  ok = (0 == strcmp(sys_token, client_token));
  free(sys_token);
  if (!ok) {
fprintf(stderr, "DEBUG: invalid token: sys(%s) cli(%s)\n", sys_token, client_token);
    oauth_response_app_error_template(sess, buff, client_id);
    return (SHERR_ACCESS);
  }

  scope = oauth_sess_scope(sess, client_id);
fprintf(stderr, "DEBUG: session scope for client %u\n", scope);
  for (idx = 0; idx < MAX_OAUTH_SCOPE; idx++) {
    if (scope & (1 << idx)) {
      oauth_sess_access_grant(sess, client_id, (1 << idx));
fprintf(stderr, "DEBUG: granted client '%s' with access '%s'\n", client_id, oauth_scope_label(1 << idx));
    }
  }

  oauth_response_token_template(sess, buff, client_id);

  return (0);
}

int oauth_response_2fa(shd_t *cli, char *token, char *client_id, char *code, int enable_2fa)
{
  shbuf_t *buff = cli->buff_out;
  shmap_t *sess;
  oauth_user_t *user;
  char key_str[256];
  char text[1024];
  char username[MAX_SHARE_NAME_LENGTH];
  char *secret;
  char *login_token;
  char *user_token;
  char *uri;
  char *c_id;
  char *str;
  int scope;
  int err;
  int idx;
  int ok;


  if (!cli || !client_id)
    return (SHERR_INVAL);

  sess = oauth_sess_load(cli, NULL);
  if (!sess) { 
    /* re-login */
    oauth_response_login_template(sess, buff, client_id, NULL);
    return (SHERR_ACCESS);
  }

  if (!token) {
    /* re-login */
    oauth_response_login_template(sess, buff, client_id, NULL);
    return (SHERR_ACCESS);
  }

  if (shmap_get_str(sess, ashkey_str("2fa"))) {
    /* already enabled */
    enable_2fa = FALSE;
  }

  secret = oauth_sess_2fa_secret(sess);

  str = shmap_get_str(sess, ashkey_str("username"));
  memset(username, 0, sizeof(username));
  if (str)
    strncpy(username, str, sizeof(username) - 1);


  login_token = oauth_sess_token(sess);
  user_token = http_token_decode(token);
  ok = (0 == strcmp(login_token, user_token));
  free(user_token);
  if (!ok) {
    oauth_response_login_template(sess, buff, client_id, NULL);
    return (SHERR_ACCESS);
  } 

  ok = oauth_2fa_verify(secret, code);
  if (!ok && !enable_2fa) {
    /* re 2fa */
    oauth_response_2fa_template(sess, buff, client_id);
    return (SHERR_ACCESS);
  }

  if (ok && enable_2fa) {
    /* session setting */
    shmap_set_astr(sess, ashkey_str("2fa"), "on");

    /* persistent setting */
    user = oauth_userdb_load(username);
    if (user) {
      user->flags |= OAF_2FA;
      oauth_userdb_save(user);
      oauth_userdb_free(&user);
    }
  }

  oauth_response_token_template(sess, buff, client_id); 

  return (0);
}

int oauth_response_favicon(shbuf_t *buff)
{
  char text[1024];

  sprintf(text,
      "HTTP/1.1 200 OK\r\n"
      "Accept-Ranges: bytes\r\n"
      "Content-Length: %u\r\n"
      "Content-Type: image/vnd.microsoft.icon\r\n"
      "\r\n",
      (unsigned int)FAVICON_ICO_SIZE);

  shbuf_catstr(buff, text);
  shbuf_cat(buff, FAVICON_ICO, FAVICON_ICO_SIZE);

}



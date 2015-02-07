

#include "pub_server.h"
#include "bits.h"

pubuser_t *_pubd_users;
int MAX_PUBUSER_NAME_LENGTH;

static int _pubd_msgqid;

static shmeta_t *fsync_preferences;
static char *fsync_preferences_data;


int fsync_shpref_init(int uid)
{
  shmeta_t *h;
  struct stat st;
  char *path;
  char *data;
  shkey_t *key;
  size_t data_len;
  size_t len;
  int err;
  int b_of;

  h = shmeta_init();
  if (!h)
    return (SHERR_NOMEM);

  key = (shkey_t *)calloc(1, sizeof(shkey_t));
  if (!key)
    return (SHERR_NOMEM);

  path = shpref_path(uid);
  err = shfs_read_mem(path, &data, &data_len);
  if (!err) { /* file may not have existed. */
    b_of = 0;
    while (b_of < data_len) {
      shmeta_value_t *hdr = (shmeta_value_t *)(data + b_of);
      memcpy(key, &hdr->name, sizeof(shkey_t));
      shmeta_set_str(h, key, data + b_of + sizeof(shmeta_value_t));

      b_of += sizeof(shmeta_value_t) + hdr->sz;
    }
  }

  free(key);

  fsync_preferences = h;
  fsync_preferences_data = data;

  return (0);
}

const char *fsync_shpref_get(char *pref, char *default_value)
{
  static char ret_val[SHPREF_VALUE_MAX+1];
  char tok[SHPREF_NAME_MAX + 16];
  shmeta_value_t *val;
  shkey_t *key;
  int err;

  if (!fsync_preferences)
    return (default_value);

  err = shpref_init();
  if (err)
    return (default_value);

  memset(tok, 0, sizeof(tok));
  strncpy(tok, pref, SHPREF_NAME_MAX);
  key = ashkey_str(tok);
  val = shmeta_get(fsync_preferences, key);

  memset(ret_val, 0, sizeof(ret_val));
  if (!val) {
    if (default_value)
      strncpy(ret_val, default_value, sizeof(ret_val) - 1);
  } else {
    strncpy(ret_val, (char *)val->raw, sizeof(ret_val) - 1);
  }

  return (ret_val);
}

void fsync_shpref_free(void)
{

  if (!fsync_preferences)
    return;

  shmeta_free(&fsync_preferences);
  fsync_preferences = NULL;

  free(fsync_preferences_data);
  fsync_preferences_data = NULL;
}


pubuser_t *pubd_user_add(int uid, char *username, char *userpass, char *path)
{
  pubuser_t *u;
  char hostname[256];
  char uname[1024];
  char upass[1024];
  char *ptr;

#ifdef HAVE_GETSPNAM
  /* check for shadow password */
  if (*username) {
    struct spwd *spwd;

    spwd = getspnam(username);
    if (spwd) {
      userpass = spwd->sp_pwdp; /* use shadow passwd */
fprintf(stderr, "DEBUG: found %s' shadow pass '%s'\n", username, userpass);
}
  }
#endif

  memset(uname, 0, sizeof(uname));
  memset(hostname, 0, sizeof(hostname));
  gethostname(hostname, sizeof(hostname)-1);
  sprintf(uname, "%s@%s", username, hostname);

  memset(upass, 0, sizeof(upass));
  strncpy(upass, userpass, sizeof(upass) - 1);

  fsync_shpref_init(uid);

  ptr = fsync_shpref_get(SHPREF_ACC_NAME, uname);
  if (0 != strcmp(ptr, uname))
    strncpy(uname, ptr, sizeof(uname));

  ptr = fsync_shpref_get(SHPREF_ACC_PASS, userpass);
  strncpy(upass, ptr, sizeof(upass));

  fsync_shpref_free();

  /* ensure this is unique path */
  for (u = _pubd_users; u; u = u->next) {
    if (0 == strcmp(u->root_path, path))
      break;
  }
  if (u)
    return (NULL); /* initial user owns path */

  for (u = _pubd_users; u; u = u->next) {
    if (0 == strcmp(u->name, uname))
      break;
  }
  if (!u) {
    u = (pubuser_t *)calloc(1, sizeof(pubuser_t));
    if (!u)
      return (NULL);

    strncpy(u->name, uname, sizeof(u->name) - 1);
  }

  strncpy(u->pass, upass, sizeof(u->pass) - 1);
  strncpy(u->root_path, path, sizeof(u->root_path) - 1);

  pubd_user_generate(u);

  u->next = _pubd_users;
  _pubd_users = u;

fprintf(stderr, "DEBUG: pubd_user_add[%x]: uname(%s) upass(%s) path(%s)\n", u, uname, upass, path);

  return (u);
}

int pubd_user_validate(pubuser_t *u, char *pass)
{
  char *enc;

  if (!u)
    return (SHERR_INVAL);

  if (!pass)
    pass = "";

  if (strlen(u->pass) >= 2) {
    enc = crypt(pass, u->pass);
    if (0 != strcmp(enc, u->pass))
      return (SHERR_ACCESS);
  }

  return (0);
}

/** Generate account identity with shared server */
int pubd_user_generate(pubuser_t *u)
{
  shkey_t *id_key;
  uint32_t mode;
  int err;

  err = shapp_ident(shpam_uid(u->name), &id_key); 
  if (err)
    return (err);

  /* retain identity name key */
  memcpy(&u->id, id_key, sizeof(shkey_t)); 
  shkey_free(&id_key);

  return (0);
}




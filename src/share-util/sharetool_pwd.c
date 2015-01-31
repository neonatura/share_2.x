
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

#include "share.h"
#include "sharetool.h"

static int pwd_flags;


int sharetool_pwd_create(shpeer_t *peer, char *acc_name, char *acc_pass, char *id_label)
{
  int err;

  err = shapp_account_create(acc_name, acc_pass, id_label);
  if (err)
    return (err);

  return (0);  
}

int sharetool_pwd_seed_set(shpeer_t *peer, char *acc_name, char *opass, char *pass)
{
  int err;

  err = shapp_account_setpass(acc_name, opass, pass);
  if (err)
    return (err);

  return (0); 
}
#if 0
int sharetool_pwd_seed_set(fs_ino_t *file, char *acc_name, char *opass, char *pass)
{
  shfs_ino_t *shadow_file;
  shkey_t *user_key;
  shkey_t *oseed_key;
  shkey_t *seed_key;
  shkey_t *sess_key;
  int err;

  err = shpam_shadow_login(acc_name, opass, NULL, &sess_key);
  if (err) {
    /* maybe pass already set? */
    if (0 == shpam_shadow_login(acc_name, opass, NULL, NULL)) {
      return (0);
    }
    return (err);
  }

  user_key = shpam_user_gen(acc_name);
  oseed_key = shpam_seed_gen(user_key, opass); 
  seed_key = shpam_seed_gen(user_key, pass); 
  shkey_free(&user_key);

  err = shpam_shadow_setpass(shadow_file, oseed_key, seed_key, sess_key); 
  shkey_free(&oseed_key);
  shkey_free(&seed_key);
  shkey_free(&sess_key);
  if (err)
    return (err);

  return (0);
}
#endif

int sharetool_pwd_seed_verify(shfs_ino_t *file, char *acc_name, char *acc_pass)
{
  int err;

  err = shpam_shadow_login(file, acc_name, acc_pass, NULL);
  if (err)
    return (err);

  return (0);
}

int sharetool_pwd_session(shpeer_t *peer, shkey_t *seed_key)
{
  int err;
  shkey_t *sess_key;

  err = shapp_session(seed_key, &sess_key);
  if (err)
    return (err);

  fprintf(sharetool_fout, "Session Token: %s\n", shkey_print(sess_key));
  shkey_free(&sess_key);

  return (0);
}

int sharetool_pwd_print(shpeer_t *peer, shkey_t *seed_key)
{
  shadow_t *ent;

  ent = shapp_account_info(seed_key);
  if (!ent)
    return (SHERR_ACCESS);

  if (*ent->sh_label) {
    fprintf(sharetool_fout, "Identity Label: %s\n", ent->sh_label); 
  }
  fprintf(sharetool_fout, "Identity Token: %s\n", shkey_print(&ent->sh_id)); 
  fprintf(sharetool_fout, "Seed Token: %s\n", shkey_print(&ent->sh_seed)); 
  if (shtime64() < ent->sh_expire) {
    char time_str[256];
    char sess_str[256];

    memset(time_str, 0, sizeof(time_str));
    if (ent->sh_expire > shtime64())
      strcpy(time_str, shstrtime64(ent->sh_expire, NULL));
    strcpy(sess_str, shkey_print(&ent->sh_sess));

    fprintf(sharetool_fout,
        "Session Token: %s\n"
        "Session Expire: %s\n",
        sess_str, time_str);
  } else {
    fprintf(sharetool_fout, "Session Token: <empty>\n");
  }

  return (0);
}


int sharetool_passwd(char **args, int arg_cnt)
{
  shadow_t *shadow;
  shpeer_t *peer;
  shkey_t *seed_key;
  shpeer_t *app_peer;
  char acc_name[1024];
  char opass[1024];
  char pass_buf[1024];
  char vpass_buf[1024];
  char *pass;
  char *vpass;
  char *in_str; 
  char subcmd[4096];
  int cnt;
  int err;
  int i;

  cnt = 0;
  peer = NULL;
  memset(acc_name, 0, sizeof(acc_name));

  memset(subcmd, 0, sizeof(subcmd));
  for (i = 1; i < arg_cnt; i++) {
    if (0 == strcmp(args[i], "-d")) {
      pwd_flags |= SHPAM_DELETE;
    } else if (0 == strcmp(args[i], "-e")) {
      pwd_flags |= SHPAM_EXPIRE;
    } else if (0 == strcmp(args[i], "-k")) {
      pwd_flags |= SHPAM_LOCK;
    } else if (0 == strcmp(args[i], "-s")) {
      pwd_flags |= SHPAM_STATUS;
    } else if (0 == strcmp(args[i], "-t")) {
      pwd_flags |= SHPAM_SESSION;
    } else if (0 == strcmp(args[i], "-u")) {
      pwd_flags |= SHPAM_UNLOCK;
    } else if (!cnt) {
      if (!*subcmd)
        strcat(subcmd, " ");
      strcat(subcmd, args[i]);
      cnt++;
    } else {
      if (*acc_name)
        strcat(acc_name, " ");
      strncat(acc_name,  args[i], MAX_SHARE_NAME_LENGTH - strlen(acc_name) - 1);
    }
  }

  peer = NULL;
  if (*subcmd)
    peer = share_info_peer(subcmd);

  if (!*acc_name) {
    /* use default account name */
    strcpy(acc_name, get_libshare_account_name());
  }

  /* register as libshare app */
  app_peer = shapp_init(NULL, NULL, 0);

  printf ("Application: '%s'\n", shpeer_print(app_peer));
  printf ("Account name: '%s'\n", acc_name);
  printf ("\n");

  shpeer_free(&app_peer);

  memset(opass, 0, sizeof(opass));
  if (0 == strcasecmp(acc_name, get_libshare_account_name())) {
    seed_key = get_libshare_account_pass();
  } else {
    seed_key = sharetool_pwd_validate(acc_name, NULL, opass);
  }

  shadow = shapp_account_info(seed_key);
  if (!shadow) { /* generate */
    /* normal passwd update */
    fprintf(sharetool_fout, "Generating passphrase for %s..\n", acc_name);  

    memset(pass_buf, 0, sizeof(pass_buf));
    sharetool_pwd_validate(acc_name, "new", pass_buf);
    err = shapp_account_create(acc_name, pass_buf, NULL); 
    if (err)
      return (err);

    fprintf(sharetool_fout, "New account generated.\n");
    shpref_set(SHPREF_ACC_PASS, pass_buf);
    return (0);
  }

  if (!pwd_flags) {
    shkey_t *ver_key;

    /* normal passwd update */
    fprintf(sharetool_fout, "Changing passphrase for %s..\n", acc_name);  

    if (0 == strcasecmp(acc_name, get_libshare_account_name())) {
      ver_key = sharetool_pwd_validate(acc_name, NULL, opass);
      if (!shkey_cmp(ver_key, seed_key)) {
        shkey_free(&ver_key);
        return (SHERR_ACCESS);
      }
      shkey_free(&ver_key);
    }

    sharetool_pwd_validate(acc_name, "new", pass_buf);
    sharetool_pwd_validate(acc_name, "re-type new", vpass_buf);

    if (0 != strcmp(pass_buf, vpass_buf))
      return (SHERR_CANCELED);

    err = sharetool_pwd_seed_set(peer, acc_name, opass, pass_buf);
    if (err)
      return (err);

    shpref_set(SHPREF_ACC_PASS, pass_buf);
    return (0);
  }

  if (pwd_flags & SHPAM_STATUS) {
    err = sharetool_pwd_print(peer, seed_key);
    if (err) {
      fprintf(stderr, "%s: error: %s.\n", process_path, sherr_str(err));
      return (err);
    }
    return (0);
  }

  if (pwd_flags & SHPAM_SESSION) {
    err = sharetool_pwd_session(peer, seed_key);
    if (err) {
      fprintf(stderr, "%s: session error: %s.\n", process_path, sherr_str(err));
      return (err);
    }
    return (0);
  }

  if (pwd_flags & SHPAM_DELETE) {
  }

  if (pwd_flags & SHPAM_EXPIRE) {
  }


  return (0);
}

shkey_t *sharetool_pwd_validate(char *acc_name, char *state, char *ret_str)
{
  shkey_t *seed_key;
  char pass_buf[1024];
  char enc_buf[1024];
  char *enc_pass;
  char salt[256];
  char *pass;

  if (!state)
    state = "current";

  fprintf(stdout, "(%s) SHARE passphrase: ", state);
  fflush(stdout);

  memset(pass_buf, 0, sizeof(pass_buf));
  pass = fgets(pass_buf, MAX_SHARE_PASS_LENGTH, stdin);
  strtok(pass, "\r\n");

  memset(salt, 0, sizeof(salt));
#ifdef HAVE_CRYPT
  strncpy(salt, pass, 2);
  memset(enc_buf, 0, sizeof(enc_buf));
  enc_pass = crypt(pass, salt);
  memset(pass_buf, 0, sizeof(pass_buf));
  strncpy(pass_buf, enc_pass, MAX_SHARE_PASS_LENGTH - 1);
#endif
  seed_key = shpam_seed(acc_name, pass, 0);

  if (ret_str)
    strcpy(ret_str, pass);

  return (seed_key);  
}



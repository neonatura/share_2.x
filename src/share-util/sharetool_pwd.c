
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


#if 0
int sharetool_pwd_create(shpeer_t *peer, char *acc_name, char *acc_pass)
{
  int err;

  err = shapp_account_create(acc_name, acc_pass, NULL);
  if (err)
    return (err);

  return (0);  
}
#endif

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

int sharetool_pwd_session(shpeer_t *peer, shseed_t *seed)
{
  int err;
  shkey_t *sess_key;

  err = shapp_session(seed, &sess_key);
  if (err)
    return (err);

  fprintf(sharetool_fout, "Session Token: %s\n", shkey_print(sess_key));
  shkey_free(&sess_key);

  return (0);
}

int sharetool_pwd_print(shpeer_t *peer, uint64_t uid)
{
  shadow_t shadow;
  int err;

  err = shapp_account_info(uid, &shadow, NULL);
  if (err)
    return (err);

  fprintf(sharetool_fout, "User ID: %llu\n", shadow.sh_uid);
  fprintf(sharetool_fout, "Identity Token: %s\n", shkey_print(&shadow.sh_id)); 

  if (shtime_before(shtime(), shadow.sh_expire)) {
    char time_str[256];
    char sess_str[256];
    shnum_t lat, lon;

    memset(time_str, 0, sizeof(time_str));
    if (shtime_after(shadow.sh_expire, shtime()))
      strcpy(time_str, shstrtime(shadow.sh_expire, NULL));
    strcpy(sess_str, shkey_print(&shadow.sh_sess));

    fprintf(sharetool_fout,
        "Session Token: %s\n"
        "Session Expire: %s\n",
        sess_str, time_str);

    shgeo_loc(&shadow.sh_geo, &lat, &lon, NULL);
    fprintf(sharetool_fout, "Geo: %Lf,%Lf\n", lat, lon);

    fprintf(sharetool_fout, "Real Name: %s\n", shadow.sh_realname);
    fprintf(sharetool_fout, "Email: %s\n", shadow.sh_email);
    fprintf(sharetool_fout, "SHC: %s\n", shadow.sh_sharecoin);
  } else {
    fprintf(sharetool_fout, "Session Token: <empty>\n");
  }


  return (0);
}

void sharetool_pwd_input(char *acc_name, char *state, char *ret_str)
{
  shseed_t *seed;
  char pass_buf[1024];
  char *pass;

  if (!state)
    state = "current";

  fprintf(stdout, "(%s) SHARE passphrase: ", state);
  fflush(stdout);

  memset(pass_buf, 0, sizeof(pass_buf));
  pass = fgets(pass_buf, MAX_SHARE_PASS_LENGTH-1, stdin);
  strtok(pass, "\r\n");

  strcpy(ret_str, pass);
}

int sharetool_passwd(char **args, int arg_cnt)
{
  shadow_t shadow;
  shseed_t seed;
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
  uint64_t uid;
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

  if (!*acc_name) {
    /* use default account name */
    strcpy(acc_name, get_libshare_account_name());
  }

  /* register as libshare app */
  app_peer = shapp_init(NULL, NULL, 0);

  peer = NULL;
  if (*subcmd) {
    peer = share_appinfo_peer(subcmd);
  }

  if (peer)
    printf ("Peer: '%s'\n", shpeer_print(peer));
  printf ("Account name: '%s'\n", acc_name);
  printf ("\n");

  shpeer_free(&app_peer);

  memset(opass, 0, sizeof(opass));

  uid = shpam_uid(acc_name);
  err = shapp_account_info(uid, &shadow, &seed);
  if (err && err != SHERR_NOENT) {
fprintf(stderr, "%s: %s: account '%s'.\n", process_path, sherrstr(err), acc_name);
return (-1);
  }
if (err == SHERR_NOENT) {
    /* generate account */
    if (0 != strcasecmp(acc_name, get_libshare_account_name())) {
      fprintf(stderr, "%s: error: account '%s' does not exist\n", process_path, acc_name);
      return (-1);
    }


#if 0
    if (0 == strcasecmp(acc_name, get_libshare_account_name())) {
      seed = shpam_pass_sys(acc_name);
    } else {
      seed = sharetool_pwd_validate(acc_name, NULL, salt, opass);
    }
#endif
   

    /* normal passwd update */
    fprintf(sharetool_fout, "Generating passphrase for %s..\n", acc_name);  

    memset(pass_buf, 0, sizeof(pass_buf));
    sharetool_pwd_input(acc_name, "new", pass_buf);
    err = shapp_account_create(acc_name, pass_buf, NULL); 
    if (err)
      return (err);

    fprintf(sharetool_fout, "New account generated.\n");
//    shpref_set(SHPREF_ACC_PASS, pass_buf);
    return (0);
  }

  if (!pwd_flags) {
    shkey_t *ver_key;

    /* normal passwd update */
    fprintf(sharetool_fout, "Changing passphrase for %s..\n", acc_name);  

    if (0 == strcasecmp(acc_name, get_libshare_account_name())) {
      shseed_t *ver_seed;
      sharetool_pwd_input(acc_name, NULL, opass);
      ver_seed = shpam_pass_gen(acc_name, opass, seed.seed_salt); 
      if (!shkey_cmp(&ver_seed->seed_key, &seed.seed_key)) {
        shkey_free(&ver_key);
        return (SHERR_ACCESS);
      }
      shkey_free(&ver_key);
    }

    sharetool_pwd_input(acc_name, "new", pass_buf);
    sharetool_pwd_input(acc_name, "re-type new", vpass_buf);

    if (0 != strcmp(pass_buf, vpass_buf))
      return (SHERR_CANCELED);

    err = sharetool_pwd_seed_set(peer, acc_name, opass, pass_buf);
    if (err)
      return (err);

//    shpref_set(SHPREF_ACC_PASS, pass_buf);
    return (0);
  }

  if (pwd_flags & SHPAM_STATUS) {
    err = sharetool_pwd_print(peer, uid);
    if (err) {
      fprintf(stderr, "%s: error: %s.\n", process_path, sherrstr(err));
      return (err);
    }
    return (0);
  }

  if (pwd_flags & SHPAM_SESSION) {
    err = sharetool_pwd_session(peer, &seed);
    if (err) {
      fprintf(stderr, "%s: session error: %s.\n", process_path, sherrstr(err));
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

#if 0
shseed_t *sharetool_pwd_validate(char *acc_name, char *state, uint64_t salt, char *ret_str)
{
  static shseed_t ret_seed;
  shseed_t *seed;
  char pass_buf[1024];
  char *pass;

  if (!state)
    state = "current";

  fprintf(stdout, "(%s) SHARE passphrase: ", state);
  fflush(stdout);

  memset(pass_buf, 0, sizeof(pass_buf));
  pass = fgets(pass_buf, MAX_SHARE_PASS_LENGTH-1, stdin);
  strtok(pass, "\r\n");

  if (ret_str)
    strcpy(ret_str, pass);

  seed = shpam_pass_gen(acc_name, pass_buf, salt);
  memcpy(&ret_seed, seed, sizeof(shseed_t));
  return (&ret_seed);
}
#endif




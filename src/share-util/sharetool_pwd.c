
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

#define PWD_DELETE (1 << 0)
#define PWD_EXPIRE (1 << 1)
#define PWD_LOCK (1 << 2)
#define PWD_STATUS (1 << 3)
#define PWD_SESSION (1 << 4)
#define PWD_UNLOCK (1 << 5)


int sharetool_pwd_seed_set(shfs_ino_t *file, char *acc_name, char *opass, char *pass)
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

int sharetool_pwd_seed_verify(shfs_ino_t *file, char *acc_name, char *acc_pass)
{
  int err;

  err = shpam_shadow_login(file, acc_name, acc_pass, NULL, NULL);
  if (err)
    return (err);

  return (0);
}

int sharetool_pwd_print(shfs_ino_t *shadow_file, char *acc_name)
{
  shadow_t *ent;
  shkey_t *seed_key;
  char time_str[256];
  char seed_str[256];
  char sess_str[256];
  char flag_str[256];

  if (0 == strcasecmp(acc_name, get_libshare_account_name())) {
    seed_key = get_libshare_account_pass();
  } else {
    /* ask for pass and validate via pam_shadow_login */
  }

  ent = shpam_shadow(shadow_file, seed_key);
  if (!ent) {
    return (SHERR_NOENT);
  }

  memset(time_str, 0, sizeof(time_str));
  if (ent->sh_expire > shtime64())
    strcpy(time_str, shstrtime64(ent->sh_expire, NULL));

  fprintf(sharetool_fout, 
      "Identity: %s (%s)\n"
      "Seed Token: %s\n"
      "Session Token: %s\n"
      "Session Expire: %s\n",
      ent->sh_label, flag_str, 
      seed_str, sess_str, time_str);
  return (0);
}

int sharetool_passwd(char **args, int arg_cnt)
{
  shfs_t *fs;
  shfs_ino_t *shadow_file;
  shfs_ino_t *file;
  shpeer_t *peer;
  char acc_name[1024];
  char opass_buf[1024];
  char pass_buf[1024];
  char vpass_buf[1024];
  char *opass;
  char *pass;
  char *vpass;
  char *in_str; 
  int cnt;
  int err;
  int i;

  cnt = 0;
  file = NULL;
  fs = NULL;
  peer = NULL;
  memset(acc_name, 0, sizeof(acc_name));

  for (i = 1; i < arg_cnt; i++) {
    if (0 == strcmp(args[i], "-d")) {
      pwd_flags |= PWD_DELETE;
    } else if (0 == strcmp(args[i], "-e")) {
      pwd_flags |= PWD_EXPIRE;
    } else if (0 == strcmp(args[i], "-k")) {
      pwd_flags |= PWD_LOCK;
    } else if (0 == strcmp(args[i], "-s")) {
      pwd_flags |= PWD_STATUS;
    } else if (0 == strcmp(args[i], "-t")) {
      pwd_flags |= PWD_SESSION;
    } else if (0 == strcmp(args[i], "-u")) {
      pwd_flags |= PWD_UNLOCK;
    } else if (!cnt) {
      file = sharetool_file(args[0], &fs);
      if (!file)
        return (SHERR_INVAL);
      peer = &fs->peer;
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

  printf ("Account name: %s\n", acc_name);
  printf ("\n");

  shadow_file = shpam_shadow_file(fs);

  if (!pwd_flags) {
    /* normal passwd update */
    fprintf(sharetool_fout, "Changing passphrase for %s..\n", acc_name);  
    fprintf(stdout, "(current) SHARE passphrase: ");
    fflush(stdout);

    opass = fgets(opass_buf, MAX_SHARE_PASS_LENGTH, stdin);
    err = sharetool_pwd_seed_verify(shadow_file, acc_name, opass);
    if (err)
      return (err);

    fprintf(stdout, "(new) SHARE passphrase: ");
    fflush(stdout);
    pass = fgets(pass_buf, MAX_SHARE_PASS_LENGTH, stdin);
    fprintf(stdout, "(new) Retype SHARE passphrase: ");
    fflush(stdout);
    vpass = fgets(vpass_buf, MAX_SHARE_PASS_LENGTH, stdin);
    if (0 != strcmp(pass, vpass))
      return (SHERR_CANCELED);

    return (sharetool_pwd_seed_set(shadow_file, acc_name, opass, pass));
  }

  if (pwd_flags & PWD_STATUS) {
    sharetool_pwd_print(shadow_file, acc_name);
    return (0);
  }

  if (pwd_flags & PWD_DELETE) {
  }

  if (pwd_flags & PWD_EXPIRE) {
  }


  return (0);
}


/*
 * @copyright
 *
 *  Copyright 2014 Neo Natura
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
 *
 *  @endcopyright
 *
 *  @file share.h 
 *  @brief Public share synchronization daemon.
 *  @date 2014
 *
 *  Provides: Distribution of data in the "$HOME/share" directory.
 */  

#ifndef __SHARE_PUB_DAEMON__PUB_USER_H__
#define __SHARE_PUB_DAEMON__PUB_USER_H__
 
typedef struct pubuser_t
{
  char name[256];
  char pass[256];
  char root_path[PATH_MAX+1];
  shfs_t *fs;
  shkey_t id;
  int err;

  struct pubuser_t *next;
} pubuser_t;

extern pubuser_t *_pubd_users;
extern int MAX_PUBUSER_NAME_LENGTH;


pubuser_t *pubd_user_add(int uid, char *uname, char *upass, char *path);

int pubd_user_validate(pubuser_t *u, char *pass);

int pubd_user_generate(pubuser_t *u);

#endif /* ndef __SHARE_PUB_DAEMON__PUB_USER_H__ */


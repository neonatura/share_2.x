/*
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
 */  

#include "share.h"
#include "sharetool.h"


int sharetool_pref(char *subcmd, char *path)
{
  char cmd[256];
  char tok[256];
  char *str_val;
  char *ptr;


  memset(cmd, 0, sizeof(cmd));
  strncpy(cmd, subcmd, sizeof(cmd) - 2);
  strtok(cmd, ":");
  if (!*cmd)
    return (SHERR_INVAL);

  str_val = NULL;
  strncpy(tok, subcmd+strlen(cmd)+1, sizeof(tok) - 1);
  ptr = strchr(tok, '=');
  if (ptr) {
    str_val = ptr + 1;
    *ptr = '\0';
  } 

  if (0 == strcmp(cmd, "unset")) {
    str_val = NULL;
    strcpy(cmd, "set");
  }
  if (0 == strcmp(cmd, "set")) {
    shpref_set(tok, str_val);
    if (str_val)
      printf ("Preference '%s' set: %s\n", tok, str_val);
    else
      printf ("Preference '%s' unset.\n", tok);
  } else if (0 == strcmp(cmd, "get")) {
    char *str_val = shpref_get(tok, NULL);
    printf ("Preference '%s': %s\n", tok, str_val);
    return;
  }
  
}


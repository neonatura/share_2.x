


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
 */

#ifndef __SEXE__SEXE_EVENT_H__
#define __SEXE__SEXE_EVENT_H__




typedef struct sexe_event_t {
  const char mod_name[MAX_SEXE_NAME_LENGTH];
  shkey_t reg_key;
  uint32_t event_type;
} sexe_event_t;


int sexe_event_remove(lua_State *L, int e_type, char *e_name);

int sexe_event_handle(lua_State *L, int e_type, shjson_t *json);
int sexe_event_call(lua_State *L, const char *f_name, int e_type, shjson_t *json);


#endif /* ndef __SEXE__SEXE_EVENT_H__ */

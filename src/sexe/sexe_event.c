

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

#include "sexe.h"


shmap_t *event_map;

char *sexe_event_init(int e_type, const char *e_name)
{
  static char key_str[256];
  sexe_event_t *e;
  shkey_t *key;

  if (!event_map) {
    event_map = shmap_init();
  }

  e = (sexe_event_t *)calloc(1, sizeof(sexe_event_t));
  if (!e)
    return (SHERR_NOMEM);

  e->event_type = e_type;
  strncpy(e->mod_name, e_name, sizeof(e->mod_name) - 1);
  key = shkey_bin(e, sizeof(sexe_event_t));
  memcpy(&e->reg_key, key, sizeof(shkey_t));
  shmap_set_ptr(event_map, key, e);
  strncpy(key_str, shkey_hex(key), sizeof(key_str) - 1);
  shkey_free(&key);

  return (key_str);
}


int sexe_event_remove(lua_State *L, int e_type, char *e_name)
{
  sexe_event_t t_event;
  sexe_event_t *e;
  shkey_t *key;
  int err;

  memset(&t_event, 0, sizeof(t_event));
  t_event.event_type = e_type;
  strncpy(t_event.mod_name, e_name, sizeof(t_event.mod_name) - 1);
  key = shkey_bin(&t_event, sizeof(sexe_event_t));

  /* remove event's global callback */
  lua_pushnil(L); 
  lua_setglobal(L, shkey_hex(key));

  e = (sexe_event_t *)shmap_get_ptr(event_map, key);
  if (e) {
    shmap_unset(event_map, key);
    free(e);
  }
  shkey_free(&key);

  return (0);
}

int sexe_event_handle(lua_State *L, int e_type, shjson_t *json)
{
  const shmap_t *h = event_map;
  shmap_index_t *hi;
  sexe_event_t *event;
  char *key;
  char *val;
  size_t len;
  int flag;

  for (hi = shmap_first(h); hi; hi = shmap_next(hi)) {
    shmap_self(hi,(void*) &key, (void*) &val, &len, &flag);
    if (!(flag & SHMAP_BINARY))
      continue;

    event = (sexe_event_t *)val;
    if (event->event_type != e_type)
      continue; /* wrong event type */

    sexe_event_call(L, shkey_hex(&event->reg_key), e_type, json);
  }

}


/**
 * Call a lua pre-stored global Lua function with one argument.
 */
int sexe_event_call(lua_State *L, const char *f_name, int e_type, shjson_t *json)
{

  lua_getglobal(L, f_name); /* push global func ref to stack */
  lua_pushnumber(L, e_type);
  if (json)
    sexe_table_set(L, json);
  return (lua_pcall(L, json ? 2 : 1, 0, 0)); 
}

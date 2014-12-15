
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


int _lfunc_sexe_shkey(lua_State *L) 
{
  shkey_t *key;
  char *seed;
  int seed_num;

  seed = luaL_checkstring(L, 1);
  if (!seed)
    seed_num = luaL_checknumber(L, 1);

  if (seed)
    key = shkey_str(seed);
  else
    key = shkey_num(seed_num);

  lua_pushstring(L, shkey_print(key));

  shkey_free(&key);
  return (1); /* (1) string key */
}


int _lfunc_sexe_shencode(lua_State *L)
{
  const char *raw_str = luaL_checkstring(L, 1);
  const char *key_str = luaL_checkstring(L, 2);
  unsigned char *data;
  size_t data_len;
  shkey_t *key;
  int err;

  if (!raw_str)
    raw_str = "";

  key = shkey_gen(key_str);  
  err = shencode(raw_str, strlen(raw_str), &data, &data_len, key);
  shkey_free(&key);
  if (err) {
    lua_pushnil(L);
    return (1); /* (1) nil */
  }

  lua_pushlstring(L, data, data_len);
  free(data);
  return (1); /* (1) encoded string */ 
}

int _lfunc_sexe_shdecode(lua_State *L)
{
  const char *enc_str = luaL_checkstring(L, 1);
  const char *key_str = luaL_checkstring(L, 2);
  shkey_t *key;
  size_t data_len;
  char *data;
  int err;

  if (!enc_str)
    enc_str = "";

  key = shkey_gen(key_str);  
  err = shdecode(enc_str, strlen(enc_str), &data, &data_len, key);
  shkey_free(&key);
  if (err) {
    lua_pushnil(L);
    return (1); /* (1) nil */
  }

  lua_pushstring(L, data);
  free(data);
  return (1); /* (1) encoded string */ 
}



void install_sexe_functions(lua_State *L)
{
  lua_pushcfunction(L, _lfunc_sexe_shkey);
  lua_setglobal(L, "shkey");

  lua_pushcfunction(L, _lfunc_sexe_shencode);
  lua_setglobal(L, "shencode");

  lua_pushcfunction(L, _lfunc_sexe_shdecode);
  lua_setglobal(L, "shdecode");
}

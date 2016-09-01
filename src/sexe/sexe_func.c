
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


static int _lbase_abs(lua_State *L)
{
  double d = lua_tonumber(L, 1);
  lua_pushnumber(L, fabs(d));
  return (1); /* (1) math 'absolute' of arg */
}

static int _lbase_ceil(lua_State *L)
{
  double d = lua_tonumber(L, 1);
  lua_pushnumber(L, ceil(d));
  return 1; /* math 'ceil' of arg */
}

static int _lbase_clamp(lua_State *L)
{
  double d = lua_tonumber(L, 1);
  double min = lua_tonumber(L, 2);
  double max = lua_tonumber(L, 3);
  if (d > max) d = max;
  if (d < min) d = min;
  lua_pushnumber(L, d);
  return 1; /* math 'clamp' of arg */
}

static int _lbase_floor(lua_State *L)
{
  double d = lua_tonumber(L, 1);
  lua_pushnumber(L, floor(d));
  return 1; /* math 'floor' of arg */
}
static int _lbase_max(lua_State *L)
{
  double d1 = lua_tonumber(L, 1);
  double d2 = lua_tonumber(L, 2);
  lua_pushnumber(L, d1 > d2 ? d1 : d2);
  return 1; /* math 'max' of arg */
}
static int _lbase_min(lua_State *L)
{
  double d1 = lua_tonumber(L, 1);
  double d2 = lua_tonumber(L, 2);
  lua_pushnumber(L, d1 < d2 ? d1 : d2);
  return 1; /* math 'min' of arg */
}
static int _lbase_mod(lua_State *L)
{
  double d = lua_tonumber(L, 1);
  double m = lua_tonumber(L, 2);
  if (m == 0) {
    lua_pushnumber(L, 0);
  } else {
    lua_pushnumber(L, (double)((int)d % (int)m));
  }
  return 1; /* math 'mod' of arg */
}

static int _lbase_pow(lua_State *L)
{
  double d = lua_tonumber(L, 1);
  double m = lua_tonumber(L, 2);
  lua_pushnumber(L, pow(d, m));
  return 1; /* math 'pow' of arg */
}
static int _lbase_random(lua_State *L)
{
  double d = (double)shrand();
  lua_pushnumber(L, d);
  return 1; /* math 'random' integral */
}

static int _lbase_round(lua_State *L)
{
  double d = lua_tonumber(L, 1);
  lua_pushnumber(L, round(d));
  return 1; /* math 'round' of arg */
}

static int _lbase_sign(lua_State *L)
{
  double d = lua_tonumber(L, 1);
  double ret_num;

  if (d == 0) {
    lua_pushnumber(L, d);
  } else {
    lua_pushnumber(L, d > 0 ? 1 : -1);
  }
  return (1); /* (1) math 'sign' of number arg */
}

static int _lbase_sqrt(lua_State *L)
{
  double d = lua_tonumber(L, 1);
  lua_pushnumber(L, sqrt(d));
  return 1; /* math 'sqrt' of arg */
}


static const luaL_Reg base_funcs[] = {
  {"abs", _lbase_abs},
  {"ceil", _lbase_ceil},
  {"clamp", _lbase_clamp},
  {"floor", _lbase_floor},
  {"max", _lbase_max},
  {"min", _lbase_min},
  {"mod", _lbase_mod},
  {"pow", _lbase_pow},
  {"random", _lbase_random},
  {"round", _lbase_round},
  {"sign", _lbase_sign},
  {"sqrt", _lbase_sqrt}
};
#define MAX_CORE_FUNCTIONS 12

void install_base_functions(lua_State *L)
{
  int i;

  for (i = 0; i < MAX_CORE_FUNCTIONS; i++) {
    lua_pushcfunction(L, base_funcs[i].func);
    lua_setglobal(L, base_funcs[i].name);
  }

}


static int _lfunc_sexe_shkey(lua_State *L) 
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


static int _lfunc_sexe_shencode(lua_State *L)
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

static int _lfunc_sexe_shdecode(lua_State *L)
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

static int _lfunc_register_event(lua_State *L)
{
  int e_type = (int)luaL_checknumber(L, 2);
  const char *e_name = (int)luaL_checkstring(L, 3);
  char *ptr;

  /* dup func ont stack. */ 
  lua_pushvalue(L, 1);
  ptr = sexe_event_init(e_type, e_name);
  lua_setglobal(L, ptr);

  return (0);
}

static int _lfunc_trigger_event(lua_State *L)
{
  shjson_t *json;
  int e_type = (int)luaL_checknumber(L, 1);
  int t_reg = 0;

  json = NULL;
  if (lua_istable(L, 2)) {
    lua_pushvalue(L, 2);
    json = sexe_table_get(L);
  }

  /* second optional arg; table of data. */
  sexe_event_handle(L, e_type, json);

  if (json)
    shjson_free(&json);

}

static int _lfunc_unregister_event(lua_State *L)
{
  int e_type = (int)luaL_checknumber(L, 1);
  const char *e_name = (int)luaL_checkstring(L, 2);
  int err;

  err = sexe_event_remove(L, e_type, e_name);
  if (err)
    return (err);

  return (0);
}

void sexe_global_nset(lua_State *L, char *name, double d)
{
  lua_pushnumber(L, d);
  lua_setglobal(L, name);
}



void install_event_functions(lua_State *L)
{

  sexe_global_nset(L, "EVENT_INIT", EVENT_INIT);
  sexe_global_nset(L, "EVENT_TERM", EVENT_TERM);
  sexe_global_nset(L, "EVENT_TIMER", EVENT_TIMER);

  lua_pushcfunction(L, _lfunc_register_event);
  lua_setglobal(L, "register_event");

  lua_pushcfunction(L, _lfunc_trigger_event);
  lua_setglobal(L, "trigger_event");

  lua_pushcfunction(L, _lfunc_unregister_event);
  lua_setglobal(L, "unregister_event");

}

#if 0
int _lfunc_ctest_table(lua_State *L)
{
  int t_reg;

  lua_pushvalue(L, 2);
  t_reg = luaL_ref(L, LUA_REGISTRYINDEX);

  /* dup func ont stack. */ 
  lua_pushvalue(L, 1);
  lua_rawgeti(L, LUA_REGISTRYINDEX, t_reg);
  lua_pcall(L, 1, 0, 0);

  return (0);
}
#endif

void install_libshare_functions(lua_State *L)
{

  lua_pushcfunction(L, _lfunc_sexe_shkey);
  lua_setglobal(L, "shkey");

  lua_pushcfunction(L, _lfunc_sexe_shencode);
  lua_setglobal(L, "shencode");

  lua_pushcfunction(L, _lfunc_sexe_shdecode);
  lua_setglobal(L, "shdecode");

}

void install_sexe_functions(lua_State *L)
{

  install_base_functions(L);
  install_event_functions(L);
  install_libshare_functions(L);

}





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
#include "lua.h"

#define SEXELIB_API /* .. */


SEXELIB_API int sexe_loadbuffer(lua_State *S, unsigned char *data, size_t data_len)
{
  int err;

  err = luaL_loadbuffer(S, data, data_len, NULL);

  return (err);
}

SEXELIB_API lua_State *sexe_init(void)
{
  lua_State *S;

  S = (lua_State *)luaL_newstate();
  luaL_openlibs((lua_State *)S);
  install_sexe_functions(S);

  return (S);
}


SEXELIB_API int sexe_load_inode(lua_State *S, SHFL *fl)
{
  unsigned char *data;
  size_t data_len;
  int err;

  err = shfs_file_read(fl, &data, &data_len);
  if (err)
    return (err);

  err = sexe_loadbuffer(S, data, data_len);
  if (data) free(data);
  if (err)
    return (err);

  return (err);
}

SEXELIB_API int sexe_load_stack(lua_State *S, sexe_stack_t *chain, size_t stack_len)
{
  int err;

  err = sexe_loadbuffer(S, chain, stack_len);

  return (err);
}

SEXELIB_API int sexe_call(lua_State *S, int nargs, int nresults, int errfunc)
{
  return (lua_pcall(S, nargs, nresults, errfunc));
} 


SEXELIB_API void sexe_free(lua_State *S)
{
  lua_close(S); 
}



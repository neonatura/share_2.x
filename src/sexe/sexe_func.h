
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


#ifndef __SEXE__SEXE_FUNC_H__
#define __SEXE__SEXE_FUNC_H__

/**
 * Builtin lua calls to access libshare C library functions.
 * @ingroup sexe
 * @defgroup sexe_func 
 * @{
 */


/** Installs the lua calls to common C functions. */
void install_base_functions(lua_State *L);

/** Installs the lua calls to access libshare C library functions. */
void install_sexe_functions(lua_State *L);

int install_sexe_userdata(sexe_t *S, char *tag);

int update_sexe_userdata(sexe_t *S);

 
/**
 *
 * @}
 */

#endif

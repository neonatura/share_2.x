
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

struct shlicense_t
{
  /** The originating peer granting the license. */
  shpeer_t lic_peer;
  /** The digital signature of the licencing content. */
  shsig_t lic_sig;
  /** The key reference to the licensing content. 
  shkey_t lic_name;
*/
  /** A key reference to the licensing certificate. */
  shkey_t lic_cert;
  /** The account identity that the license is applicable for. */
  shkey_t lic_id;
  /** A key referencing this license instance. */
  shkey_t lic_key;
  /** When the license expires. */
  shtime_t lic_expire;
  /** total usde cost of license */
  uint64_t lic_fee;
};
typedef struct shlicense_t shlicense_t;



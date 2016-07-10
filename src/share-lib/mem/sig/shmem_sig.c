

/*
 * @copyright
 *
 *  Copyright 2013 Neo Natura 
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

#include "share.h"




const char *shsig_alg_str(int alg)
{
  static char ret_buf[4096];

  memset(ret_buf, 0, sizeof(ret_buf));

  if (alg & SHKEY_ALG_SHR)
    strcat(ret_buf, "SHR ");

  if (alg & SHKEY_ALG_MD5)
    strcat(ret_buf, "MD5 ");
  else if (alg & SHKEY_ALG_MD4)
    strcat(ret_buf, "MD4 ");
  else if (alg & SHKEY_ALG_MD2)
    strcat(ret_buf, "MD2 ");

  if (alg & SHKEY_ALG_SHA512)
    strcat(ret_buf, "SHA512 ");
  else if (alg & SHKEY_ALG_SHA384)
    strcat(ret_buf, "SHA384 ");
  else if (alg & SHKEY_ALG_SHA256)
    strcat(ret_buf, "SHA256 ");
  else if (alg & SHKEY_ALG_SHA224)
    strcat(ret_buf, "SHA224 ");
  else if (alg & SHKEY_ALG_SHA1)
    strcat(ret_buf, "SHA1 ");

  if (alg & SHKEY_ALG_RSA)
    strcat(ret_buf, "RSA ");

  if (alg & SHKEY_ALG_ECDSA)
    strcat(ret_buf, "ECDSA ");

  if (*ret_buf)
    ret_buf[strlen(ret_buf)-1] = '\000';

  return (ret_buf);
}


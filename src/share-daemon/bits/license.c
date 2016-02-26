
/*
 * @copyright
 *
 *  Copyright 2016 Brian Burrell 
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

#include "sharedaemon.h"



int txop_lic_init(shpeer_t *cli_peer, tx_license_t *lic)
{
  return (0);
}

int txop_lic_confirm(shpeer_t *cli_peer, tx_license_t *lic)
{
  shtime_t expire;

  if (!lic)
    return (SHERR_INVAL);

  expire = shcert_sub_expire(&lic->lic_cert);
  if (shtime_after(shtime(), expire))
    return (SHERR_KEYEXPIRED);

  if (shtime_after(shtime(), lic->lic.lic_expire))
    return (SHERR_KEYEXPIRED);


  return (0);
}

int txop_lic_send(shpeer_t *cli_peer, tx_license_t *lic)
{
  return (0);
}

int txop_lic_recv(shpeer_t *cli_peer, tx_license_t *lic)
{
  return (0);
}



/*
 * @copyright
 *
 *  Copyright 2016 Neo Natura
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

void init_subscribe_tx(tx_subscribe_t *sub, 
    shpeer_t *peer, shkey_t *key, int op, int flags)
{

  memset(&sub->sub_tx, 0, sizeof(sub->sub_tx));
  local_transid_generate(TX_SUBSCRIBE, &sub->sub_tx);

  memcpy(&sub->sub_peer, peer, sizeof(sub->sub_peer));
  memcpy(&sub->sub_key, key, sizeof(sub->sub_key));
  sub->sub_op = op;
  sub->sub_flag = flags;

}

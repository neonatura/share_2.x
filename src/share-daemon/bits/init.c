
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
#include <stddef.h>

#define INIT_TX_KEY(_ini) \
  shkey_bin((char *)(_ini), offsetof(struct tx_init_t, ini_stamp));

int prep_init_tx(tx_init_t *ini)
{
  shkey_t *key;

  memcpy(&ini->ini_peer, sharedaemon_peer(), sizeof(ini->ini_peer));
  ini->ini_ver = SHARENET_PROTOCOL_VERSION;
  ini->ini_endian = SHMEM_MAGIC;
  ini->ini_seq++;

  key = INIT_TX_KEY(ini);
  if (!key)
    return (SHERR_NOMEM);

  memset(ini->ini_hash, '\000', sizeof(ini->ini_hash));
  strncpy(ini->ini_hash, shkey_print(key), sizeof(ini->ini_hash));
  shkey_free(&key);

  ini->ini_stamp = shtime();

  return (0);
}

int confirm_init_tx(tx_init_t *ini)
{
  shkey_t *key;
  char hash[MAX_SHARE_HASH_LENGTH];
  int ok;

  key = INIT_TX_KEY(ini);
  if (!key)
    return (SHERR_NOMEM);

  memset(hash, 0, sizeof(hash));
  strncpy(hash, shkey_print(key), sizeof(hash) - 1); 
  ok = (0 == strcmp(hash, ini->ini_hash));
  shkey_free(&key);
  if (!ok)
    return (SHERR_ACCESS);

  return (0);
}

int process_init_tx(shd_t *cli, tx_init_t *ini)
{
  tx_app_t *ent;
  int err;

  err = confirm_init_tx(ini);
  if (err)
    return (err);

  err = prep_init_tx(ini);
  if (err)
    return (err);

fprintf(stderr, "DEBUG: ini_seq %d\n", ini->ini_seq);
  switch (ini->ini_seq) {
    case 1:
    case 2:
      /* public peer notification */
      break;
    case 3:
    case 4:
      /* private peer notification */
      break;
    case 5:
    case 6:
      /* ledger notification */
    case 7:
    case 8:
      /* app notification */
      break;
    case 9:
    case 10:
      /* time sync */
      break;
    default:
      /* term init sequence */
      return (0);
  }

  sched_tx_sink(shpeer_kpriv(&cli->peer), ini, sizeof(tx_init_t));

  return (0);
}


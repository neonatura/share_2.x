
/*
 * @copyright
 *
 *  Copyright 2013, 2014 Neo Natura
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


/** @todo don't need to send around payload after initial proposal. */
void propose_ledger(sh_ledger_t *led, sh_tx_t *payload, size_t size)
{

  generate_transaction_id(&led->tx);
  led->ledger_confirm++;
  led->ledger_height = size;
  save_ledger(led, payload, "pending");

  /* ship 'er off */
  sched_tx_payload(led, sizeof(sh_ledger_t), 
      (char *)(led + sizeof(sh_ledger_t)),
      (led->ledger_height * sizeof(sh_tx_t)));

}

void abandon_ledger(sh_tx_t *tx)
{
}

int confirm_ledger(sh_ledger_t *led, sh_tx_t *payload)
{
  sh_ledger_t *p_led = NULL;
  sh_tx_t *ptx_led = NULL;
  sh_ledger_t *t_led;
  sh_tx_t *ttx_led = NULL;
  int bcast;
  int err;

  if (led->ledger_seq != 0) {
    /* find parent ledger */
    err = load_ledger(led->parent_hash, "confirm", &p_led, &ptx_led);
    if (err)
      return (err);
  }

  if (led->ledger_seq != p_led->ledger_seq + 1)
    return (SHERR_ILSEQ);

  err = load_ledger(led->hash, "confirm", &t_led, &ttx_led);
  if (!err) {
    free_ledger(&p_led, &ptx_led);
    free_ledger(&t_led, &ttx_led);
    return (err); /* this ledger entry has already been stored. */
  }

  bcast = FALSE;

  err = load_ledger(led->hash, "pending", &t_led, &ttx_led);
  if (err) {
    /* new ledger entry. */
    led->ledger_confirm++;
    bcast = TRUE;
    free_ledger(&t_led, &ttx_led);
  } 

  if (led->ledger_confirm >= led->ledger_height) {
    /* broadcast on initial confirmation inform. */
    led->ledger_stamp = shtime(); 
    bcast = TRUE;
    if (led->ledger_stamp == 0) {
      /* this ledger is now confirmed -- inform peers. */
      led->ledger_stamp = shtime(); 
      remove_ledger(led->hash, "pending"); 
    }
    bcast = TRUE;
    save_ledger(led, payload, "confirm");
  } else {
    save_ledger(led, payload, "pending");
  }

  if (bcast) {
    sched_tx_payload(led, sizeof(sh_ledger_t), 
        payload, (led->ledger_height * sizeof(sh_tx_t)));
#if 0
    broadcast_raw(led, sizeof(sh_ledger_t)); 
    broadcast_raw(payload, sizeof(sh_ledger_t) * led->ledger_height); 
#endif
  }

  if (led->ledger_confirm >= led->ledger_height &&
      led->ledger_stamp == 0) {
    led->ledger_stamp = shtime(); 
  }

  return (0);
}



int load_ledger(char *hash, char *type, sh_ledger_t **ledger_p, sh_tx_t **payload_p)
{
  shfs_t *fs = sharedaemon_fs();
  shfs_ino_t *fl;
  char path[PATH_MAX+1];
  char *data;
  size_t data_len;
  int err;

  sprintf(path, "/shnet/ledger/%s/%s", type, hash);
  fl = shfs_file_find(fs, path);
  err = shfs_file_read(fl, &data, &data_len);
  if (err)
    return (err);
  if (data_len != sizeof(sh_ledger_t)) {
    PRINT_ERROR(SHERR_IO, "[load_ledger] invalid file size");
    return (SHERR_IO);
  }
  *ledger_p = (sh_ledger_t *)data;

  sprintf(path, "/shnet/ledger/%s/%s.tx", type, hash);
  fl = shfs_file_find(fs, path);
  err = shfs_file_read(fl, &data, &data_len);
  if (err)
    return (err);
  if (data_len != (sizeof(sh_tx_t) * (*ledger_p)->ledger_height)) {
    PRINT_ERROR(SHERR_IO, "[load_ledger] invalid file size");
    return (SHERR_IO);
  }
  *payload_p = (sh_tx_t *)data;

  return (0);
}


int save_ledger(sh_ledger_t *ledger, sh_tx_t *payload, char *type)
{
  shfs_t *fs = sharedaemon_fs();
  shfs_ino_t *fl;
  char path[PATH_MAX+1];
  int err;

  sprintf(path, "/shnet/ledger/%s/%s", type, ledger->hash);
  fl = shfs_file_find(fs, path);
  err = shfs_file_write(fl, (char *)ledger, sizeof(sh_ledger_t));
  if (err)
    return (err);

  sprintf(path, "/shnet/ledger/%s/%s.tx", type, ledger->hash);
  fl = shfs_file_find(fs, path);
  err = shfs_file_write(fl, (char *)payload,
      (sizeof(sh_tx_t) * ledger->ledger_height));
  if (err)
    return (err);

  return (0);
}

int remove_ledger(sh_ledger_t *ledger, char *type)
{
  shfs_t *fs = sharedaemon_fs();
  shfs_ino_t *fl;
  char path[PATH_MAX+1];
  int err;

  sprintf(path, "/shnet/ledger/%s/%s", type, ledger->hash);
  fl = shfs_file_find(fs, path);
  err = shfs_inode_unlink(fl);
  if (err)
    return (err);

  sprintf(path, "/shnet/ledger/%s/%s.tx", type, ledger->hash);
  fl = shfs_file_find(fs, path);
  err = shfs_inode_unlink(fl);
  if (err)
    return (err);

  return (0);
}

void free_ledger(sh_ledger_t **ledger_p, sh_tx_t **tx_p)
{

  if (tx_p) {
    free(*tx_p);
    *tx_p = NULL;
  }

  if (ledger_p) {
    free(*ledger_p);
    *ledger_p = NULL;
  } 

}


/*
 * @copyright
 *
 *  Copyright 2013 Brian Burrell 
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

int local_broadcast_file(tx_file_t *file)
{
fprintf(stderr, "DEBUG: local_broadcast_file[%s]: header<%d bytes) + data<%d bytes>\n", file->ino_path, sizeof(tx_file_t), file->ino_size);
  sched_tx(file, sizeof(tx_file_t) + file->ino_size);
  return (0);
}

/**
 * Conduct a file operation against connected peers.
 */
int broadcast_file_op(tx_file_t *file, int op, unsigned char *payload, size_t payload_len)
{
  tx_file_t *s_file;
  size_t len;
  int err;

  len = sizeof(tx_file_t) + payload_len;
  s_file = (tx_file_t *)calloc(1, len);
  if (!s_file)
    return (SHERR_NOMEM);

  memcpy(s_file, file, sizeof(tx_file_t));
  if (payload && payload_len)
    memcpy(((char *)s_file) + sizeof(tx_file_t), payload, payload_len); 

  s_file->ino_stamp = shtime();
  s_file->ino_op = op;

  err = local_broadcast_file(s_file);
  if (err)
    return (err);

  free(s_file);

  return (0);
}

int local_confirm_file(tx_file_t *file)
{

  /* ? initial file reference - verify integrity. */

  /* ? indicate outgoing peer reference for network transaction. */

  /* ? add inode's peer to in-memory hashmap. */

  return (0);
}

int remote_confirm_file(tx_app_t *cli, tx_file_t *file)
{

/* ensure local file's create date is equal */

  return (0);
}

/**
 * A file notification received from a client on the local machine.
 *
 * Inform remote hosts of the current file checksum.
 * Listening peers will return a TXFILE_READ, TXFILE_WRITE, or TXFILE_SYNC.
 * A TXFILE_READ indicates new data needs to be sent.
 * A TXFILE_WRITE indicates that local data should be over-written.
 * A TXFILE_SYNC indicates inode synchronization or willing non-compliance.
 * @note The blk argument will never contain data in this context.
 */
int local_file_notification(shpeer_t *peer, shfs_hdr_t *blk)
{
  tx_file_t *file;
  char sig_hash[MAX_SHARE_HASH_LENGTH];
  int err;

  strcpy(sig_hash, shkey_print(&blk->name));
  file = (tx_file_t *)pstore_load(TX_FILE, sig_hash);
  if (!file) {
    file = (tx_file_t *)calloc(1, sizeof(tx_file_t));
    local_transid_generate(TX_FILE, &file->ino_tx);
    memcpy(&file->ino_peer, peer, sizeof(shpeer_t));
    memcpy(&file->ino, blk, sizeof(shfs_hdr_t));

    /* verify integrity and assign/inform network transaction */
    err = local_confirm_file(file);
    if (err)
      return (err);

    pstore_save(file, sizeof(tx_file_t));
  }

  broadcast_file_op(file, TXFILE_CHECKSUM, NULL, 0);

  pstore_free(file);

  return (0);
}

#define SEG_CHECKSUM_SIZE 65536
int local_request_segments(shpeer_t *origin, tx_file_t *tx, shfs_ino_t *file)
{
  shbuf_t *buff;
  char data[SEG_CHECKSUM_SIZE];
  shsize_t max;
  shsize_t len;
  shsize_t of;
  int err;

  /* adjust size. */
  err = shfs_truncate(file, tx->ino.size);
  if (err)
    return (err);

  if (tx->ino.size > shfs_size(file)) {
    /* request missing data suffix */
    tx->ino_op = TXFILE_READ;
    tx->ino_of = shfs_size(file);
    tx->ino_crc = 0L;
    tx->ino_size = (tx->ino.size - shfs_size(file));
    sched_tx_sink(shpeer_kpriv(origin), file, sizeof(tx_file_t));
fprintf(stderr, "DEBUG: local_request_segements[suffix]: sched_tx_sink()\n"); 
  }


  of = 0;
  buff = shbuf_init();
  max = MIN(shfs_size(file), tx->ino.size);
  for (of = 0; of < max; of += SEG_CHECKSUM_SIZE) {
    shbuf_clear(buff);
    len = MIN(SEG_CHECKSUM_SIZE, max - of);
    err = shfs_read_of(file, buff, of, len);
    if (err)
      return (err); /* corruption ensues.. */

    /* request data segment. */
    tx->ino_op = TXFILE_READ;
    tx->ino_of = of;
    tx->ino_crc = shcrc(shbuf_data(buff), len);
    tx->ino_size = len;
    sched_tx_sink(shpeer_kpriv(origin), file, sizeof(tx_file_t));
fprintf(stderr, "DEBUG: local_request_segements[prefix]: sched_tx_sink()\n"); 
  }
  shbuf_free(&buff);

  return (0);
}

/**
 * An incoming TXFILE_CHECKSUM file operation from another server.
 */
int remote_file_notification(shpeer_t *origin, tx_file_t *tx)
{
  shfs_t *fs;
  SHFL *file;
  SHFL *dir;
  double fee;
  int ret_err;
  int err;

  ret_err = 0;

  fs = shfs_init(&tx->ino_peer);
  dir = shfs_dir_find(fs, tx->ino_path);
  file = shfs_inode_load(dir, &tx->ino.name);
  if (!file) {
    /* referenced file does not exist. */
    if (!(shfs_attr(dir) & SHATTR_SYNC)) {
      /* file is only a remote reference. */
      ret_err = SHERR_NOKEY;
      goto done;
    }
  } else {
    int create_t = shfs_ctime(file);

    if (!(shfs_attr(file) & SHATTR_SYNC)) {
      /* file is not marked to be synchronized. */
      ret_err = SHERR_REMOTE;
      goto done;
    }

    if (create_t != tx->ino.ctime) {
      /* remote reference is alternate */
      ret_err = SHERR_TOOMANYREFS;
      goto done;
    }
  }

  if (shfs_crc(file) != tx->ino.crc) {
    /* checksums do not match. */
  
    if (shtime_after(shfs_mtime(file), tx->ino.ctime)) {
      /* local file is newer - send checksum op back */
      memcpy(&tx->ino, &file->blk.hdr, sizeof(shfs_hdr_t));
      tx->ino_op = TXFILE_SYNC;
      sched_tx_sink(shpeer_kpriv(origin), file, sizeof(tx_file_t));
fprintf(stderr, "DEBUG: remote_file_notifiation[local file newer]: sched_tx_sink()\n"); 
      goto done;
    }

    /* local time is after - we need their info */

    fee = CALC_TXFILE_FEE(tx->ino.size, tx->ino.ctime);
    if (!NO_TXFILE_FEE(fee)) {
      /* create a bond to cover net traffic expense */
#if 0
/* DEBUG: */
      bond = load_bond_peer(NULL, origin, shfs_inode_peer(file));
      if (bond && get_bond_state(bond) != TXBOND_CONFIRM) {
        free_bond(&bond);
      }
      if (!bond) {
        /* create a bond to cover net traffic expense */
        bond = create_bond_peer(origin, shfs_inode_peer(file), 0, fee, 0.0);
        /* allow remote to confirm value [or portion of] bond. */
        set_bond_state(bond, TXBOND_CONFIRM);
      }
/* DEBUG: */
#endif
    }

    /* assign payment. */
/* DEBUG: 
    if (!bond) {
      memset(tx->ino_bond, '\000', MAX_SHARE_HASH_LENGTH);
    } else {
      strncpy(tx->ino_bond, bond->bond_tx.tx_hash, MAX_SHARE_HASH_LENGTH);
    }
*/ 

    err = local_request_segments(origin, tx, file);
fprintf(stderr, "DEBUG: %d = local_request_segments()\n", err); 

    //pstore_save(file, sizeof(tx_file_t));
    goto done;
  }

  
  /* notify origin that file is synchronized. */
  tx->ino_op = TXFILE_SYNC;
  tx->ino_of = 0;
  tx->ino_size = 0;
  tx->ino_crc = 0L;
#if 0
/* DEBUG: */
  memset(tx->ino_bond, '\000', MAX_SHARE_HASH_LENGTH);
#endif
  sched_tx_sink(shpeer_kpriv(origin), file, sizeof(tx_file_t));
fprintf(stderr, "DEBUG: remote_file_notification: sched_tx_sink()\n"); 

done:
  return (ret_err);
}

/**
 * Process a TXFILE_READ request from a remote peer.
 */
int remote_file_distribute(shpeer_t *origin, tx_file_t *tx)
{
  SHFL *file;
  SHFL *dir;
  shfs_t *fs;
  tx_file_t *send_tx; 
  tx_bond_t *bond;
  shbuf_t *buff;
  double fee;
  unsigned char *data;
  int err;

  fs = shfs_init(&tx->ino_peer);
  dir = shfs_dir_find(fs, tx->ino_path);
  file = shfs_inode_load(dir, &tx->ino.name);
  if (!file) {
    shfs_free(&fs);
    return (SHERR_NOENT);
  }

  fee = CALC_TXFILE_FEE(tx->ino.size, tx->ino.ctime);
  if (!NO_TXFILE_FEE(fee)) {
#if 0
/* DEBUG: */
    /* verify xfer is appropriated with bond. */
    bond = load_bond_peer(origin, NULL, shfs_peer(file));
    if (!bond) {
      /* no bond has been established for xfer. */
      shfs_free(&fs);
      return (SHERR_CANCELED);
    }
    if (fee > BOND_CREDIT_VALUE(bond->bond_credit)) {
      /* bond is not appropriated enough for xfer. */
      free_bond(&bond);
      shfs_free(&fs);
      return (SHERR_ACCESS);
    }
/* DEBUG: */
#endif
  }


  buff = shbuf_init();
  shbuf_cat(buff, tx, sizeof(tx_file_t));

  send_tx = (tx_file_t *)shbuf_data(buff);
  send_tx->ino_op = TXFILE_WRITE;
  send_tx->ino_crc = shcrc(shbuf_data(buff) + sizeof(tx_file_t), tx->ino_size);

  err = shfs_read_of(file, buff, tx->ino_of, tx->ino_size);
  shfs_free(&fs);
  if (err) {
#if 0
/* DEBUG: */
    free_bond(&bond);
#endif
    return (err);
  }

  sched_tx_sink(shpeer_kpriv(origin), shbuf_data(buff), shbuf_size(buff));
fprintf(stderr, "DEBUG: remote_file_distribute: sched_tx_sink()\n"); 

#if 0
/* DEBUG: */
  confirm_bond_value(bond, fee);
  free_bond(&bond);
#endif

  return (0);
}

int process_file_tx(tx_app_t *cli, tx_file_t *file)
{
  tx_file_t *ent;
  int err;

  ent = (tx_file_t *)pstore_load(TX_FILE, file->ino_tx.hash);
  if (!ent) {
    /* only save if local file is to be over-written */
    err = remote_confirm_file(cli, file);
    if (err)
      return (err);
    pstore_save(file, sizeof(tx_file_t));
  }

fprintf(stderr, "DEBUG: process_file_tx: ino-op(%d) ino-path(%s)\n", file->ino_op, file->ino_path); 
  switch (file->ino_op) {
    case TXFILE_CHECKSUM:
      /* entire file checksum notification. */
      err = remote_file_notification(&cli->app_peer, file);
      if (err)
        return (err);
      break;
    case TXFILE_READ:
      /* read data segment request. */
      err = remote_file_distribute(&cli->app_peer, file);
      if (err)
        return (err);
      break;
  }

  return (0);
}



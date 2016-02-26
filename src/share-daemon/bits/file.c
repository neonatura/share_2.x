
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

static void get_tx_inode(tx_file_t *tx, shfs_t **fs_p, shfs_ino_t **ino_p)
{
  shfs_t *fs;
  shfs_ino_t *inode;

  fs = shfs_init(&tx->ino_peer);
  inode = shfs_file_find(fs, tx->ino_path);
//  inode = shfs_inode_load(dir, &tx->ino_name);

fprintf(stderr, "DEBUG: get_tx_inode: fs inode name '%s'\n", shkey_print(shfs_token(inode)));
fprintf(stderr, "DEBUG: get_Tx_inode: tx inode name '%s'\n", shkey_print(&tx->ino_name));

  *fs_p = fs;
  *ino_p = inode;
}

static void update_tx_inode(tx_file_t *tx, SHFL *inode)
{
  tx->ino_ctime = shfs_ctime(inode);
  tx->ino_crc = shfs_crc(inode);
}

static void set_tx_inode(tx_file_t *tx, SHFL *inode)
{
  shpeer_t *fs_peer = shfs_inode_peer(inode);
  shkey_t *ino_name = shfs_token(inode);
  char *ino_path = shfs_inode_path(inode);

  strncpy(tx->ino_path, ino_path, SHFS_PATH_MAX - 1);
  memcpy(&tx->ino_peer, fs_peer, sizeof(shpeer_t));
  memcpy(&tx->ino_name, ino_name, sizeof(shkey_t));

  update_tx_inode(tx, inode);
}

static tx_file_t *prep_tx_file(tx_file_t *tx_file, int op_type, shfs_ino_t *inode, size_t data_of, size_t data_len)
{
  tx_fileseg_t seg;
  tx_file_t *file;
  shbuf_t *f_buff;
  shbuf_t *buff;
  size_t of;
  int err;

  buff = shbuf_init();
  shbuf_cat(buff, tx_file, sizeof(tx_file_t));

  file = (tx_file_t *)shbuf_data(buff);
  set_tx_inode(file, inode);
  file->ino_op = op_type;

  if (op_type == TXFILE_READ) {
  } else if (op_type == TXFILE_WRITE) {
    f_buff = shbuf_init();
    shfs_read_of(inode, f_buff, data_of, data_len);
    file->seg_len = MIN(data_len, shbuf_size(f_buff));
    file->seg_crc = shcrc(shbuf_data(f_buff), file->seg_len);
    shbuf_cat(f_buff, buff, file->seg_len);
    shbuf_free(&f_buff);
  }

  return ((tx_file_t *)shbuf_unmap(buff));
}




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
int local_file_notification(shpeer_t *peer, char *path)
{
  shfs_t *fs;
  shfs_ino_t *inode;
  tx_file_t *file;
  tx_file_t *send_tx;
  shbuf_t *buff;
  char sig_hash[MAX_SHARE_HASH_LENGTH];
  int err;


  fs = shfs_init(peer);
  inode = shfs_file_find(fs, path);

  strcpy(sig_hash, shkey_print(shfs_token(inode)));
  file = (tx_file_t *)pstore_load(TX_FILE, sig_hash);
  if (!file) {
    file = (tx_file_t *)calloc(1, sizeof(tx_file_t));
    if (!file)
      return (SHERR_NOMEM);

    set_tx_inode(file, inode);
    err = tx_init(NULL, (tx_t *)file, TX_FILE);
    if (err)
      return (err);

    pstore_save(file, sizeof(tx_file_t));
  } else {
    /* update inode state */
    update_tx_inode(file, inode);
  }

  send_tx = prep_tx_file(file, TXFILE_CHECKSUM, inode, 0, 0);
  if (send_tx) {
    /* broadcast checksum of synchronized file. */
    tx_send(NULL, (tx_t *)send_tx);
    free(send_tx);
  }

  pstore_free(file);

  return (0);
}

#define SEG_CHECKSUM_SIZE 65536
int local_request_segments(shpeer_t *origin, tx_file_t *tx, SHFL *file)
{
#if 0
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

  if (tx->ino.size > shfs_size(inode)) {
    /* request missing data suffix */
    tx->ino_op = TXFILE_READ;
    tx->ino_of = shfs_size(inode);
    tx->ino_crc = 0L;
    tx->ino_size = (tx->ino.size - shfs_size(inode));
    sched_tx_sink(shpeer_kpriv(origin), inode, sizeof(tx_file_t));
fprintf(stderr, "DEBUG: local_request_segements[suffix]: sched_tx_sink()\n"); 
  }


  of = 0;
  buff = shbuf_init();
  max = MIN(shfs_size(inode), tx->ino.size);
  for (of = 0; of < max; of += SEG_CHECKSUM_SIZE) {
    shbuf_clear(buff);
    len = MIN(SEG_CHECKSUM_SIZE, max - of);
    err = shfs_read_of(inode, buff, of, len);
    if (err)
      return (err); /* corruption ensues.. */

    /* request data segment. */
    tx->ino_op = TXFILE_READ;
    tx->ino_of = of;
    tx->ino_crc = shcrc(shbuf_data(buff), len);
    tx->ino_size = len;
    sched_tx_sink(shpeer_kpriv(origin), inode, sizeof(tx_file_t));
fprintf(stderr, "DEBUG: local_request_segements[prefix]: sched_tx_sink()\n"); 
  }
  shbuf_free(&buff);

#endif
  return (0);
}

/**
 * An incoming TXFILE_CHECKSUM file operation from another server.
 */
int remote_file_notification(shpeer_t *origin, tx_file_t *tx)
{
  struct stat st;
  tx_file_t *send_tx;
  shfs_t *fs;
  SHFL *inode;
  double fee;
  int ret_err;
  int err;

  ret_err = 0;

  
  get_tx_inode(tx, &fs, &inode);
  if (0 != shfs_fstat(inode, &st)) {
    /* referenced file does not exist. */
    if (!(shfs_attr(shfs_inode_parent(inode)) & SHATTR_SYNC)) {
      /* file is only a remote reference. */
      ret_err = SHERR_NOKEY;
      goto done;
    }
  } else {
    int create_t = shfs_ctime(inode);

    if (!(shfs_attr(inode) & SHATTR_SYNC)) {
      /* file is not marked to be synchronized. */
      ret_err = SHERR_REMOTE;
      goto done;
    }

    if (create_t != tx->ino_ctime) {
      /* remote reference is alternate */
      ret_err = SHERR_TOOMANYREFS;
      goto done;
    }
  }

  if (shfs_crc(inode) != tx->ino_crc) {
    /* checksums do not match. */
  
    if (shtime_after(shfs_mtime(inode), tx->ino_ctime)) {
      /* local file is newer */
      send_tx = prep_tx_file(tx, TXFILE_SYNC, inode, 0, 0); 
      if (send_tx) {
        tx_send(origin, (tx_t *)send_tx);
        free(send_tx);
      }

fprintf(stderr, "DEBUG: remote_file_notifiation[local file newer]: sched_tx_sink()\n"); 
      goto done;
    }

    /* local time is after - we need their info */

    fee = CALC_TXFILE_FEE(shfs_size(inode), shfs_ctime(inode));
    if (!NO_TXFILE_FEE(fee)) {
      /* create a bond to cover net traffic expense */
#if 0
/* DEBUG: */
      bond = load_bond_peer(NULL, origin, shfs_inode_peer(inode));
      if (bond && get_bond_state(bond) != TXBOND_CONFIRM) {
        free_bond(&bond);
      }
      if (!bond) {
        /* create a bond to cover net traffic expense */
        bond = create_bond_peer(origin, shfs_inode_peer(inode), 0, fee, 0.0);
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

    err = local_request_segments(origin, tx, inode);
fprintf(stderr, "DEBUG: %d = local_request_segments()\n", err); 

    //pstore_save(file, sizeof(tx_file_t));
    goto done;
  }
  
  /* notify origin that file is synchronized. */
  send_tx = prep_tx_file(tx, TXFILE_SYNC, inode, 0, 0);
  if (send_tx) {
    tx_send(origin, send_tx);
    free(send_tx);
  }
fprintf(stderr, "DEBUG: remote_file_notification: sched_tx_sink()\n"); 

done:
  return (ret_err);
}

/**
 * Process a TXFILE_READ request from a remote peer.
 */
int remote_file_distribute(shpeer_t *origin, tx_file_t *tx)
{
  struct stat st;
  SHFL *inode;
  shfs_t *fs;
  tx_file_t *send_tx; 
  tx_bond_t *bond;
  shbuf_t *buff;
  double fee;
  unsigned char *data;
  int err;

  get_tx_inode(tx, &fs, &inode);
  err = shfs_fstat(inode, &st);
  if (err)
    return (err);

  fee = CALC_TXFILE_FEE(shfs_size(inode), shfs_ctime(inode));
  if (!NO_TXFILE_FEE(fee)) {
#if 0
/* DEBUG: */
    /* verify xfer is appropriated with bond. */
    bond = load_bond_peer(origin, NULL, shfs_peer(inode));
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

  send_tx = prep_tx_file(tx, TXFILE_WRITE, inode, tx->seg_of, tx->seg_len); 
  if (send_tx) {
    tx_send(origin, send_tx);
    free(send_tx);
  }

#if 0
  confirm_bond_value(bond, fee);
  free_bond(&bond);
#endif

  return (0);
}

#if 0 
int remote_confirm_file(tx_app_t *cli, tx_file_t *file)
{

/* ensure local file's create date is equal */

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
#endif

int txop_file_recv(shpeer_t *cli, tx_file_t *file)
{
  int err;

  switch (file->ino_op) {
    case TXFILE_CHECKSUM:
      /* entire file checksum notification. */
      err = remote_file_notification(cli, file);
      if (err)
        return (err);
      break;
    case TXFILE_READ:
      /* read data segment request. */
      err = remote_file_distribute(cli, file);
      if (err)
        return (err);
      break;
    case TXFILE_WRITE:
      break;
    case TXFILE_SYNC:
      break;
  }

  return (0);
}

int txop_file_send(shpeer_t *cli, tx_file_t *file)
{
#if 0
  sched_tx_sink(shpeer_kpriv(cli), file, 
      sizeof(tx_file_t) + file->ino_size);
#endif
  return (0);
}

int txop_file_confirm(shpeer_t *cli, tx_file_t *file)
{
  /* ensure local file's create date is equal */
  /* ? initial file reference - verify integrity. */

  /* ? indicate outgoing peer reference for network transaction. */

  /* ? add inode's peer to in-memory hashmap. */

  return (0);
}


int txop_file_init(shpeer_t *cli, tx_file_t *file)
{
  struct stat st;
  shbuf_t *buff;
  shfs_t *fs;
  shfs_ino_t *inode;
  int err;

  get_tx_inode(file, &fs, &inode);
  err = shfs_fstat(inode, &st);
  if (err)
    return (err);

  buff = shbuf_init();
  shbuf_cat(buff, file, sizeof(tx_file_t));
  prep_tx_file(buff, TXFILE_CHECKSUM, inode, 0, 0);
  memcpy(file, shbuf_data(buff), sizeof(tx_file_t));
  shbuf_free(&buff);

  shfs_free(&fs);
  return (0);
}



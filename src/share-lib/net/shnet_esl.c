
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



static shkey_t *esl_key(int sk)
{
  unsigned int usk;

	usk = (unsigned short)sk;
  if (usk >= USHORT_MAX)
    return (NULL);

  return (&_sk_table[usk].key);
}

static void esl_key_merge(int sk, shkey_t *m_key)
{
  shkey_t *e_key;
  unsigned int usk;

  if (!m_key)
    return;

	usk = (unsigned short)sk;
  if (usk >= USHORT_MAX)
    return;

  e_key = shkey_xor(m_key, esl_key(sk));
  memcpy(esl_key(sk), e_key, sizeof(shkey_t));
  shkey_free(&e_key);
}


int esl_control(int sk, int mode, shkey_t *key) 
{
  esl_t sec;
  unsigned int usk;
  int err;

	usk = (unsigned short)sk;
  if (usk >= USHORT_MAX)
    return (SHERR_BADF);

  memset(&sec, 0, sizeof(esl_t));
  sec.s_magic = SHMEM16_MAGIC;
  sec.s_ver = htons(SHNET_ENCRYPT_PROTO_VERSION);
  sec.s_mode = htons(mode);
  memcpy(&sec.s_key, key, sizeof(sec.s_key));

  /* append control to outgoing buffer */
  shnet_write_buf(sk, (unsigned char *)&sec, sizeof(sec));

}

/**
 * Initiate a secure connection.
 */
int esl_connect(char *hostname, int port, shkey_t *eslkey)
{
  shpeer_t *peer;
  int sk;
  int err;

  sk = shconnect_host(hostname, port, SHNET_ASYNC);
  if (sk < 0)
    return (sk);

  peer = NULL;
  if (!eslkey) {
    peer = shpeer_init("esl", hostname);
    eslkey = shapp_kpub(peer);
  }

  err = esl_control(sk, ESL_INIT_CERT, eslkey);
  if (err) {
    shpeer_free(&peer);
    return (err);
}

  memcpy(esl_key(sk), eslkey, sizeof(shkey_t));

    shpeer_free(&peer);

  return (sk);
}

int esl_write_data(int sk, unsigned char *data, size_t *data_len_p)
{
  esl_data_t hdr;
  unsigned char *raw_data;
  shkey_t *key;
  size_t data_len = *data_len_p;
  ssize_t w_len;
  size_t b_len;
  size_t b_of;
  size_t raw_data_len;
  unsigned int usk = (unsigned int)sk;
  int err;

  if (usk >= USHORT_MAX)
    return (SHERR_BADF);

  w_len = 0;
  key = esl_key(sk);
  for (b_of = 0; b_of < data_len; b_of += b_len) {
    b_len = MIN(8192, (data_len - b_of));


    /* encode using encrypt key */
    err = shencode(data + b_of, b_len, &raw_data, &raw_data_len, key);
    if (err)
      return (err);

    /* esl protocol data header */
    memset(&hdr, 0, sizeof(hdr));
    hdr.s_magic = SHMEM16_MAGIC;
    hdr.s_crc = ESL_CHECKSUM(data + b_of, b_len); /* crc of decoded data */ 
    hdr.s_size = htons(raw_data_len); /* size of encoded data */

    shnet_write_buf(sk, (unsigned char *)&hdr, sizeof(hdr));
    shnet_write_buf(sk, raw_data, raw_data_len);

    /* deallocate resources */
    free(raw_data);
  }

  *data_len_p = b_of;
  return (0);
}

int esl_writeb(int sk, shbuf_t *wbuff)
{
  ssize_t len;

  len = esl_write(sk, shbuf_data(wbuff), shbuf_size(wbuff));
  if (len < 0)
    return (len);

  return (0);
}

ssize_t esl_write(int sk, const void *data, size_t data_len)
{
  unsigned int usk = (unsigned int)sk;
  int err;

  if (!(_sk_table[usk].flags & SHNET_CRYPT)) {
    esl_readb(sk, NULL);
  }
  if (!(_sk_table[usk].flags & SHNET_CRYPT)) {
    /* not initialized for ESL */
    return (SHERR_AGAIN);
  }

  err = esl_write_data(sk, (unsigned char *)data, &data_len);
  if (err)
    return ((ssize_t)err);

  err = shnet_write_flush(sk);
  if (err)
    return (err);

  return ((ssize_t)data_len);
}


static int esl_read_ctrl(int sk, shbuf_t *rbuff)
{
  shkey_t *key;
  unsigned int usk;
  esl_t hdr;
  int err;

	usk = (unsigned short)sk;
  if (usk >= USHORT_MAX)
    return (SHERR_BADF);

  if (shbuf_size(rbuff) < sizeof(esl_t))
    return (SHERR_AGAIN);

  memcpy(&hdr, shbuf_data(rbuff), sizeof(esl_t));
  hdr.s_mode = ntohs(hdr.s_mode);

  if (hdr.s_mode == ESL_INIT_CERT) {
    /* this socket initiated the connection */
    shkey_t *p_key = &_sk_table[usk].key;
    shkey_t *c_key;
    if (!shkey_cmp(p_key, ashkey_blank()) &&
        !shkey_cmp(p_key, &hdr.s_key)) {
      /* client did not send pre-defined key for listen socket. */
      shnet_close(sk);
      return (SHERR_ACCESS);
    }

    /* merge orig key sent with received key */
    c_key = shkey_bin((char *)&_sk_table[usk].addr_dst,
        sizeof(struct sockaddr));
    memcpy(p_key, c_key, sizeof(shkey_t));
    shkey_free(&c_key);
  }

  if (hdr.s_mode == ESL_INIT_CERT ||
      hdr.s_mode == ESL_INIT_PRIV) {
    /* receiver of public handshake. */
    esl_key_merge(sk, &hdr.s_key);
    _sk_table[usk].flags |= SHNET_CRYPT;
  } 

  shbuf_trim(rbuff, sizeof(esl_t));
  return (0);
}


static int esl_read_data(int sk, shbuf_t *rbuff, shbuf_t *pbuff)
{
  esl_data_t hdr;
  char *raw_data;
  unsigned int usk;
  size_t raw_data_len;
  int err;

	usk = (unsigned short)sk;
  if (usk >= USHORT_MAX)
    return (SHERR_BADF);

  if (shbuf_size(rbuff) < sizeof(esl_data_t)) {
    return (SHERR_AGAIN);
  }

  memcpy(&hdr, shbuf_data(rbuff), sizeof(hdr));
  hdr.s_size = ntohs(hdr.s_size);

  if (shbuf_size(rbuff) < (hdr.s_size + sizeof(hdr))) {
    return (SHERR_AGAIN);
  }

  shbuf_trim(rbuff, sizeof(hdr));

  if (hdr.s_size == 0) {
return (SHERR_AGAIN);
  }

  raw_data = NULL;
  raw_data_len = 0;
  err = shdecode(shbuf_data(rbuff), (size_t)hdr.s_size,
    &raw_data, &raw_data_len, esl_key(sk));
  if (err)
    return (err);

  /* verify checksum */
  if (hdr.s_crc != ESL_CHECKSUM(raw_data, raw_data_len)) {
    free(raw_data);
    return (SHERR_ILSEQ);
  }

  /* clear incoming encoded segment. */
  shbuf_trim(rbuff, (size_t)hdr.s_size);
  
  /* append to processed data buffer */
  shbuf_cat(pbuff, raw_data, raw_data_len);
  free(raw_data);

  return (0); 
}

int esl_readb(int sk, shbuf_t *in_buff)
{
  ssize_t b_len;
  shbuf_t *rbuff;
  unsigned char *rdata;
  uint16_t *hdr;
  unsigned int usk;
  int _data_read;
  int mode;
  int err;

  usk = (unsigned int)sk;
  if (usk >= USHORT_MAX)
    return (SHERR_BADF);

  if (in_buff != _sk_table[usk].proc_buff &&
      shbuf_size(_sk_table[usk].proc_buff) != 0) {
    /* pending data from esl_read() call */
    shbuf_append(_sk_table[usk].proc_buff, in_buff);
    shbuf_clear(_sk_table[usk].proc_buff);
  }

  _data_read = FALSE;
  while (1) {
    rbuff = shnet_read_buf(sk);
    if (!rbuff) {
      if (_data_read)
        break;
      return (SHERR_CONNRESET);
    }

    if (shbuf_size(rbuff) < 4)
      break;

    hdr = (uint16_t *)shbuf_data(rbuff);
    mode = (int)ntohs(hdr[1]);

    if (hdr[0] != SHMEM16_MAGIC) {
      shclose(sk);
      return (SHERR_ILSEQ);
    }

    err = 0;
    if (mode == ESL_DATA) {
      err = esl_read_data(sk, rbuff, in_buff);
      if (!err)
        _data_read = TRUE;
    } else {
      err = esl_read_ctrl(sk, rbuff);
    }
    if (err && err != SHERR_AGAIN) {
      PRINT_ERROR(err, "esl_readb"); 
      /* critical error in protocol */
      rbuff = shnet_read_buf(sk);
      if (rbuff)
        shbuf_clear(rbuff);
      shclose(sk);
      return (err);
    }
  }
   
  return (0);
}
ssize_t esl_read(int sk, const void *data, size_t data_len)
{
  shbuf_t *in_buff;
  unsigned short usk;
  ssize_t len;
  int err;

	usk = (unsigned short)sk;
  if (usk >= USHORT_MAX)
    return (SHERR_BADF);

  if (!_sk_table[usk].proc_buff)
    _sk_table[usk].proc_buff = shbuf_init();

  in_buff = _sk_table[usk].proc_buff;
  err = esl_readb(sk, in_buff);
  if (err)
    return (err);

  len = MIN(data_len, shbuf_size(in_buff));
  memcpy((void *)data, shbuf_data(in_buff), len);
  shbuf_trim(in_buff, len);

  return (len);
}

int esl_bind(int port)
{
  int err;
  int sk;

  sk = shnet_sk();
  if (sk < 0)
    return (sk);

  err = shnet_bindsk(sk, NULL, port); 
  if (err < 0) {
    close(sk);
    return (err);
  }

  return (sk);
}

int esl_accept(int sk)
{
  shkey_t *key;
  unsigned short usk;
  int l_sk;
  int err;

  l_sk = shnet_accept(sk);
  if (l_sk < 0)
    return (-errno);

  shnet_fcntl(l_sk, F_SETFL, O_NONBLOCK);

	usk = (unsigned short)l_sk;
  if (usk >= USHORT_MAX)
    return (SHERR_IO);

   memcpy(esl_key(l_sk), esl_key(sk), sizeof(shkey_t));

  /* send priveleged handshake. */
  key = shkey_bin((char *)&_sk_table[usk].addr_dst, sizeof(struct sockaddr));
  err = esl_control(l_sk, ESL_INIT_PRIV, key); 
  shkey_free(&key);
  if (err)
    return (err);

  return (l_sk);
}

void esl_key_set(int sk, shkey_t *key)
{
  unsigned short usk;

	usk = (unsigned short)sk;
  if (usk >= USHORT_MAX)
    return;

  if (_sk_table[usk].flags & SHNET_LISTEN)
    return;

  memcpy(&_sk_table[usk].key, key, sizeof(shkey_t)); 
}


int esl_verify(int sk)
{
  unsigned int usk = (unsigned int)sk;
  fd_set write_set;
  fd_set read_set;
  long ms;
  int err;

  if (usk >= USHORT_MAX)
    return (SHERR_BADF);

  if (!(_sk_table[usk].flags & SHNET_CRYPT))
    esl_readb(sk, NULL);

  ms = 100;
  FD_ZERO(&read_set);
  FD_ZERO(&write_set);
  FD_SET(sk, &read_set);
  FD_SET(sk, &write_set);
  err = shnet_verify(&read_set, &write_set, &ms);

  if (err >= 0 && /* not an error */
      !FD_ISSET(sk, &read_set) && /* socket not marked */
      (shbuf_size(_sk_table[usk].recv_buff) != 0 ||
       shbuf_size(_sk_table[usk].proc_buff) != 0)) { /* pending incoming data */
    err++;
    FD_SET(sk, &read_set);
  }

  return (err);
}

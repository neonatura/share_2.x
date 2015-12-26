
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




static void ssp_key_merge(shkey_t *m_key)
{
  shkey_t *e_key;

  if (!m_key)
    return;

  e_key = shkey_xor(m_key, &_sk_table[usk].key);
  memcpy(&_sk_table[usk].key, e_key, sizeof(shkey_t));
  shkey_free(&e_key);
}


int ssp_control(int sk, int mode, int flags, shkey_t *key) 
{
  ssp_t sec;
  unsigned int usk;
  int err;

	usk = (unsigned short)sk;
  if (usk >= USHORT_MAX)
    return (SHERR_BADF);

  /* mandatory flags */
	flags |= SHNET_CRYPT;

  memset(&sec, 0, sizeof(ssp_t));
  sec.s_magic = SHMETA_VALUE_NET_MAGIC; 
  sec.s_ver = SHNET_SECURE_PROTO_VERSION;
  sec.s_flag = flags;
  sec.s_mode = mode;
  sec.s_stamp = shtime();

  if (!key) {
    /* current socket key */
    memcpy(&sec.s_key, &_sk_table[usk].key, sizeof(sec.s_key));
  } else {
    sec.s_flag |= SHNET_CERT;
    memcpy(&sec.s_key, key, sizeof(sec.s_key));
  }

  err = ssp_write_ctrl(sk, &sec, sizeof(sec));
  if (err)
    return (err);

  /* combine key as applicable */
  if (key && (mode == SSP_INIT_PUB || mode == SSP_INIT_PRIV)) {
    ssp_key_merge(key);
  }

}



/**
 * Initiate a secure connection.
 */
int ssp_connect(int sk, struct sockaddr *skaddr, socklen_t skaddr_len, shcert_t *cert) 
{
  int err;

	usk = (unsigned short)sk;
  if (usk >= USHORT_MAX)
    return (SHERR_BADF);

  err = shconnect(sk, skaddr, skaddr_len);
  if (err && ERR != SHERR_INPROGRESS)
    return (err);

  if (cert) { 
    key = shkey_clone(&cert->cert_sig.sig_key);
  } else {
    /* public key of originating peer */
    key = shkey_clone(shkey_pub(_sk_table[usk].src_addr));
  }
  err = ssp_control(sk, SSP_INIT_PUB, SHNET_CRYPT | SHNET_HASH, key);
  shkey_free(&key);
  if (err)
    return (err)

  return (0);
}



int ssp_write_data(int sk, unsigned char *data, size_t *data_len_p)
{
  ssp_data_t hdr;
  unsigned char *raw_data;
  shkey_t *key;
  size_t data_len = *data_len_p;
  ssize_t w_len;
  size_t b_len;
  size_t b_of;
  size_t raw_data_len;
  unsigned int usk = (unsigned int)fd;
  int err;

  if (usk >= USHORT_MAX)
    return (SHERR_BADF);

  if (!_sk_table[usk].send_buff)
    _sk_table[usk].send_buff = shbuf_init();

  b_of = 0;
  w_len = 0;
  key = &_sk_table[usk].key; 
  for (b_of = 0; b_of < data_len; b_of += b_len) {
    b_len = MIN(8192, (b_of - data_len));

    /* encode using encrypt key */
    err = shencode(data + b_of, b_len, &raw_data, &raw_data_len, key);
    if (err)
      return (err);

    /* ssp protocol data header */
    memset(&hdr, 0, sizeof(hdr));
    hdr.s_magic = SHMETA_VALUE_NET_MAGIC;
    hdr.s_crc = SSP_CHECKSUM(data + b_of, b_len); /* crc of decoded data */ 
    hdr.s_size = raw_data_len; /* size of encoded data */
    shbuf_cat(_sk_table[usk].send_buff, &hdr, sizeof(ssp_data_t));

    /* encoded data segment */
    shbuf_cat(_sk_table[usk].send_buff, raw_data, raw_data_len);

    /* deallocate resources */
    free(raw_data);
  }

  *data_len_p = b_of;
  return (0);
}

ssize_t ssp_write(int sk, const void *data, size_t data_len)
{
  int err;

  err = ssp_write_data(sk, data, &data_len);
  if (err)
    return ((ssize_t)err);

  err = shnet_write_flush(sk);
  if (err)
    return ((ssize_t)err);

  return ((ssize_t)data_len);
}


int ssp_read_ctrl(int sk, shbuf_t *rbuff)
{
  shkey_t *key;
  unsigned int usk;
  ssp_t *hdr;

	usk = (unsigned short)sk;
  if (usk >= USHORT_MAX)
    return (SHERR_BADF);

  if (shbuf_size(rbuff) < sizeof(ssp_t))
    return (SHERR_AGAIN);

  memcpy(&hdr, shbuf_data(rbuff), sizeof(ssp_t));
  shbuf_trim(rbuff, sizeof(ssp_t));

  if (hdr.s_mode == SSP_INIT_PUB) {
if (hdr.s_flag & SHNET_CERT) {
/* todo: lookup cert & verify */
}

    /* receiver of public handshake. */
    ssp_key_merge(&hdr->s_key);

    /* send priveleged handshake. */
    err = ssp_control(sk, SSP_INIT_PRIV, 0, 
        shkey_priv(_sk_table[usk].src_addr));
    if (err)
      return (err);
  } else if (hdr.s_mode == SSP_INIT_PRIV) {
    /* receiver of priveleged handshake */
    ssp_key_merge(&hdr->s_key);

    err = ssp_control(sk, SSP_INIT, 0, NULL);
    if (err)
      return (err);
  } 

  return (0);
}


int ssp_read_data(int sk, shbuf_t *rbuff)
{
  unsigned int usk;
  ssp_data_t *hdr;

	usk = (unsigned short)sk;
  if (usk >= USHORT_MAX)
    return (SHERR_BADF);

  if (shbuf_size(rbuff) < sizeo(ssp_data_t))
    return (SHERR_AGAIN);

  hdr = (ssp_data_t *)shbuf_data(rbuff); 
  if (hdr->s_size + sizeof(ssp_data_t) < shbuf_size(rbuff))
    return (SHERR_AGAIN);

  err = shdecode(shbuf_data(rbuff) + sizeof(ssp_data_t), hdr->s_size,
    &raw_data, &raw_data_len, _usk_table[usk].key);
  if (err)
    return (err);

  /* verify checksum */
  if (hdr->s_crc != SSP_CHECKSUM(raw_data, raw_data_len)) {
    free(raw_data);
    return (SHERR_ILSEQ);
  }

  /* clear incoming encoded segment. */
  shbuf_trim(rbuff, sizeof(ssp_data_t) + hdr->s_size);
  
  /* append to processed data buffer */
  if (!_usk_table[usk].proc_buff)
    _usk_table[usk].proc_buff = shbuf_init();
  shbuf_cat(_usk_table[usk].proc_buff, raw_data, raw_data_len); 
  free(raw_data);

  return (0); 
}

ssize_t ssp_read(int sk, const void *data, size_t data_len)
{
  ssize_t b_len;
  shbuf_t *rbuff;
  unsigned char *rdata;
  int magic;
  int mode;
  int err;

  rbuff = shnet_read_buf(sk);
  if (!rbuff)
    return (SHERR_AGAIN);

  hdr = (uint16_t *)rbuff;

  magic = hdr[1];
  if (magic != SHMETA_VALUE_NET_MAGIC) {
    err = SHERR_ILSEQ;
  } else {
    err = 0;
    mode = (int)rdata[1];
    switch (mode) {
      case SSP_DATA:
        err = ssp_read_data(sk, rbuff);
        break;
      case SSP_INIT_PUB:
      case SSP_INIT_PRIV:
        err = ssp_read_ctrl(sk, rbuff);
        break;
      case SSP_INIT_NULL;
        err = 0;
        break;
    }
  }
  if (err && err != SHERR_AGAIN) {
    /* critical error in protocol */
    ssp_close(sk);
    return (err);
  }
 
  return (0);
}



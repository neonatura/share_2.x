
/*
 * @copyright
 *
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
 *
 *  @endcopyright
 */  

#include "sharedaemon.h"

#define SHARE_CARD_POLL_TIME 10

shdev_t *card_usb_open(int vid, int pid)
{
  shdev_t *c_dev;
  struct libusb_device *dev = NULL;
  struct libusb_config_descriptor *conf_desc;
  libusb_device_handle *hndl = NULL;
  char ebuf[1024];
  char buf[4096];
  char rbuf[1024];
  uint16_t max_len;
  unsigned int r_len;
  unsigned int len;
  int nb_ifaces;
  int val;
  int code;
  int iface;
  int err;
  int i;

  hndl = libusb_open_device_with_vid_pid(NULL, vid, pid);
  if (!hndl)
    return (NULL);

#ifdef __linux__
  /* detach kernel from device. */
  libusb_detach_kernel_driver(hndl, 0);
#endif

  /* Claim the device if it lists one or more interfaces available. */
  dev = libusb_get_device(hndl);
  libusb_get_config_descriptor(dev, 0, &conf_desc);
  nb_ifaces = conf_desc->bNumInterfaces;
  if (nb_ifaces) {
    for (iface = 0; iface < nb_ifaces; iface++) {
      err = libusb_claim_interface(hndl, iface);
      if (err != LIBUSB_SUCCESS)
        continue; /* in use */

      sprintf(ebuf, "claimed usb v%d:p%d (iface #%d).", vid, pid, iface);
      shinfo(ebuf);
fprintf(stderr, "DEBUG: %s\n", ebuf);
      break;
    }
    if (iface == nb_ifaces)
      return (NULL);
  }

  c_dev = (shdev_t *)calloc(1, sizeof(shdev_t));
  if (!c_dev) return (NULL);
  c_dev->type = SHDEV_CARD;
  c_dev->handle = hndl;
  c_dev->iface = iface;
  c_dev->buff = shbuf_init();

  return (c_dev);
}


/**
 * Writes up to 337 bytes of raw interrupt data.
 * @param The buffer to write the incoming data stream.
 */
int card_usb_read_io(shdev_t *c_dev)
{
  char rbuf[64];
  char buf[64];
  unsigned int r_len;
  int code;
  int err;

  r_len = 0;
  memset(rbuf, 0, sizeof(rbuf));
  err = libusb_interrupt_transfer(c_dev->handle, 
      (1 | LIBUSB_ENDPOINT_IN), rbuf, 8, &r_len, SHARE_CARD_POLL_TIME);
  if (err != LIBUSB_SUCCESS) {
    if (err == -7) 
      return (SHERR_AGAIN);
    fprintf(stderr, "DEBUG: libusb_interrupt_transfer: err %d (%s)\n", err, libusb_error_name(err));
    return (SHERR_IO);
  }
  if (r_len != 8) {
    fprintf(stderr, "DEBUG: r_len == 0, breaking..\n");
    return (SHERR_IO);
  }

  code = card_kmap_convert(rbuf);
  if (code == 0) {
    return (SHERR_AGAIN);
}

  buf[0] = code;
  shbuf_cat(c_dev->buff, buf, 1);

  if (code == '\n')
    fprintf(stderr, "INPUT: \"%s\"\n", shbuf_data(c_dev->buff));

  return (0);
}

int card_usb_fill_lic(shcard_t *card, char *data)
{
  return (SHERR_INVAL);
}

int card_usb_fill_fin(shcard_t *card, char *data)
{
  shkey_t *key;
  struct tm *cur;
  struct tm t;
  time_t now;
  char id_str[256]; /* max 19b */ 
  char id_name[256]; /* max 26b */
  char y_str[32];
  char m_str[32];
  uint64_t id_num;
  int idx;

  now = time(NULL);
  cur = gmtime(&now);

  /* parse id number */
  idx = stridx(data, '^');
  if (idx == -1) return (SHERR_INVAL);
  memset(id_str, 0, sizeof(id_str));
  strncpy(id_str, data, idx);
  id_num = atoll(id_str);
  data += (idx + 1);

  /* parse name info */
  idx = stridx(data, '^');
  if (idx == -1) return (SHERR_INVAL);
  memset(id_name, 0, sizeof(id_name));
  strncpy(id_name, data, idx);
fprintf(stderr, "DEBUG: CARD NAME '%s'\n", id_name);

  /* parse expiration */
  y_str[0] = data[idx+1];
  y_str[1] = data[idx+2];
  y_str[2] = '\0';
  m_str[0] = data[idx+3];
  m_str[1] = data[idx+4];
  m_str[2] = '\0';
  memset(&t, 0, sizeof(t));
  t.tm_year = atoi(y_str) + 100;
  t.tm_mon = atoi(m_str) - 1;
  t.tm_mday = 1;
  t.tm_isdst = -1;
fprintf(stderr, "DEBUG: t_year %d, t_mon %d\n", t.tm_year, t.tm_mon);
  if (t.tm_year < cur->tm_year ||
      (t.tm_year == cur->tm_year && t.tm_mon <= cur->tm_mon)) {
time_t tim = mktime(&t); fprintf(stderr, "DEBUG: CARD EXPIRED '%19.19s'\n", ctime(&tim)+4);
    return (SHERR_KEYEXPIRED);
  }
{
time_t tim = mktime(&t);
fprintf(stderr, "DEBUG: CARD EXPIRES @ '%20.20s'\n", ctime(&tim)+4);
}

  /* fill card credentials. */
  key = shkey_bin(&id_num, sizeof(id_num));
  memcpy(&card->card_id, key, sizeof(shkey_t)); 
  shkey_free(&key);
  card->card_acc = shpam_uid(id_name);
  card->card_expire = shmktime(&t);
fprintf(stderr, "DEBUG: CARD ID '%s' KEY '%s' EXPIRE '%s'\n", id_str, shkey_hex(&card->card_id), shctime(card->card_expire));

  return (0);
}

int card_usb_fill(shcard_t *card, char *data)
{

  if (strlen(data) < 17)
    return (SHERR_INVAL);

  if (data[0] == '%') {
    if (isalpha(data[1]) && isalpha(data[2])) {
      return (card_usb_fill_lic(card, data + 1));
    } 
    if (data[1] == 'B') {
      return (card_usb_fill_fin(card, data + 2));
    }
  } 

  return (SHERR_INVAL);
}

int card_usb_read(shdev_t *c_dev)
{
  char *data;
  char buf[1024];
  int idx;
  int err;

  err = card_usb_read_io(c_dev);
  if (err) {
if (err != SHERR_AGAIN) fprintf(stderr, "DEBUG: card_usb_read_io: err %d\n", err);
    return (err);
}

  data = shbuf_data(c_dev->buff);
  if (!shbuf_size(c_dev->buff) ||
      -1 == (idx = stridx(data, '\n')) || idx > 337) {
    return (SHERR_AGAIN);
  }

  memset(buf, 0, sizeof(buf));
  strncpy(buf, data, idx);
  shbuf_trim(c_dev->buff, idx);

  err = card_usb_fill(&c_dev->data.card, buf);
  if (err)
    return (err);
  c_dev->index++;

  return (0);
}

void card_usb_close(shdev_t *c_dev)
{

  if (!c_dev->handle)
    return;

  libusb_release_interface(c_dev->handle, c_dev->iface);
  libusb_close(c_dev->handle);
  shbuf_free(&c_dev->buff);

  c_dev->handle = NULL;
  c_dev->iface = 0;
  c_dev->buff = NULL;

}

#if 0
int main(void)
{
  shdev_t *c_dev;
  int vid = 0x0801;
  int pid = 0x0001;
  int err;

  libusb_init(NULL);

  c_dev = card_usb_open(vid, pid);
if (c_dev) {

  do {
    err = card_usb_read(c_dev);
  } while (err == SHERR_AGAIN);

  card_usb_close(c_dev);
}

  libusb_exit(NULL);

  return (0);
}
#endif

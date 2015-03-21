

#include "sharedaemon.h"

shdev_t *sharedaemon_device_list;

int sharedaemon_device_add(int type, int vid, int pid)
{
  shdev_t *dev;

  dev = NULL;
  switch (type) {
    case SHDEV_CARD:
      dev = card_usb_open(vid, pid);
      break;
  }
  if (!dev)
    return (SHERR_OPNOTSUPP);

  dev->next = sharedaemon_device_list;
  sharedaemon_device_list = dev;

  return (0);
}
int sharedaemon_device_poll(shdev_t *dev, int poll_ms)
{

  switch (dev->type) {
    case SHDEV_CARD:
      return (card_usb_read(dev, poll_ms));
  }

  return (SHERR_OPNOTSUPP);
}

void sharedaemon_device_remove(shdev_t *dev)
{
  
  switch (dev->type) {
    case SHDEV_CARD:
      card_usb_close(dev);
      break;
  }
/* DEBUG: remove from link list */
}

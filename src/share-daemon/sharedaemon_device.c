

#include "sharedaemon.h"

shdev_t *sharedaemon_device_list;

shdev_def_t device_def[MAX_DEVICE_DEFINITIONS] = {
  { "magtek", SHDEV_MAGTEK_VID, SHDEV_MAGTEK_PID, SHDEV_USB | SHDEV_CARD,
    shdev_card_init, NULL, shdev_card_poll, NULL, NULL, shdev_card_shutdown },
  { "ztex", SHDEV_ZTEX_VID, SHDEV_ZTEX_PID, SHDEV_USB | SHDEV_CARD, 
    shdev_fpga_init, NULL, shdev_fpga_poll, shdev_fpga_ctrl, 
    shdev_fpga_timer, shdev_fpga_shutdown },
  { "leitch", 0, 0, SHDEV_SERIAL | SHDEV_CLOCK,
NULL, NULL, NULL, NULL, NULL, NULL 
/*    shdev_leitch_init, shdev_leitch_poll, shdev_leitch_ctrl, 
    shdev_leitch_timer, shdev_leitch_shutdown*/ }
};

int sharedaemon_device_add(shdev_def_t *def)
{
  shdev_t *dev;
  int err;

  dev = (shdev_t *)calloc(1, sizeof(shdev_t));
  if (!dev)
    return (SHERR_NOMEM);

  /* assign definition */
  dev->def = def;

  /* initialize device interface */
  if (dev->def->flags & SHDEV_USB) {
    err = shdev_usb_init(dev);
    if (err) {
      free(dev);
      return (err);
    }
  }

  /* initialize device */
  if (dev->def->init) {
    err = dev->def->init(dev);
    if (err) {
      shdev_usb_shutdown(dev);
      free(dev);
      return (err);
    }
  }

  /* allocate I/O buffer */
  dev->buff = shbuf_init();

  /* add to list */
  dev->next = sharedaemon_device_list;
  sharedaemon_device_list = dev;

  if (dev->def->flags & SHDEV_START) {
    err = sharedaemon_device_start(dev); 
    if (err) {
      dev->err_state = err; /* close next cycle */
      return (err);
    }
  }

  return (0);
}

int sharedaemon_device_start(shdev_t *dev)
{
  int err;

  err = 0;
  if (dev->def->start) {
    err = dev->def->start(dev);
    if (err)
      dev->err_state = err;
  }
  
  return (err);
}

int sharedaemon_device_poll(shdev_t *dev, int poll_ms)
{
  int err;

  err = 0;
  if (dev->def->poll)
    err = dev->def->poll(dev);

  return (err);
}

/**
 * Called once per second.
 */
int sharedaemon_device_timer(shdev_t *dev)
{
  int err;

  err = 0;
  if (dev->def->timer)
    err = dev->def->timer(dev);

  return (err);
}

int sharedaemon_device_control(shdev_t *dev)
{
  int err;

  err = 0;
  if (dev->def->control)
    err = dev->def->control(dev);

  return (err);
}

int sharedaemon_device_remove(shdev_t *r_dev)
{
  
  if (r_dev->def->shutdown)
    r_dev->def->shutdown(r_dev);

  if (r_dev->def->flags & SHDEV_USB)
    shdev_usb_shutdown(r_dev);

  return (0);
}

void sharedaemon_device_free(shdev_t *r_dev)
{
  shdev_t *dev;
  shdev_t *l_dev;
  
  shbuf_free(&r_dev->buff);

  l_dev = NULL;
  for (dev = sharedaemon_device_list; dev; dev = dev->next) {
    if (dev == dev) {
      if (!l_dev) {
        sharedaemon_device_list = dev->next;
      } else {
        l_dev->next = dev->next;
      }
    }
  }
  free(dev);

}


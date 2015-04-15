
#include "pub_server.h"


int run_state = RUN_NONE;

static double _scan_stamp;

void pubd_cycle_verify(void)
{
  double now;

  now = shtimef();

  if ((now - _scan_stamp) > PUB_SCAN_WAIT_TIME) {
    run_state = RUN_SCAN;
    _scan_stamp = now;
  }

}

void pubd_cycle_init(void)
{
  pubd_file_init();
  pubd_scan_init();
}

void pubd_cycle_free(void)
{
  pubd_scan_free();
  pubd_file_free();
}

void pubd_cycle(void)
{
  struct timeval tv;
  double start_t, end_t;
  int diff;

  run_state = RUN_INIT;
  while (run_state != RUN_NONE) {
    start_t = shtimef();
    switch (run_state) {
      case RUN_IDLE:
        pubd_cycle_verify();
        break;
      case RUN_INIT:
        pubd_cycle_init();
        run_state = RUN_IDLE;
        break;
      case RUN_SCAN:
        pubd_scan();
        run_state = RUN_IDLE;
        break;
    }
    end_t = shtimef();

    /* wait remainder of 20ms */
    diff = (int)((end_t - start_t) * 1000); /* -> ms */
    if (!diff)
      continue;

    memset(&tv, 0, sizeof(tv));
    tv.tv_usec = MIN(20, diff) * 1000; /* usec */
    select(0, NULL, NULL, NULL, &tv);
  }

}


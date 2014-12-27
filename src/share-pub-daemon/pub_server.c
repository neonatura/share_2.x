
#include "pub_server.h"

static void pubd_free(void)
{
}

void pubd_terminate(int sig_num)
{

  signal(sig_num, SIGDFL);

  pubd_free();

  raise(sig_num);

}

void pubd_signal(void)
{
  signal(SIG_TERM, pubd_terminate);
}

int main(void)
{

  pubd_signal();

  pubd_cycle();
 
  return (0);
}



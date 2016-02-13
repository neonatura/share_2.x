
/*
 * @copyright
 *
 *  Copyright 2016 Neo Natura
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

#include <stdio.h>
#include <string.h>

#include "config.h"
#include "share.h"

#define EXAMPLE_PORT 48981

char prog_name[PATH_MAX+1];

/**
 * Displays the program's version information to the command console.
 */
void program_version(void)
{
  printf ("%s version %s (%s)\n"
      "\n"
      "Copyright 2016 Neo Natura\n"
      "Licensed under the GNU GENERAL PUBLIC LICENSE Version 3\n"
      "Visit 'https://github.com/neonatura/share' for more information.\n",
      prog_name, PACKAGE_VERSION, PACKAGE_NAME);
}

/**
 * Displays the program's usage information to the command console.
 */
void program_usage(void)
{
  printf (
      "%s version %s (%s)\n"
      "usage: esl_client <FILE> <HOSTNAME>\n"
      "\n"
      "Example of utilizing the Encrypted Socket Protocol.\n"
      "\n"
      "Visit 'http://www.sharelib.net/' for more information.\n",
      prog_name, PACKAGE_VERSION, PACKAGE_NAME, prog_name);
}

void main_esl_client(char *opt_path, char *opt_host, int opt_port)
{
  shbuf_t *r_buff;
  shbuf_t *buff;
  ssize_t b_len;
  uint32_t raw_len;
  char raw_data[8192];
  int err;
  int of;
  int sk;

  memset(raw_data, 0, sizeof(raw_data));

  buff = shbuf_init();
  err = shfs_mem_read(opt_path, buff);
  if (err) {
    fprintf(stderr, "error: unable to access file '%s'.\n", opt_path);
    shbuf_free(&buff);
    return;
  }
  printf ("info: loaded %d bytes from file '%s'.\n", shbuf_size(buff), opt_path);

  sk = esl_connect(opt_host, opt_port, NULL);
  if (sk < 1) {
    fprintf(stderr, "error: unable to connect to host '%s' at port %d: %s.\n", opt_host, opt_port, sherrstr(sk));
    shbuf_free(&buff);
    return;
  }
  printf ("info: connected to host '%s' at port %d.\n", opt_host, opt_port);

  /* write file to socket */
  raw_len = htonl(shbuf_size(buff));
  b_len = esl_write(sk, &raw_len, sizeof(raw_len));
  if (b_len < 0) { 
    shbuf_free(&buff);
    return;
  }
  b_len = esl_write(sk, shbuf_data(buff), shbuf_size(buff));
  if (b_len < 0) { 
    shbuf_free(&buff);
    return;
  }

  /* read file from socket */
  of = 0;
  r_buff = shbuf_init();
  while (of < shbuf_size(buff)) {
    b_len = esl_read(sk, raw_data, sizeof(raw_data));
    if (b_len < 0) {
      fprintf(stderr, "error: failure reading from socket: %s.\n", sherrstr(b_len));
      return;
    }
    if (b_len == 0) {
      sleep(1);
    }
    shbuf_cat(r_buff, raw_data, b_len);
    of += b_len;   
  }

  if (shbuf_cmp(buff, r_buff)) {
    fprintf(stderr, "%s: file transmission confirmed.\n", opt_path);
  } else {
    fprintf(stderr, "%s: file transmission failure.\n", opt_path);
  }

  shbuf_free(&buff);
  shbuf_free(&r_buff);
  shnet_close(sk);
}

int main(int argc, char *argv[])
{
  char *app_name;
  shpeer_t *app_peer;
  shpeer_t *serv_peer;
  char opt_path[PATH_MAX+1];
  char opt_host[MAXHOSTNAMELEN+1];
  int opt_port = EXAMPLE_PORT;
  int err;
  int i;

  strcpy(opt_host, "localhost");
  strcpy(opt_path, "README.txt");

  app_name = shfs_app_name(argv[0]);
  strncpy(prog_name, app_name, sizeof(prog_name));
  for (i = 1; i < argc; i++) {
    if (*argv[i] == '-') {
      if (0 == strcmp(argv[i], "-v") ||
          0 == strcmp(argv[i], "--version")) {
        program_version();
        return (0);
      }
      if (0 == strcmp(argv[i], "-h") ||
          0 == strcmp(argv[i], "--help")) {
        program_usage();
        return (0);
      }
      continue;
    }
    if (!*opt_path) {
      strncpy(opt_path, argv[i], sizeof(opt_path)-1);
      continue;
    }
    if (!*opt_host) {
      strncpy(opt_host, argv[i], sizeof(opt_host)-1);
      continue;
    }
  }

  app_peer = shapp_init(app_name, NULL, 0);

  main_esl_client(opt_path, opt_host, opt_port);

  shpeer_free(&app_peer);
  return (0);
}



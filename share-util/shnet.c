/*
 *  Copyright 2013 Brian Burrell 
 *
 *  This file is part of the Share Library.
 *  (https://github.com/briburrell/share)
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

#include "shnet.h"
#include "../SHARE-VERSION-FILE"

char process_path[PATH_MAX + 1];
char process_file_path[PATH_MAX + 1];
char process_socket_host[PATH_MAX + 1];
unsigned int process_socket_port;

void print_process_usage(void)
{
  printf (
      "%s version %s\n\n"
      "Command-line usage:\n"
      "\tshnet <mode> [-p<port>] [-f<path>] [-h<host>]\n"
      "\n"
      "Modes:\n"
      "\ttest\t\tRun diagnostic tests against the Share Library.\n"
      "\tscan\t\tScan the local computer for listening ports.\n"
      "\tserver\t\tStart up a generic server.\n"
      "\tfile\t\tAccess a sharefs file.\n"
      "\tping\t\tPrint statistics from a shnet ping server.\n"
      "\n"
      "Options:\n"
      "\t-h<host>\t\tSpecifies a network hostname.\n"
      "\t-p<port>\t\tSpecifies a ipv4/6 socket port number.\n"
      "\t-f<path>\t\tSpecifies a file path.\n"
      "\n"
      "Copyright 2013 Brian Burrell\n"
      "Licensed under the GNU GENERAL PUBLIC LICENSE Version 3\n"
      "Visit 'https://github.com/briburrell/share' for more information.\n",
      process_path, VERSION);
}

int main(int argc, char **argv)
{
  char subcmd[256];
  int mode;
  int i;

  strncpy(process_path, argv[0], PATH_MAX);

  mode = SHM_NONE;

  /* handle traditional arguments */
  for (i = 1; i < argc; i++) {
    if (0 == strcmp(argv[i], "--version")) {
      printf("%s version %s\n", process_path, VERSION);
      return (0);
    }
    if (0 == strcmp(argv[i], "--help")) {
      print_process_usage();
      return (0);
    }
  }

  memset(subcmd, 0, sizeof(subcmd));
  for (i = 1; i < argc; i++) {
    if (0 == strncmp(argv[i], "-p", 2)) {
      process_socket_port = (unsigned int)atoi(argv[i] + 2);
      continue;
    }
    if (0 == strncmp(argv[i], "-f", 2)) {
      strncpy(process_file_path, argv[i] + 2, sizeof(process_file_path) - 1);
      continue;
    }
    if (0 == strncmp(argv[i], "-h", 2)) {
      strncpy(process_socket_host, argv[i] + 2, sizeof(process_socket_host) - 1);
      continue;
    }
    if (0 == strcmp(argv[i], "test")) {
      mode = SHM_TEST;
    } else if (0 == strcmp(argv[i], "scan")) {
      mode = SHM_SCAN;
    } else if (0 == strcmp(argv[i], "server")) {
      mode = SHM_SERVER;
    } else if (0 == strcmp(argv[i], "file")) {
      mode = SHM_FILE;
    } else if (0 == strcmp(argv[i], "ping")) {
      mode = SHM_PING;
    } else if (argv[i][0] != '-') { 
      if (!*subcmd)
        strncpy(subcmd, argv[i], sizeof(subcmd) - 1);
    }
  }

  switch (mode) {
    case SHM_NONE:
      print_process_usage();
      break;
    case SHM_TEST:
      shnet_test(argc, argv);
      break;
    case SHM_SCAN:
      shnet_scan(argc, argv);
      break;
    case SHM_SERVER:
      shnet_server(subcmd);
      break;
    case SHM_FILE:
      shnet_file(subcmd, process_file_path);
      break;
    case SHM_PING:
      shnet_ping(subcmd);
      break;
  }

	return (0);
}



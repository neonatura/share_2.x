/*
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
 */  

#include "share.h"
#include "sharetool.h"

char process_path[PATH_MAX + 1];
char process_file_path[PATH_MAX + 1];
char process_outfile_path[PATH_MAX + 1];
char process_socket_host[PATH_MAX + 1];
unsigned int process_socket_port;

void print_process_version(void)
{
  char *app_name = shfs_app_name(process_path);
  printf (
      "%s version %s\n"
      "\n"
      "Copyright 2013 Brian Burrell\n"
      "Licensed under the GNU GENERAL PUBLIC LICENSE Version 3\n",
      app_name, VERSION); 
}

void print_process_usage(void)
{
  printf (
      "%s: Command-line tool for the Share Library.\n"
      "\n"
      "Usage: %s [OPTION] <mode> <command>\n"
      "\n"
      "Modes:\n"
      "\ttest\t\tRun diagnostic tests against the Share Library.\n"
//      "\tscan\t\tScan the local computer for listening ports.\n"
//      "\tserver\t\tStart up a generic server.\n"
      "\tfile\t\tAccess a sharefs file.\n"
      "\tpref\t\tDefine or review sharelib global preferences.\n"
//      "\tping\t\tPrint statistics from a shnet ping server.\n"
      "\n"
      "Commands:\n"
      "\tFile/Pref Commands:\n"
      "\t\tset:<token>=<value>\tSet the reference <token> to <value>.\n"
      "\t\tget:<token>\t\tGet the value of the reference <token>.\n"
      "\n"
      "Options:\n"
      "\t--help\t\t\t\tShows program usage instructions.\n"
      "\t--version\t\t\tShows program version.\n"
      "\t--nosync\t\t\tDo not write outside of the base directory.\n"
      "\t-b<dir>[=\$HOME/.share]\t\tSpecifies the base directory to use for sharefs.\n"
      "\t-d<host>[=localhost]\t\tSpecifies a network hostname.\n"
      "\t-p<port>\t\t\tSpecifies a ipv4/6 socket port number.\n"
      "\t-f<path>\t\t\tSpecifies a input file path.\n"
      "\t-o<path>\t\t\tSpecifies a output file path.\n"
      "\n"
      "Examples:\n"
      "\tshnet scan\t\t\tScan the localhost for open ports.\n"
      "\tshnet file set:shfs_track=1\tSet an attribute on a file.\n" 
      "\tshnet ping -dhost.com\t\tStart a ping client to 'host.com'.\n"
      "\n"
      "Visit 'https://github.com/briburrell/share' for more information.\n"
      "See 'man libshare' for additional manuals on the Share Library.\n"
      "Report bugs to <%s>.\n",
      get_libshare_title(), process_path, get_libshare_email());
}

int main(int argc, char **argv)
{
  char subcmd[256];
  char **args;
  int mode;
  int i;

  strncpy(process_path, argv[0], PATH_MAX);

  mode = SHM_NONE;

  /* handle traditional arguments */
  for (i = 1; i < argc; i++) {
    if (0 == strcmp(argv[i], "--version")) {
      print_process_version();
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
    if (0 == strncmp(argv[i], "-o", 2)) {
      strncpy(process_outfile_path, argv[i] + 2, sizeof(process_outfile_path) - 1);
      continue;
    }
    if (0 == strncmp(argv[i], "-d", 2)) {
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
    } else if (0 == strcmp(argv[i], "diff")) {
      mode = SHM_DIFF;
    } else if (0 == strcmp(argv[i], "pref")) {
      mode = SHM_PREF;
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
      share_test(argc, argv);
      break;
    case SHM_SCAN:
      share_scan(argc, argv);
      break;
    case SHM_SERVER:
      share_server(subcmd);
      break;
    case SHM_FILE:
      share_file(subcmd, process_file_path);
      break;
    case SHM_PING:
      share_ping(subcmd);
      break;
    case SHM_DIFF:
      args = (char **)calloc(7, sizeof(char *));
      args[0] = argv[0];
      args[1] = "-f";
      args[2] = "-e";
      args[3] = "-s";
      args[4] = process_file_path;
      args[5] = subcmd;
      args[6] = process_outfile_path;
      xd3_main_cmdline(7, args);
      break;
    case SHM_PREF:
      sharetool_pref(subcmd);
      break;
  }

	return (0);
}



/*
 *  Copyright 2013 Neo Natura
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
int process_run_mode;
shfs_t *sharetool_fs;

void print_process_version(void)
{
  char *app_name = shfs_app_name(process_path);
  printf (
      "%s version %s\n"
      "\n"
      "Copyright 2013 Neo Natura\n"
      "Licensed under the GNU GENERAL PUBLIC LICENSE Version 3\n",
      app_name, VERSION); 
}

void print_process_usage(void)
{

  printf (
      "share-util: Command-line tools for the Share Library.\n"
      "\n"
      );
  switch (process_run_mode) {
    case SHM_FILE_LIST:
      printf("Usage: %s [OPTION] [<path>]\n", process_path);
      printf("List entries in a shfs partition.\n");
      break;
    case SHM_FILE_MKDIR:
      printf("Usage: %s [OPTION] <path>\n", process_path);
      printf("Create a directory in a shfs partition.\n");
      break;
    case SHM_FILE_REMOVE:
      printf("Usage: %s [OPTION] <path>\n", process_path);
      printf("Remove a entry in a shfs partition.\n");
      break;
    case SHM_FILE_CAT:
      printf("Usage: %s [OPTION] <path>\n", process_path);
      printf("Print a file in a shfs partition.\n");
      break;
    case SHM_FILE_DIFF:
      printf("Usage: %s [OPTION] <path> [<path>]\n", process_path);
      printf("Print the differences between file(s).\n");
      break;
    case SHM_FILE_COPY:
      printf("Usage: %s [OPTION] <path> [<path>]\n", process_path);
      printf ("Copy a file in a shfs partition.\n");
      break;
    case SHM_FILE_IMPORT:
      printf("Usage: %s [OPTION] <path> [<path>]\n", process_path);
      printf ("Copy a file into a shfs partition.\n");
      break;
    case SHM_FILE_EXPORT:
      printf("Usage: %s [OPTION] <path> [<path>]\n", process_path);
      printf ("Copy a file out of a shfs partition.\n");
      break;
    case SHM_PING:
      printf("Usage: %s [OPTION] [<host>[:<port>]]\n", process_path);
      printf ("Verify and track latency of daemons.\n");
      break;
    case SHM_PREF:
      printf("Usage: %s [OPTION] [<name>[=<value>]]\n", process_path);
      printf("Define or view global preferences.\n");
      break;
    default:
      printf ("Usage: %s [OPTION]\n", process_path);
      break;
  }
  printf(
      "\n"
      "Options:\n"
      "\t-h | --help\tShows program usage instructions.\n"
      "\t-v | --version\tShows program version.\n"
      "\t-a | --all\tShow verbose information (when applicable).\n"
//      "\t-i | --inode\tPrint shfs inode journal indexes (when applicable).\n"
      "\t-l | --local\tUse local hard-drive as source or destination.\n"
//      "\t-d | --decode\tDecode the data contents referenced (when applicable).\n"
      "\t-c <name>\tUse a specific application peer instance.\n"
      "\n"
      "Visit 'http://docs.sharelib.net/' for libshare API documentation.\n"
      "Report bugs to <%s>.\n",
      get_libshare_email());
}

int main(int argc, char **argv)
{
  shpeer_t *fs_peer;
  char peer_name[4096];
  char subcmd[256];
  char *app_name;
  char **args;
  int pflags;
  int i;

  process_run_mode = SHM_NONE;
  memset(subcmd, 0, sizeof(subcmd));

  app_name = shfs_app_name(argv[0]);
  strncpy(process_path, app_name, PATH_MAX);

  if (0 == strcmp(app_name, "shls")) {
    process_run_mode = SHM_FILE_LIST;
  } else if (0 == strcmp(app_name, "shcp")) {
    process_run_mode = SHM_FILE_COPY;
  } else if (0 == strcmp(app_name, "shin")) {
    process_run_mode = SHM_FILE_IMPORT;
  } else if (0 == strcmp(app_name, "shout")) {
    process_run_mode = SHM_FILE_EXPORT;
  } else if (0 == strcmp(app_name, "shmkdir")) {
    process_run_mode = SHM_FILE_MKDIR;
  } else if (0 == strcmp(app_name, "shrm")) {
    process_run_mode = SHM_FILE_REMOVE;
  } else if (0 == strcmp(app_name, "shcat")) {
    process_run_mode = SHM_FILE_CAT;
  } else if (0 == strcmp(app_name, "shpref")) {
    process_run_mode = SHM_PREF;
  } else if (0 == strcmp(app_name, "shping")) {
    process_run_mode = SHM_PING;
  } else if (0 == strcmp(app_name, "shdiff")) {
    process_run_mode = SHM_FILE_DIFF;
  } else if (0 == strcmp(app_name, "shattr")) {
    process_run_mode = SHM_FILE_ATTR;
  }

  pflags = 0;
  memset(peer_name, 0, sizeof(peer_name));
  for (i = 1; i < argc; i++) {
    if (0 == strcmp(argv[i], "-a") ||
        0 == strcmp(argv[i], "--all")) {
      pflags |= PFLAG_VERBOSE;
    } else if (0 == strcmp(argv[i], "-l") ||
        0 == strcmp(argv[i], "--local")) {
      pflags |= PFLAG_LOCAL;
    } else if (0 == strcmp(argv[i], "-h") ||
        0 == strcmp(argv[i], "--help")) {
      pflags |= PFLAG_SYNTAX;
    } else if (0 == strcmp(argv[i], "-v") ||
        0 == strcmp(argv[i], "--version")) {
      pflags |= PFLAG_VERSION;
    } else if (0 == strcmp(argv[i], "-c")) {
      if ( (i + 1) < argc ) {
        i++;
        strncpy(peer_name, argv[i], sizeof(peer_name) - 1);
      }
    } else {
      if (*subcmd)
        strncat(subcmd, " ", sizeof(subcmd) - 1);
      strncat(subcmd, argv[i], sizeof(subcmd) - 1);
    } 
  }

  if (pflags & PFLAG_VERSION) {
    print_process_version();
    exit(0);
  }
  if (pflags & PFLAG_SYNTAX) {
    print_process_usage();
    exit(0);
  }

  fs_peer = NULL;
  if (*peer_name) {
    char *ptr = strchr(peer_name, '@');
    if (ptr) {
      *ptr++ = '\0';
      fs_peer = shpeer_init(peer_name, ptr);
    } else {
      fs_peer = shpeer_init(peer_name, NULL);
    }
  }
  //sharetool_fs = shfs_init(fs_peer);

  switch (process_run_mode) {
    case SHM_FILE_LIST:
      share_file_list(subcmd, pflags);
      break;
    case SHM_FILE_IMPORT:
      share_file_import(subcmd, pflags);
      break;
    case SHM_FILE_CAT:
      share_file_cat(subcmd, pflags);
      break;
    case SHM_FILE_MKDIR:
      share_file_mkdir(subcmd, pflags);
      break;
    case SHM_FILE_REMOVE:
      share_file_remove(subcmd, pflags);
      break;
    case SHM_FILE_ATTR:
      share_file_attr(subcmd, pflags);
      break;

#if 0
    case SHM_FILE_DIFF:
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
#endif

    default:
      print_process_usage();
      break;
  }

  if (fs_peer)
    shpeer_free(&fs_peer);
//  shfs_free(&sharetool_fs);

	return (0);
}


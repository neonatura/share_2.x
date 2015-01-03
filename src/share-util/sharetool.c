
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

FILE *sharetool_fout;

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
      printf("Usage: %s [OPTION] [PATH]\n", process_path);
      printf("List entries in a shfs partition.\n");
      break;
    case SHM_FILE_MKDIR:
      printf("Usage: %s [OPTION] [PATH]\n", process_path);
      printf("Create a directory in a shfs partition.\n");
      break;
    case SHM_FILE_REMOVE:
      printf("Usage: %s [OPTION] [PATH]\n", process_path);
      printf("Remove a entry in a shfs partition.\n");
      break;
    case SHM_FILE_CAT:
      printf("Usage: %s [OPTION] [PATH]\n", process_path);
      printf("Print a file in a shfs partition.\n");
      break;
    case SHM_FILE_COPY:
      printf("Usage: %s [OPTION] [PATH] ..\n", process_path);
      printf ("Copy a file to another location.\n");
      break;
    case SHM_FILE_DIFF:
      printf("Usage: %s [OPTION] [PATH]\n", process_path);
      printf("Generate a binary patch file from two files.\n");
      break;
    case SHM_FILE_PATCH:
      printf("Usage: %s [OPTION] [PATH]\n", process_path);
      printf("Apply a binary patch file to a file.\n");
      break;
    case SHM_PREF:
      printf("Usage: %s [OPTION] [PREFERENCE] [<value>]\n", process_path);
      printf("Define or view global preferences.\n");
      break;
    case SHM_FILE_REV:
      printf("Usage: %s [OPTION] [COMMAND] [PATH|<hash>]\n", process_path);
      printf("Track file revisions.\n");
      break;
    default:
      printf ("Usage: %s [OPTION]\n", process_path);
      break;
  }
  printf
    (
     "\n"
     "Options:\n"
     "\t-h | --help\t\tShows program usage instructions.\n"
     "\t-v | --version\t\tShows program version.\n"
     "\t-a | --all\t\tShow verbose information.\n"
     "\t-o | --out <path>\tPrint output to a file.\n"
     "\t-r | --recursive\tCopy directories recursive.\n"
     "\n"
    );

  if (process_run_mode == SHM_FILE_REV) {
    printf (
        "Commands:\n"
        "\tadd\tAdd file(s) to the working repository.\n"
        "\t\tadd <path>\tAdd supplemental file(s) to the repository.\n"
        "\t\tNote: This can also be achieved by 'shattr +r <path>'.\n" 
        "\tbranch\tCreate an alternate repository based on current revision.\n"
        "\t\tbranch\tShow all known branch names.\n"
        "\t\tbranch <name>\tCreate a new branch in the repository.\n"
        "\tcommit\tCommit a new version of file modifications.\n"
        "\t\tcommit\tCommit all file(s) in current directory.\n"
        "\t\tcommit <path>\tCommit specified file(s).\n"
        "\tdiff\tDisplay the difference between versions.\n"
        "\t\tdiff\tShow modifications to current directory.\n"
        "\t\tdiff <path>\tShow modifications to specified file(s).\n"
        "\t\tdiff <path> <hash>\tShow modifications since revision.\n"
        "\tlog\tDisplay the revision commit history.\n"
        "\t\tlog\tDisplay the current revision commit history.\n"
        "\t\tlog <path>\tDisplay the commit history for specified file(s).\n"
        "\t\tlog <hash>\tDisplay revision's commit history.\n"
        "\t\tlog <path> <hash>\tDisplay revision for file(s).\n"
        "\trevert\tReturn specified path to last committed version.\n"
        "\t\trevert\tRevert all file(s) in current directory.\n"
        "\t\trevert <path>\tRevert specified file(s).\n"
        "\tstatus\tDisplay status of modified files.\n"
        "\t\tstatus\tDisplay status of current directory.\n"
        "\t\tstatus <path>\tDisplay status of specified file(s).\n"
        "\tswitch\tSet the current working revision.\n"
        "\t\tswitch master\tSet working area to initial branch.\n"
        "\t\tswitch HEAD\tSet working area to initial branch.\n"
        "\t\tswitch PREV\tSet working area to prior committed revision.\n"
        "\t\tswitch <hash>\tSet working area to committed revision.\n"
        "\t\tswitch <tag>\tSet working area to a tag reference.\n"
        "\t\tswitch <branch>\tSet working area to latest branch reference.\n"
        "\ttag\tCreate a named reference to a file revision.\n"
        "\t\ttag\tShow all known tag names.\n"
        "\t\ttag <name> <path>\tReference working revision of <path> file(s).\n"
        "\t\ttag <name> <hash>\tReference a committed revision.\n"
        "\n"
        );
  }

  if (process_run_mode == SHM_PREF) { 
    printf(
        "Preferences:\n"
        "\tuser.name\tThe login user's real name.\n"
        "\tuser.email\tThe login user's email address.\n"
        "\n"
        );
  } else {
    printf(
        "Paths:\n"
        "\t<filename>\n"
        "\t\tA local hard-drive path in the current directory.\n"
        "\t/<path>/[<filename>]\n"
        "\t\tA path in the default share-fs partition.\n"
        "\tfile://<path>/[<filename>]\n"
        "\t\tAn absolute local hard-drive path.\n"
        "\t<app>:/[<group>[@<host>[:<port>]]]/<path>/[<filename>]\n"
        "\t\tAn absoluate path in a share-fs partition.\n"
        "\n"
        );
  }

  printf(
      "Visit 'http://docs.sharelib.net/' for libshare API documentation.\n"
      "Report bugs to <%s>.\n",
      get_libshare_email()
      );

}

int main(int argc, char **argv)
{
  char out_path[PATH_MAX+1];
  char peer_name[4096];
  char subcmd[256];
  char *app_name;
  char **args;
  int arg_cnt;
  int err_code;
  int pflags;
  int err;
  int i;

  sharetool_fout = stdout;
  process_run_mode = SHM_NONE;

  app_name = shfs_app_name(argv[0]);
  strncpy(process_path, app_name, PATH_MAX);

  if (0 == strcmp(app_name, "shls")) {
    process_run_mode = SHM_FILE_LIST;
  } else if (0 == strcmp(app_name, "shcp")) {
    process_run_mode = SHM_FILE_COPY;
  } else if (0 == strcmp(app_name, "shmkdir")) {
    process_run_mode = SHM_FILE_MKDIR;
  } else if (0 == strcmp(app_name, "shrm")) {
    process_run_mode = SHM_FILE_REMOVE;
  } else if (0 == strcmp(app_name, "shcat")) {
    process_run_mode = SHM_FILE_CAT;
  } else if (0 == strcmp(app_name, "shpref")) {
    process_run_mode = SHM_PREF;
  } else if (0 == strcmp(app_name, "shdiff")) {
    process_run_mode = SHM_FILE_DIFF;
  } else if (0 == strcmp(app_name, "shpatch")) {
    process_run_mode = SHM_FILE_PATCH;
  } else if (0 == strcmp(app_name, "shattr")) {
    process_run_mode = SHM_FILE_ATTR;
  } else if (0 == strcmp(app_name, "shrev")) {
    process_run_mode = SHM_FILE_REV;
  }

  args = (char **)calloc(argc+1, sizeof(char *));
  args[0] = strdup(process_path);
  arg_cnt = 1;

  pflags = 0;
  memset(out_path, 0, sizeof(out_path));
  memset(peer_name, 0, sizeof(peer_name));
  for (i = 1; i < argc; i++) {
    if (0 == strcmp(argv[i], "-a") ||
        0 == strcmp(argv[i], "--all")) {
      pflags |= PFLAG_VERBOSE;
    } else if (0 == strcmp(argv[i], "-r") ||
        0 == strcmp(argv[i], "--recursive")) {
      pflags |= PFLAG_RECURSIVE;
#if 0
    } else if (0 == strcmp(argv[i], "-l") ||
        0 == strcmp(argv[i], "--local")) {
      pflags |= PFLAG_LOCAL;
#endif
    } else if (0 == strcmp(argv[i], "-h") ||
        0 == strcmp(argv[i], "--help")) {
      pflags |= PFLAG_SYNTAX;
    } else if (0 == strcmp(argv[i], "-v") ||
        0 == strcmp(argv[i], "--version")) {
      pflags |= PFLAG_VERSION;
    } else if (0 == strcmp(argv[i], "-o") ||
        0 == strcmp(argv[i], "--out")) {
      if (i + 1 < argc) {
        i++;
        strncpy(out_path, argv[i], sizeof(out_path) - 1);
      } else {
        printf ("%s: warning: no output path specified.", process_path); 
      } 
#if 0
    } else if (0 == strcmp(argv[i], "-c")) {
      if ( (i + 1) < argc ) {
        i++;
        strncpy(peer_name, argv[i], sizeof(peer_name) - 1);
      }
#endif
    } else {
      args[arg_cnt] = strdup(argv[i]);
      arg_cnt++;
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

  if (*out_path) {
    sharetool_fout = fopen(out_path, "wb");
  }

  memset(subcmd, 0, sizeof(subcmd));
  for (i = 1; i < arg_cnt; i++) {
    if (*subcmd)
      strcat(subcmd, " ");
    strcat(subcmd, args[i]);
  }

  err_code = 0;
  switch (process_run_mode) {
    case SHM_FILE_LIST:
      share_file_list(subcmd, pflags);
      break;
#if 0
    case SHM_FILE_IMPORT:
      share_file_import(subcmd, pflags);
      break;
#endif
    case SHM_FILE_CAT:
      for (i = 1; i < arg_cnt; i++) {
        err_code = share_file_cat(args[i], pflags);
        if (err_code) {
          fprintf(stderr, "%s: cannot access %s: %s.\n", process_path, subcmd, sherr_str(err_code)); 
        }
      }
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

    case SHM_FILE_DIFF:
      err = share_file_delta(args, arg_cnt, pflags);
      if (err) {
        fprintf(stderr, "%s: error: %s.\n", process_path, sherr_str(err));
      }
      break;
    case SHM_FILE_PATCH:
      err = share_file_patch(args, arg_cnt, pflags);
      if (err) {
        fprintf(stderr, "%s: error: %s.\n", process_path, sherr_str(err));
      }
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

#endif

    case SHM_PREF:
      err = sharetool_pref(subcmd);
      if (err) {
        if (err == SHERR_INVAL)
          fprintf(stderr, "%s: error: no preference name specified.\n", process_path);
        else if (err == SHERR_NOENT)
          fprintf(stderr, "%s: warning: preference has no value set.\n", process_path);
        return (1);
      }
      break;

    case SHM_FILE_REV:
      err = share_file_revision(args, arg_cnt, pflags);
      if (err) {
        fprintf(stderr, "%s: error: %s\n", process_path, sherr_str(err));
        return (1);
      }
      break;

    default:
      print_process_usage();
      break;
  }

  if (sharetool_fout) {
    fclose(sharetool_fout);
  }

	return (err_code);
}


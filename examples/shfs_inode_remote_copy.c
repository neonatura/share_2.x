
/*
 * @copyright
 *
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
 *
 *  @endcopyright
*/

#include "share.h"

int main(int argc, char **argv) 
{
  shfs_t *tree;
  shpeer_t *peer;
  shfs_ino_t *root_dir;
  shfs_ino_t *pub_dir;
  shfs_ino_t *net_file;

  peer = shpeer_host("share.neo-natura.com");
  tree = shfs_init(peer);
  net_file = shfs_file_find(tree, "/system/version");
  shfs_file_pipe(net_file, fileno(stdout));

  return (0);
}
 

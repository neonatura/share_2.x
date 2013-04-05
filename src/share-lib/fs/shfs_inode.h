
/*
 * @copyright
 *
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
 *
 *  @endcopyright
 *
 *  @file shfs_inode.h
*/  

#ifndef __FS__SHFS_INODE_H__
#define __FS__SHFS_INODE_H__

/**
 * @addtogroup
 * @{
 */

/**
 * Retrieve a sharefs inode directory entry based on a given parent inode and path name.
 *
 * Example making a new directory from the base directory of the default local sharefs partition:
 * int main(int argc, char **argv) {
 * shfs_t *tree = shfs_init(argv[0], 0);
 * shfs_ino_t *root_dir = shfs_inode(NULL, SHINODE_PARTITION);
 * shfs_ino_t *new_dir = shfs_inode(root_dir, "new_dir", SHINODE_DIRECTORY);
 * }
 * Example command-line usage;
 * shnet file mkdir -f/new_dir 
 *
 * Example of creating a local sym-link to a remote sharefs file.
 * int main(int argc, char **argv) {
 * shfs_t *tree = shfs_init(argv[0], 0);
 * shfs_ino_t *root_dir = tree->root_ino;
 * shfs_ino_t *in_dir = shfs_inode(root_dir, "in_dir", SHINODE_DIRECTORY);
 * shfs_ino_t *lcl_file = shfs_inode(in_dir, "share://192.1.0.1/public/netfile.txt", SHINODE_REFERENCE);
 * shfs_write_print(stdout, lcl_file);
 * return (0);
 * }
 * 
 * Example of copying a remote file to the local filesystem's current directory:
 * int main(int argc, char **argv) {
 * shfs_t *tree = shfs_init("share://192.1.0.1/", SHFS_REMOTE);
 * shfs_ino_t *root_dir = tree->root_ino;
 * shfs_ino_t *pub_dir = shfs_inode(root_dir, "public", SHINODE_DIRECTORY);
 * shfs_ino_t *net_file = shfs_inode(pub_dir, "netfile.txt", 0);
 * shfs_write_print(stdout, net_file);
 * return (0);
 * }
 *
 * The following creates a local file in the path "/remote/netfile.txt" which references a remote file at host "192.1.0.1" named "/public/netfile.txt".
 * shfs_t *net_tree = shfs_init("192.1.0.1", SHFS_REMOTE);
 * shfs_ino_t *net_node = shfs_node_entry(net_tree->base_ino, "public", SHFS_DIRECTORY);
 * shfs_ino_t *lcl_root = shfs_node_entry(root, "share://192.1.0.1/remote/netfile.txt", 0);
 *
 * @note Searches for a reference to a sharefs inode labelled "name" in the @a parent inode.
 * @note A new inode is created if a pre-existing one is not found.
 * @param parent The parent inode such as a directory where the file presides.
 * @param name The relational pathname of the file being referenced.
 * @param mode The type of information that this inode is referencing (SHINODE_XX).
 * @returns A @c shfs_node is returned based on the @c parent, @c name, @c and mode specified. If one already exists it will be returned, and otherwise a new entry will be created.
 */
shfs_ino_t *shfs_inode(shfs_ino_t *parent, char *name, int mode);

/**
 * Retrieve the data segment of a sharefs filesystem inode.
 * @param tree The sharefs partition allocated by @c shfs_init().
 * @param inode The inode whose data is being retrieved.
 * @param data_p The inode content's return data will be filled here.
 * @param data_of The offset to begin reading data from the inode.
 * @param data_len The length of data to be read.
 * @returns A zero (0) on success, and a (-1) if the file does not exist.
 */
int shfs_inode_data(shfs_t *tree, shfs_ino_t *inode, char **data_p, shfs_size_t data_of, shfs_size_t data_len);

/**
 * @}
 */

#endif /* ndef __FS__SHFS_INODE_H__ */


/* ----------------------------------------------------------------------------
 * This file was automatically generated by SWIG (http://www.swig.org).
 * Version 2.0.9
 * 
 * This file is not intended to be easily readable and contains a number of 
 * coding conventions designed to improve portability and efficiency. Do not make
 * changes to this file unless you know what you are doing--modify the SWIG 
 * interface file instead. 
 * ----------------------------------------------------------------------------- */

#ifndef PHP_SHARE_PHP_H
#define PHP_SHARE_PHP_H

extern zend_module_entry share_php_module_entry;
#define phpext_share_php_ptr &share_php_module_entry

#ifdef PHP_WIN32
# define PHP_SHARE_PHP_API __declspec(dllexport)
#else
# define PHP_SHARE_PHP_API
#endif

#ifdef ZTS
#include "TSRM.h"
#endif

PHP_MINIT_FUNCTION(share_php);
PHP_MSHUTDOWN_FUNCTION(share_php);
PHP_RINIT_FUNCTION(share_php);
PHP_RSHUTDOWN_FUNCTION(share_php);
PHP_MINFO_FUNCTION(share_php);

ZEND_NAMED_FUNCTION(_wrap_test_main);
ZEND_NAMED_FUNCTION(_wrap_get_libshare_email);
ZEND_NAMED_FUNCTION(_wrap_get_libshare_version);
ZEND_NAMED_FUNCTION(_wrap_get_libshare_title);
ZEND_NAMED_FUNCTION(_wrap_get_libshare_path);
ZEND_NAMED_FUNCTION(_wrap_shcrc);
ZEND_NAMED_FUNCTION(_wrap_shfile_init);
ZEND_NAMED_FUNCTION(_wrap_shfile_free);
ZEND_NAMED_FUNCTION(_wrap_shpeer);
ZEND_NAMED_FUNCTION(_wrap_ashpeer);
ZEND_NAMED_FUNCTION(_wrap_shpeer_free);
ZEND_NAMED_FUNCTION(_wrap_shpref_path);
ZEND_NAMED_FUNCTION(_wrap_shpref_init);
ZEND_NAMED_FUNCTION(_wrap_shpref_free);
ZEND_NAMED_FUNCTION(_wrap_shpref_get);
ZEND_NAMED_FUNCTION(_wrap_shpref_set);
ZEND_NAMED_FUNCTION(_wrap_shtime);
ZEND_NAMED_FUNCTION(_wrap_shtime64);
ZEND_NAMED_FUNCTION(_wrap_shctime64);
ZEND_NAMED_FUNCTION(_wrap_shutime64);
ZEND_NAMED_FUNCTION(_wrap_shmsgget);
ZEND_NAMED_FUNCTION(_wrap_shmsgsnd);
ZEND_NAMED_FUNCTION(_wrap_shmsgrcv);
ZEND_NAMED_FUNCTION(_wrap_shmsgctl);
ZEND_NAMED_FUNCTION(_wrap_shfs_journal_index);
ZEND_NAMED_FUNCTION(_wrap_shfs_app_name);
ZEND_NAMED_FUNCTION(_wrap_shfs_crc);
ZEND_NAMED_FUNCTION(_wrap_shfs_init);
ZEND_NAMED_FUNCTION(_wrap_shfs_free);
ZEND_NAMED_FUNCTION(_wrap_shfs_partition_id);
ZEND_NAMED_FUNCTION(_wrap_shfs_journal_path);
ZEND_NAMED_FUNCTION(_wrap_shfs_journal_open);
ZEND_NAMED_FUNCTION(_wrap_shfs_journal_scan);
ZEND_NAMED_FUNCTION(_wrap_shfs_journal_close);
ZEND_NAMED_FUNCTION(_wrap_shfs_journal_block);
ZEND_NAMED_FUNCTION(_wrap_shfs_journal_size);
ZEND_NAMED_FUNCTION(_wrap_shfs_inode);
ZEND_NAMED_FUNCTION(_wrap_shfs_inode_tree);
ZEND_NAMED_FUNCTION(_wrap_shfs_inode_root);
ZEND_NAMED_FUNCTION(_wrap_shfs_inode_parent);
ZEND_NAMED_FUNCTION(_wrap_shfs_inode_write_entity);
ZEND_NAMED_FUNCTION(_wrap_shfs_inode_write_block);
ZEND_NAMED_FUNCTION(_wrap_shfs_inode_read_block);
ZEND_NAMED_FUNCTION(_wrap_shfs_inode_token);
ZEND_NAMED_FUNCTION(_wrap_shfs_inode_filename_set);
ZEND_NAMED_FUNCTION(_wrap_shfs_inode_filename_get);
ZEND_NAMED_FUNCTION(_wrap_shfs_inode_path);
ZEND_NAMED_FUNCTION(_wrap_shfs_inode_id);
ZEND_NAMED_FUNCTION(_wrap_shfs_inode_print);
ZEND_NAMED_FUNCTION(_wrap_shfs_inode_block_print);
ZEND_NAMED_FUNCTION(_wrap_shfs_dir_base);
ZEND_NAMED_FUNCTION(_wrap_shfs_dir_cwd);
ZEND_NAMED_FUNCTION(_wrap_shfs_dir_parent);
ZEND_NAMED_FUNCTION(_wrap_shfs_dir_entry);
ZEND_NAMED_FUNCTION(_wrap_shfs_dir_find);
ZEND_NAMED_FUNCTION(_wrap_shfs_meta);
ZEND_NAMED_FUNCTION(_wrap_shfs_meta_save);
ZEND_NAMED_FUNCTION(_wrap_shfs_meta_get);
ZEND_NAMED_FUNCTION(_wrap_shfs_meta_perm);
ZEND_NAMED_FUNCTION(_wrap_shfs_meta_set);
ZEND_NAMED_FUNCTION(_wrap_shfs_sig_gen);
ZEND_NAMED_FUNCTION(_wrap_shfs_sig_get);
ZEND_NAMED_FUNCTION(_wrap_shfs_sig_verify);
ZEND_NAMED_FUNCTION(_wrap_shfs_read_mem);
ZEND_NAMED_FUNCTION(_wrap_shfs_write_mem);
ZEND_NAMED_FUNCTION(_wrap_shfs_file_write);
ZEND_NAMED_FUNCTION(_wrap_shfs_file_read);
ZEND_NAMED_FUNCTION(_wrap_shfs_file_find);
ZEND_NAMED_FUNCTION(_wrap_shfs_file_pipe);
ZEND_NAMED_FUNCTION(_wrap_shfs_file_key);
ZEND_NAMED_FUNCTION(_wrap_shfs_stat);
ZEND_NAMED_FUNCTION(_wrap_shfs_cache_get);
ZEND_NAMED_FUNCTION(_wrap_shfs_cache_set);
ZEND_NAMED_FUNCTION(_wrap_shfs_inode_cache_free);
ZEND_NAMED_FUNCTION(_wrap_shfs_aux_write);
ZEND_NAMED_FUNCTION(_wrap_shfs_aux_read);
ZEND_NAMED_FUNCTION(_wrap_shfs_aux_pipe);
ZEND_NAMED_FUNCTION(_wrap_shfs_aux_crc);
ZEND_NAMED_FUNCTION(_wrap_shlog);
ZEND_NAMED_FUNCTION(_wrap_shlog_print);
ZEND_NAMED_FUNCTION(_wrap_shlog_print_line);
ZEND_NAMED_FUNCTION(_wrap_shlog_level_label);
ZEND_NAMED_FUNCTION(_wrap_shlog_init);
ZEND_NAMED_FUNCTION(_wrap_shlog_free);
ZEND_NAMED_FUNCTION(_wrap_shnet_accept);
ZEND_NAMED_FUNCTION(_wrap_shnet_bindsk);
ZEND_NAMED_FUNCTION(_wrap_shnet_bind);
ZEND_NAMED_FUNCTION(_wrap_shnet_close);
ZEND_NAMED_FUNCTION(_wrap_shnet_conn);
ZEND_NAMED_FUNCTION(_wrap_shnet_fcntl);
ZEND_NAMED_FUNCTION(_wrap_shnet_gethostbyname);
ZEND_NAMED_FUNCTION(_wrap_shnet_peer);
ZEND_NAMED_FUNCTION(_wrap_shnet_read);
ZEND_NAMED_FUNCTION(_wrap_shnet_sk);
ZEND_NAMED_FUNCTION(_wrap_shnet_socket);
ZEND_NAMED_FUNCTION(_wrap_shnet_host);
ZEND_NAMED_FUNCTION(_wrap_shnet_write);
ZEND_NAMED_FUNCTION(_wrap_shnet_verify);
ZEND_NAMED_FUNCTION(_wrap_shnet_select);
ZEND_NAMED_FUNCTION(_wrap_shbuf_init);
ZEND_NAMED_FUNCTION(_wrap_shbuf_catstr);
ZEND_NAMED_FUNCTION(_wrap_shbuf_cat);
ZEND_NAMED_FUNCTION(_wrap_shbuf_size);
ZEND_NAMED_FUNCTION(_wrap_shbuf_data);
ZEND_NAMED_FUNCTION(_wrap_shbuf_clear);
ZEND_NAMED_FUNCTION(_wrap_shbuf_trim);
ZEND_NAMED_FUNCTION(_wrap_shbuf_free);
ZEND_NAMED_FUNCTION(_wrap_shbuf_grow);
ZEND_NAMED_FUNCTION(_wrap_shbuf_file);
ZEND_NAMED_FUNCTION(_wrap_shbuf_growmap);
ZEND_NAMED_FUNCTION(_wrap_shbuf_map);
ZEND_NAMED_FUNCTION(_wrap_shkey_bin);
ZEND_NAMED_FUNCTION(_wrap_shkey_str);
ZEND_NAMED_FUNCTION(_wrap_shkey_num);
ZEND_NAMED_FUNCTION(_wrap_shkey_uniq);
ZEND_NAMED_FUNCTION(_wrap_shkey_free);
ZEND_NAMED_FUNCTION(_wrap_shkey_print);
ZEND_NAMED_FUNCTION(_wrap_ashkey_str);
ZEND_NAMED_FUNCTION(_wrap_ashkey_num);
ZEND_NAMED_FUNCTION(_wrap_shkey_cmp);
ZEND_NAMED_FUNCTION(_wrap_shkey_clone);
ZEND_NAMED_FUNCTION(_wrap_shkey_cert);
ZEND_NAMED_FUNCTION(_wrap_shkey_verify);
ZEND_NAMED_FUNCTION(_wrap_shkey_gen);
ZEND_NAMED_FUNCTION(_wrap_shpool_init);
ZEND_NAMED_FUNCTION(_wrap_shpool_size);
ZEND_NAMED_FUNCTION(_wrap_shpool_grow);
ZEND_NAMED_FUNCTION(_wrap_shpool_get);
ZEND_NAMED_FUNCTION(_wrap_shpool_get_index);
ZEND_NAMED_FUNCTION(_wrap_shpool_put);
ZEND_NAMED_FUNCTION(_wrap_shpool_free);
ZEND_NAMED_FUNCTION(_wrap_shmeta_init);
ZEND_NAMED_FUNCTION(_wrap_shmeta_free);
ZEND_NAMED_FUNCTION(_wrap_shmeta_set);
ZEND_NAMED_FUNCTION(_wrap_shmeta_set_str);
ZEND_NAMED_FUNCTION(_wrap_shmeta_unset_str);
ZEND_NAMED_FUNCTION(_wrap_shmeta_set_void);
ZEND_NAMED_FUNCTION(_wrap_shmeta_unset_void);
ZEND_NAMED_FUNCTION(_wrap_shmeta_get_str);
ZEND_NAMED_FUNCTION(_wrap_shmeta_get_void);
ZEND_NAMED_FUNCTION(_wrap_shmeta_get);
ZEND_NAMED_FUNCTION(_wrap_shmeta_print);
ZEND_NAMED_FUNCTION(_wrap_shbuf_append);
ZEND_NAMED_FUNCTION(_wrap_shbuf_clone);
ZEND_NAMED_FUNCTION(_wrap_shbuf_sprintf);
ZEND_NAMED_FUNCTION(_wrap_shmeta_count);
ZEND_NAMED_FUNCTION(_wrap_shmeta_unset_ptr);
ZEND_NAMED_FUNCTION(_wrap_shmeta_set_ptr);
ZEND_NAMED_FUNCTION(_wrap_shmeta_get_ptr);
ZEND_NAMED_FUNCTION(_wrap_ashencode);
ZEND_NAMED_FUNCTION(_wrap_shencode);
ZEND_NAMED_FUNCTION(_wrap_shencode_str);
ZEND_NAMED_FUNCTION(_wrap_ashdecode);
ZEND_NAMED_FUNCTION(_wrap_shdecode);
ZEND_NAMED_FUNCTION(_wrap_shdecode_str);
ZEND_NAMED_FUNCTION(_wrap_shlock_open);
ZEND_NAMED_FUNCTION(_wrap_shlock_tryopen);
ZEND_NAMED_FUNCTION(_wrap_shlock_close);
ZEND_NAMED_FUNCTION(_wrap__sh_sha256_init);
ZEND_NAMED_FUNCTION(_wrap__sh_sha256_update);
ZEND_NAMED_FUNCTION(_wrap__sh_sha256_final);
ZEND_NAMED_FUNCTION(_wrap_sh_sha256);
ZEND_NAMED_FUNCTION(_wrap_shdigest);
ZEND_NAMED_FUNCTION(_wrap_shjson_print);
ZEND_NAMED_FUNCTION(_wrap_shjson_str);
ZEND_NAMED_FUNCTION(_wrap_shjson_astr);
ZEND_NAMED_FUNCTION(_wrap_shjson_str_add);
ZEND_NAMED_FUNCTION(_wrap_shjson_free);
ZEND_NAMED_FUNCTION(_wrap_shjson_num);
ZEND_NAMED_FUNCTION(_wrap_shjson_num_add);
ZEND_NAMED_FUNCTION(_wrap_shjson_init);
ZEND_NAMED_FUNCTION(_wrap_shjson_array_add);
ZEND_NAMED_FUNCTION(_wrap_shjson_array_str);
ZEND_NAMED_FUNCTION(_wrap_shjson_array_astr);
ZEND_NAMED_FUNCTION(_wrap_shjson_array_num);
ZEND_NAMED_FUNCTION(_wrap_shjson_obj);
ZEND_NAMED_FUNCTION(_wrap_shjson_strlen);
#endif /* PHP_SHARE_PHP_H */
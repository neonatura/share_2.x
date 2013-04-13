<?php

/* ----------------------------------------------------------------------------
 * This file was automatically generated by SWIG (http://www.swig.org).
 * Version 2.0.9
 * 
 * This file is not intended to be easily readable and contains a number of 
 * coding conventions designed to improve portability and efficiency. Do not make
 * changes to this file unless you know what you are doing--modify the SWIG 
 * interface file instead. 
 * ----------------------------------------------------------------------------- */

// Try to load our extension if it's not already loaded.
if (!extension_loaded('phpshare')) {
  if (strtolower(substr(PHP_OS, 0, 3)) === 'win') {
    if (!dl('php_phpshare.dll')) return;
  } else {
    // PHP_SHLIB_SUFFIX gives 'dylib' on MacOS X but modules are 'so'.
    if (PHP_SHLIB_SUFFIX === 'dylib') {
      if (!dl('phpshare.so')) return;
    } else {
      if (!dl('phpshare.'.PHP_SHLIB_SUFFIX)) return;
    }
  }
}



abstract class phpshare {
	static function test_main() {
		test_main();
	}

	static function shbuf_init() {
		return shbuf_init();
	}

	static function shbuf_catstr($buf,$data) {
		shbuf_catstr($buf,$data);
	}

	static function shbuf_cat($buf,$data,$data_len) {
		shbuf_cat($buf,$data,$data_len);
	}

	static function shbuf_size($buf) {
		return shbuf_size($buf);
	}

	static function shbuf_clear($buf) {
		shbuf_clear($buf);
	}

	static function shbuf_trim($buf,$len) {
		shbuf_trim($buf,$len);
	}

	static function shbuf_free($buf_p) {
		shbuf_free($buf_p);
	}

	static function shtime() {
		return shtime();
	}

	static function shtime64() {
		return shtime64();
	}

	static function shfs_dir_base($tree) {
		return shfs_dir_base($tree);
	}

	static function shfs_dir_cwd($tree) {
		return shfs_dir_cwd($tree);
	}

	static function shfs_dir_parent($inode) {
		return shfs_dir_parent($inode);
	}

	static function shfs_dir_entry($inode,$fname) {
		return shfs_dir_entry($inode,$fname);
	}

	static function shfs_app_name($app_name) {
		return shfs_app_name($app_name);
	}

	static function shfs_inode($parent,$name,$mode) {
		return shfs_inode($parent,$name,$mode);
	}

	static function shfs_inode_tree($inode) {
		return shfs_inode_tree($inode);
	}

	static function shfs_inode_root($inode) {
		return shfs_inode_root($inode);
	}

	static function shfs_inode_parent($inode) {
		return shfs_inode_parent($inode);
	}

	static function shfs_inode_write_entity($tree,$ent) {
		return shfs_inode_write_entity($tree,$ent);
	}

	static function shfs_inode_write_block($tree,$scan_hdr,$hdr,$data,$data_len) {
		return shfs_inode_write_block($tree,$scan_hdr,$hdr,$data,$data_len);
	}

	static function shfs_inode_write($tree,$inode,$data,$data_of,$data_len) {
		return shfs_inode_write($tree,$inode,$data,$data_of,$data_len);
	}

	static function shfs_inode_read_block($tree,$hdr,$inode) {
		return shfs_inode_read_block($tree,$hdr,$inode);
	}

	static function shfs_inode_read($tree,$inode,$ret_buff,$data_of,$data_len) {
		return shfs_inode_read($tree,$inode,$ret_buff,$data_of,$data_len);
	}

	static function shfs_inode_filename($name) {
		return shfs_inode_filename($name);
	}

	static function shfs_inode_filename_set($inode,$name) {
		shfs_inode_filename_set($inode,$name);
	}

	static function shfs_inode_name($inode) {
		return shfs_inode_name($inode);
	}

	static function shfs_inode_path($inode) {
		return shfs_inode_path($inode);
	}

	static function shfs_journal_tree($jrnl) {
		return shfs_journal_tree($jrnl);
	}

	static function shfs_journal_path($tree,$index) {
		return shfs_journal_path($tree,$index);
	}

	static function shfs_journal_open($tree,$index) {
		return shfs_journal_open($tree,$index);
	}

	static function shfs_journal_free($tree,$jrnl_p) {
		shfs_journal_free($tree,$jrnl_p);
	}

	static function shfs_journal_index($inode) {
		return shfs_journal_index($inode);
	}

	static function shfs_journal_scan($tree,$jno) {
		return shfs_journal_scan($tree,$jno);
	}

	static function shfs_journal_write($jrnl) {
		return shfs_journal_write($jrnl);
	}

	static function shfs_journal_grow($jrnl_p) {
		return shfs_journal_grow($jrnl_p);
	}

	static function shfs_journal_close($jrnl_p) {
		return shfs_journal_close($jrnl_p);
	}

	static function shfs_meta($tree,$ent,$val_p) {
		return shfs_meta($tree,$ent,$val_p);
	}

	static function shfs_meta_save($tree,$ent,$h) {
		return shfs_meta_save($tree,$ent,$h);
	}

	static function shfs_init($app_name,$flags) {
		return shfs_init($app_name,$flags);
	}

	static function shfs_free($tree_p) {
		shfs_free($tree_p);
	}

	static function shfs_partition_id($tree) {
		return shfs_partition_id($tree);
	}

	static function shfs_proc_lock($process_path,$runtime_mode) {
		return shfs_proc_lock($process_path,$runtime_mode);
	}

	static function shfs_read_mem($path,$data_p,$data_len_p) {
		return shfs_read_mem($path,$data_p,$data_len_p);
	}

	static function shfs_write_mem($path,$data,$data_len) {
		return shfs_write_mem($path,$data,$data_len);
	}

	static function shfs_write_print($tree,$inode,$fd) {
		return shfs_write_print($tree,$inode,$fd);
	}

	static function shkey_bin($data,$data_len) {
		return shkey_bin($data,$data_len);
	}

	static function shkey_str($kvalue) {
		return shkey_str($kvalue);
	}

	static function shkey_num($kvalue) {
		return shkey_num($kvalue);
	}

	static function shkey_uniq() {
		return shkey_uniq();
	}

	static function shkey_free($key_p) {
		shkey_free($key_p);
	}

	static function shkey_print($key) {
		return shkey_print($key);
	}

	static function shmeta_init() {
		return shmeta_init();
	}

	static function shmeta_free($meta_p) {
		shmeta_free($meta_p);
	}

	static function shmetafunc_default($char_key,$klen_p) {
		return shmetafunc_default($char_key,$klen_p);
	}

	static function shmeta_set($ht,$key,$val) {
		shmeta_set($ht,$key,$val);
	}

	static function shmeta_set_str($h,$key,$value) {
		shmeta_set_str($h,$key,$value);
	}

	static function shmeta_unset_str($h,$name) {
		shmeta_unset_str($h,$name);
	}

	static function shmeta_set_void($ht,$key,$data,$data_len) {
		shmeta_set_void($ht,$key,$data,$data_len);
	}

	static function shmeta_unset_void($h,$key) {
		shmeta_unset_void($h,$key);
	}

	static function shmeta_get_str($h,$key) {
		return shmeta_get_str($h,$key);
	}

	static function shmeta_get_void($h,$key) {
		return shmeta_get_void($h,$key);
	}

	static function shmeta_get($ht,$key) {
		return shmeta_get($ht,$key);
	}

	static function shmeta_print($h,$ret_buff) {
		shmeta_print($h,$ret_buff);
	}

	static function shlock_open($key,$flags) {
		return shlock_open($key,$flags);
	}

	static function shlock_tryopen($key,$flags,$lock_p) {
		return shlock_tryopen($key,$flags,$lock_p);
	}

	static function shlock_close($key) {
		return shlock_close($key);
	}

	static function shlock_destroy() {
		shlock_destroy();
	}

	static function shencode($data,$data_len,$data_p,$data_len_p,$key) {
		return shencode($data,$data_len,$data_p,$data_len_p,$key);
	}

	static function shencode_str($data) {
		return shencode_str($data);
	}

	static function shdecode($data,$data_len,$data_p,$data_len_p,$key) {
		return shdecode($data,$data_len,$data_p,$data_len_p,$key);
	}

	static function shdecode_str($data,$key) {
		return shdecode_str($data,$key);
	}

	static function shkey_cmp($key_1,$key_2) {
		return shkey_cmp($key_1,$key_2);
	}

	static function shpool_init() {
		return shpool_init();
	}

	static function shpool_size($pool) {
		return shpool_size($pool);
	}

	static function shpool_grow($pool) {
		shpool_grow($pool);
	}

	static function shpool_get($pool) {
		return shpool_get($pool);
	}

	static function shpool_get_index($pool,$index) {
		return shpool_get_index($pool,$index);
	}

	static function shpool_put($pool,$buff) {
		shpool_put($pool,$buff);
	}

	static function shpool_free($pool_p) {
		shpool_free($pool_p);
	}

	static function shnet_accept($sockfd) {
		return shnet_accept($sockfd);
	}

	static function shnet_bindsk($sockfd,$hostname,$port) {
		return shnet_bindsk($sockfd,$hostname,$port);
	}

	static function shnet_bind($sockfd,$addr,$addrlen) {
		return shnet_bind($sockfd,$addr,$addrlen);
	}

	static function shnet_close($sk) {
		return shnet_close($sk);
	}

	static function shnet_conn($sk,$host,$port,$async) {
		return shnet_conn($sk,$host,$port,$async);
	}

	static function shnet_fcntl($fd,$cmd,$arg) {
		return shnet_fcntl($fd,$cmd,$arg);
	}

	static function shnet_gethostbyname($name) {
		return shnet_gethostbyname($name);
	}

	static function shnet_peer($name) {
		return shnet_peer($name);
	}

	static function shnet_read($fd,$buf,$count) {
		return shnet_read($fd,$buf,$count);
	}

	static function shnet_verify($readfds,$writefds,$millis) {
		return shnet_verify($readfds,$writefds,$millis);
	}

	static function shnet_select($nfds,$readfds,$writefds,$exceptfds,$timeout) {
		return shnet_select($nfds,$readfds,$writefds,$exceptfds,$timeout);
	}

	static function shnet_sk() {
		return shnet_sk();
	}

	static function shnet_socket($domain,$type,$protocol) {
		return shnet_socket($domain,$type,$protocol);
	}

	static function shnet_write($fd,$buf,$count) {
		return shnet_write($fd,$buf,$count);
	}
}

/* PHP Proxy Classes */

?>
/* ----------------------------------------------------------------------------
 * This file was automatically generated by SWIG (http://www.swig.org).
 * Version 2.0.9
 *
 * Do not make changes to this file unless you know what you are doing--modify
 * the SWIG interface file instead.
 * ----------------------------------------------------------------------------- */

package libshare;

public class jshare {
  public static void test_main() {
    jshareJNI.test_main();
  }

  public static SWIGTYPE_p_shbuf_t shbuf_init() {
    long cPtr = jshareJNI.shbuf_init();
    return (cPtr == 0) ? null : new SWIGTYPE_p_shbuf_t(cPtr, false);
  }

  public static void shbuf_catstr(SWIGTYPE_p_shbuf_t buf, String data) {
    jshareJNI.shbuf_catstr(SWIGTYPE_p_shbuf_t.getCPtr(buf), data);
  }

  public static void shbuf_cat(SWIGTYPE_p_shbuf_t buf, SWIGTYPE_p_void data, long data_len) {
    jshareJNI.shbuf_cat(SWIGTYPE_p_shbuf_t.getCPtr(buf), SWIGTYPE_p_void.getCPtr(data), data_len);
  }

  public static long shbuf_size(SWIGTYPE_p_shbuf_t buf) {
    return jshareJNI.shbuf_size(SWIGTYPE_p_shbuf_t.getCPtr(buf));
  }

  public static void shbuf_clear(SWIGTYPE_p_shbuf_t buf) {
    jshareJNI.shbuf_clear(SWIGTYPE_p_shbuf_t.getCPtr(buf));
  }

  public static void shbuf_trim(SWIGTYPE_p_shbuf_t buf, long len) {
    jshareJNI.shbuf_trim(SWIGTYPE_p_shbuf_t.getCPtr(buf), len);
  }

  public static void shbuf_free(SWIGTYPE_p_p_shbuf_t buf_p) {
    jshareJNI.shbuf_free(SWIGTYPE_p_p_shbuf_t.getCPtr(buf_p));
  }

  public static double shtime() {
    return jshareJNI.shtime();
  }

  public static SWIGTYPE_p_shtime_t shtime64() {
    return new SWIGTYPE_p_shtime_t(jshareJNI.shtime64(), true);
  }

  public static SWIGTYPE_p_shkey_t shkey_bin(String data, long data_len) {
    long cPtr = jshareJNI.shkey_bin(data, data_len);
    return (cPtr == 0) ? null : new SWIGTYPE_p_shkey_t(cPtr, false);
  }

  public static SWIGTYPE_p_shkey_t shkey_str(String kvalue) {
    long cPtr = jshareJNI.shkey_str(kvalue);
    return (cPtr == 0) ? null : new SWIGTYPE_p_shkey_t(cPtr, false);
  }

  public static SWIGTYPE_p_shkey_t shkey_num(int kvalue) {
    long cPtr = jshareJNI.shkey_num(kvalue);
    return (cPtr == 0) ? null : new SWIGTYPE_p_shkey_t(cPtr, false);
  }

  public static SWIGTYPE_p_shkey_t shkey_uniq() {
    long cPtr = jshareJNI.shkey_uniq();
    return (cPtr == 0) ? null : new SWIGTYPE_p_shkey_t(cPtr, false);
  }

  public static void shkey_free(SWIGTYPE_p_p_shkey_t key_p) {
    jshareJNI.shkey_free(SWIGTYPE_p_p_shkey_t.getCPtr(key_p));
  }

  public static String shkey_print(SWIGTYPE_p_shkey_t key) {
    return jshareJNI.shkey_print(SWIGTYPE_p_shkey_t.getCPtr(key));
  }

  public static SWIGTYPE_p_shmeta_t shmeta_init() {
    long cPtr = jshareJNI.shmeta_init();
    return (cPtr == 0) ? null : new SWIGTYPE_p_shmeta_t(cPtr, false);
  }

  public static void shmeta_free(SWIGTYPE_p_p_shmeta_t meta_p) {
    jshareJNI.shmeta_free(SWIGTYPE_p_p_shmeta_t.getCPtr(meta_p));
  }

  public static long shmetafunc_default(String char_key, SWIGTYPE_p_ssize_t klen_p) {
    return jshareJNI.shmetafunc_default(char_key, SWIGTYPE_p_ssize_t.getCPtr(klen_p));
  }

  public static void shmeta_set(SWIGTYPE_p_shmeta_t ht, SWIGTYPE_p_shkey_t key, SWIGTYPE_p_void val) {
    jshareJNI.shmeta_set(SWIGTYPE_p_shmeta_t.getCPtr(ht), SWIGTYPE_p_shkey_t.getCPtr(key), SWIGTYPE_p_void.getCPtr(val));
  }

  public static void shmeta_set_str(SWIGTYPE_p_shmeta_t h, SWIGTYPE_p_shkey_t key, String value) {
    jshareJNI.shmeta_set_str(SWIGTYPE_p_shmeta_t.getCPtr(h), SWIGTYPE_p_shkey_t.getCPtr(key), value);
  }

  public static void shmeta_unset_str(SWIGTYPE_p_shmeta_t h, SWIGTYPE_p_shkey_t name) {
    jshareJNI.shmeta_unset_str(SWIGTYPE_p_shmeta_t.getCPtr(h), SWIGTYPE_p_shkey_t.getCPtr(name));
  }

  public static void shmeta_set_void(SWIGTYPE_p_shmeta_t ht, SWIGTYPE_p_shkey_t key, SWIGTYPE_p_void data, long data_len) {
    jshareJNI.shmeta_set_void(SWIGTYPE_p_shmeta_t.getCPtr(ht), SWIGTYPE_p_shkey_t.getCPtr(key), SWIGTYPE_p_void.getCPtr(data), data_len);
  }

  public static void shmeta_unset_void(SWIGTYPE_p_shmeta_t h, SWIGTYPE_p_shkey_t key) {
    jshareJNI.shmeta_unset_void(SWIGTYPE_p_shmeta_t.getCPtr(h), SWIGTYPE_p_shkey_t.getCPtr(key));
  }

  public static String shmeta_get_str(SWIGTYPE_p_shmeta_t h, SWIGTYPE_p_shkey_t key) {
    return jshareJNI.shmeta_get_str(SWIGTYPE_p_shmeta_t.getCPtr(h), SWIGTYPE_p_shkey_t.getCPtr(key));
  }

  public static SWIGTYPE_p_void shmeta_get_void(SWIGTYPE_p_shmeta_t h, SWIGTYPE_p_shkey_t key) {
    long cPtr = jshareJNI.shmeta_get_void(SWIGTYPE_p_shmeta_t.getCPtr(h), SWIGTYPE_p_shkey_t.getCPtr(key));
    return (cPtr == 0) ? null : new SWIGTYPE_p_void(cPtr, false);
  }

  public static SWIGTYPE_p_void shmeta_get(SWIGTYPE_p_shmeta_t ht, SWIGTYPE_p_shkey_t key) {
    long cPtr = jshareJNI.shmeta_get(SWIGTYPE_p_shmeta_t.getCPtr(ht), SWIGTYPE_p_shkey_t.getCPtr(key));
    return (cPtr == 0) ? null : new SWIGTYPE_p_void(cPtr, false);
  }

  public static void shmeta_print(SWIGTYPE_p_shmeta_t h, SWIGTYPE_p_shbuf_t ret_buff) {
    jshareJNI.shmeta_print(SWIGTYPE_p_shmeta_t.getCPtr(h), SWIGTYPE_p_shbuf_t.getCPtr(ret_buff));
  }

  public static SWIGTYPE_p_shlock_t shlock_open(SWIGTYPE_p_shkey_t key, int flags) {
    long cPtr = jshareJNI.shlock_open(SWIGTYPE_p_shkey_t.getCPtr(key), flags);
    return (cPtr == 0) ? null : new SWIGTYPE_p_shlock_t(cPtr, false);
  }

  public static int shlock_tryopen(SWIGTYPE_p_shkey_t key, int flags, SWIGTYPE_p_p_shlock_t lock_p) {
    return jshareJNI.shlock_tryopen(SWIGTYPE_p_shkey_t.getCPtr(key), flags, SWIGTYPE_p_p_shlock_t.getCPtr(lock_p));
  }

  public static int shlock_close(SWIGTYPE_p_shkey_t key) {
    return jshareJNI.shlock_close(SWIGTYPE_p_shkey_t.getCPtr(key));
  }

  public static void shlock_destroy() {
    jshareJNI.shlock_destroy();
  }

  public static int shencode(String data, long data_len, SWIGTYPE_p_p_uint8_t data_p, SWIGTYPE_p_uint32_t data_len_p, SWIGTYPE_p_shkey_t key) {
    return jshareJNI.shencode(data, data_len, SWIGTYPE_p_p_uint8_t.getCPtr(data_p), SWIGTYPE_p_uint32_t.getCPtr(data_len_p), SWIGTYPE_p_shkey_t.getCPtr(key));
  }

  public static SWIGTYPE_p_shkey_t shencode_str(String data) {
    long cPtr = jshareJNI.shencode_str(data);
    return (cPtr == 0) ? null : new SWIGTYPE_p_shkey_t(cPtr, false);
  }

  public static int shdecode(SWIGTYPE_p_uint8_t data, SWIGTYPE_p_uint32_t data_len, SWIGTYPE_p_p_char data_p, SWIGTYPE_p_size_t data_len_p, SWIGTYPE_p_shkey_t key) {
    return jshareJNI.shdecode(SWIGTYPE_p_uint8_t.getCPtr(data), SWIGTYPE_p_uint32_t.getCPtr(data_len), SWIGTYPE_p_p_char.getCPtr(data_p), SWIGTYPE_p_size_t.getCPtr(data_len_p), SWIGTYPE_p_shkey_t.getCPtr(key));
  }

  public static int shdecode_str(String data, SWIGTYPE_p_shkey_t key) {
    return jshareJNI.shdecode_str(data, SWIGTYPE_p_shkey_t.getCPtr(key));
  }

  public static int shkey_cmp(SWIGTYPE_p_shkey_t key_1, SWIGTYPE_p_shkey_t key_2) {
    return jshareJNI.shkey_cmp(SWIGTYPE_p_shkey_t.getCPtr(key_1), SWIGTYPE_p_shkey_t.getCPtr(key_2));
  }

  public static SWIGTYPE_p_shpool_t shpool_init() {
    long cPtr = jshareJNI.shpool_init();
    return (cPtr == 0) ? null : new SWIGTYPE_p_shpool_t(cPtr, false);
  }

  public static long shpool_size(SWIGTYPE_p_shpool_t pool) {
    return jshareJNI.shpool_size(SWIGTYPE_p_shpool_t.getCPtr(pool));
  }

  public static void shpool_grow(SWIGTYPE_p_shpool_t pool) {
    jshareJNI.shpool_grow(SWIGTYPE_p_shpool_t.getCPtr(pool));
  }

  public static SWIGTYPE_p_shbuf_t shpool_get(SWIGTYPE_p_shpool_t pool) {
    long cPtr = jshareJNI.shpool_get(SWIGTYPE_p_shpool_t.getCPtr(pool));
    return (cPtr == 0) ? null : new SWIGTYPE_p_shbuf_t(cPtr, false);
  }

  public static SWIGTYPE_p_shbuf_t shpool_get_index(SWIGTYPE_p_shpool_t pool, int index) {
    long cPtr = jshareJNI.shpool_get_index(SWIGTYPE_p_shpool_t.getCPtr(pool), index);
    return (cPtr == 0) ? null : new SWIGTYPE_p_shbuf_t(cPtr, false);
  }

  public static void shpool_put(SWIGTYPE_p_shpool_t pool, SWIGTYPE_p_shbuf_t buff) {
    jshareJNI.shpool_put(SWIGTYPE_p_shpool_t.getCPtr(pool), SWIGTYPE_p_shbuf_t.getCPtr(buff));
  }

  public static void shpool_free(SWIGTYPE_p_p_shpool_t pool_p) {
    jshareJNI.shpool_free(SWIGTYPE_p_p_shpool_t.getCPtr(pool_p));
  }

  public static int shnet_accept(int sockfd) {
    return jshareJNI.shnet_accept(sockfd);
  }

  public static int shnet_bindsk(int sockfd, String hostname, long port) {
    return jshareJNI.shnet_bindsk(sockfd, hostname, port);
  }

  public static int shnet_bind(int sockfd, SWIGTYPE_p_sockaddr addr, SWIGTYPE_p_socklen_t addrlen) {
    return jshareJNI.shnet_bind(sockfd, SWIGTYPE_p_sockaddr.getCPtr(addr), SWIGTYPE_p_socklen_t.getCPtr(addrlen));
  }

  public static int shnet_close(int sk) {
    return jshareJNI.shnet_close(sk);
  }

  public static int shnet_conn(int sk, String host, int port, int async) {
    return jshareJNI.shnet_conn(sk, host, port, async);
  }

  public static int shnet_fcntl(int fd, int cmd, int arg) {
    return jshareJNI.shnet_fcntl(fd, cmd, arg);
  }

  public static SWIGTYPE_p_hostent shnet_gethostbyname(String name) {
    long cPtr = jshareJNI.shnet_gethostbyname(name);
    return (cPtr == 0) ? null : new SWIGTYPE_p_hostent(cPtr, false);
  }

  public static SWIGTYPE_p_hostent shnet_peer(String name) {
    long cPtr = jshareJNI.shnet_peer(name);
    return (cPtr == 0) ? null : new SWIGTYPE_p_hostent(cPtr, false);
  }

  public static SWIGTYPE_p_ssize_t shnet_read(int fd, SWIGTYPE_p_void buf, long count) {
    return new SWIGTYPE_p_ssize_t(jshareJNI.shnet_read(fd, SWIGTYPE_p_void.getCPtr(buf), count), true);
  }

  public static int shnet_verify(SWIGTYPE_p_fd_set readfds, SWIGTYPE_p_fd_set writefds, SWIGTYPE_p_long millis) {
    return jshareJNI.shnet_verify(SWIGTYPE_p_fd_set.getCPtr(readfds), SWIGTYPE_p_fd_set.getCPtr(writefds), SWIGTYPE_p_long.getCPtr(millis));
  }

  public static int shnet_select(int nfds, SWIGTYPE_p_fd_set readfds, SWIGTYPE_p_fd_set writefds, SWIGTYPE_p_fd_set exceptfds, SWIGTYPE_p_timeval timeout) {
    return jshareJNI.shnet_select(nfds, SWIGTYPE_p_fd_set.getCPtr(readfds), SWIGTYPE_p_fd_set.getCPtr(writefds), SWIGTYPE_p_fd_set.getCPtr(exceptfds), SWIGTYPE_p_timeval.getCPtr(timeout));
  }

  public static int shnet_sk() {
    return jshareJNI.shnet_sk();
  }

  public static int shnet_socket(int domain, int type, int protocol) {
    return jshareJNI.shnet_socket(domain, type, protocol);
  }

  public static SWIGTYPE_p_ssize_t shnet_write(int fd, SWIGTYPE_p_void buf, long count) {
    return new SWIGTYPE_p_ssize_t(jshareJNI.shnet_write(fd, SWIGTYPE_p_void.getCPtr(buf), count), true);
  }

}
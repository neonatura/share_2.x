<div class="navigation" id="top">
  <div class="tabs">
    <ul>
      <li>[[Home]]</li>
      <li>[[Modules]]</li>
      <li>[[Structures]]</li>
      <li>[[Files]]</li>
      <li>[[Examples]]</li>
    </ul>
  </div>
</div>
<div class="contents">
<h1>Libshare</h1>
<p>Utility functions to generate unique checksums of data.  
<a href="#_details">More...</a></p>
<table border="0" cellpadding="0" cellspacing="0">
<tr><td colspan="2"><h2>Data Structures</h2></td></tr>
<tr><td class="memItemLeft" align="right" valign="top">struct &nbsp;</td><td class="memItemRight" valign="bottom"><a class="el" href="structshpeer__t.html">shpeer_t</a></td></tr>
<tr><td class="mdescLeft">&nbsp;</td><td class="mdescRight">The local or remote machine associated with the sharefs partition.  <a href="structshpeer__t.html#_details">More...</a><br/></td></tr>
<tr><td class="memItemLeft" align="right" valign="top">struct &nbsp;</td><td class="memItemRight" valign="bottom"><a class="el" href="structshbuf__t.html">shbuf_t</a></td></tr>
<tr><td class="mdescLeft">&nbsp;</td><td class="mdescRight">A memory buffer.  <a href="structshbuf__t.html#_details">More...</a><br/></td></tr>
<tr><td colspan="2"><h2>Modules</h2></td></tr>
<tr><td class="memItemLeft" align="right" valign="top">&nbsp;</td><td class="memItemRight" valign="bottom"><a class="el" href="group__libshare__fs.html">The 'sharefs' file-system.</a></td></tr>

<p><tr><td class="mdescLeft">&nbsp;</td><td class="mdescRight"><p>The sharefs file system overlays ontop of your current filesystem in order to provide extended file operations. </p>
<br/></td></tr>
</p>
<tr><td class="memItemLeft" align="right" valign="top">&nbsp;</td><td class="memItemRight" valign="bottom"><a class="el" href="group__libshare__meta.html">Meta definition hash maps (meta maps).</a></td></tr>

<p><tr><td class="mdescLeft">&nbsp;</td><td class="mdescRight"><p>A meta definition is part of a <code><a class="el" href="structshmeta__t.html" title="The size of the array is always a power of two.">shmeta_t</a></code> hashmap. </p>
<br/></td></tr>
</p>
<tr><td class="memItemLeft" align="right" valign="top">&nbsp;</td><td class="memItemRight" valign="bottom"><a class="el" href="group__libshare__socket.html">Network socket operations.</a></td></tr>

<p><tr><td class="mdescLeft">&nbsp;</td><td class="mdescRight"><p>The libshare Socket Handling provides access to regular socket operations with posix or convienence functions in addition to access to the ESP network protocol. </p>
<br/></td></tr>
</p>
<tr><td colspan="2"><h2>Defines</h2></td></tr>
<tr><td class="memItemLeft" align="right" valign="top"><a class="anchor" id="ga48bed91de5526aa96e1024c9d0eac8df"></a><!-- doxytag: member="libshare::SHSK_PEER_LOCAL" ref="ga48bed91de5526aa96e1024c9d0eac8df" args="" -->
#define&nbsp;</td><td class="memItemRight" valign="bottom"><a class="el" href="group__libshare.html#ga48bed91de5526aa96e1024c9d0eac8df">SHSK_PEER_LOCAL</a>&nbsp;&nbsp;&nbsp;0</td></tr>
<tr><td class="mdescLeft">&nbsp;</td><td class="mdescRight">The local machine. <br/></td></tr>
<tr><td class="memItemLeft" align="right" valign="top"><a class="anchor" id="gacd98c2c9d52bd8710e245cda62e1505c"></a><!-- doxytag: member="libshare::SHSK_PEER_IPV4" ref="gacd98c2c9d52bd8710e245cda62e1505c" args="" -->
#define&nbsp;</td><td class="memItemRight" valign="bottom"><a class="el" href="group__libshare.html#gacd98c2c9d52bd8710e245cda62e1505c">SHSK_PEER_IPV4</a>&nbsp;&nbsp;&nbsp;1</td></tr>
<tr><td class="mdescLeft">&nbsp;</td><td class="mdescRight">A remote IPv4 network destination. <br/></td></tr>
<tr><td class="memItemLeft" align="right" valign="top"><a class="anchor" id="ga4e00d7a3c6696acb7fb010eb71b2af02"></a><!-- doxytag: member="libshare::SHSK_PEER_IPV6" ref="ga4e00d7a3c6696acb7fb010eb71b2af02" args="" -->
#define&nbsp;</td><td class="memItemRight" valign="bottom"><a class="el" href="group__libshare.html#ga4e00d7a3c6696acb7fb010eb71b2af02">SHSK_PEER_IPV6</a>&nbsp;&nbsp;&nbsp;2</td></tr>
<tr><td class="mdescLeft">&nbsp;</td><td class="mdescRight">A remote IPv6 network destination. <br/></td></tr>
<tr><td class="memItemLeft" align="right" valign="top"><a class="anchor" id="ga20cf6d21450c2a6ea79e594e0cb4b7d9"></a><!-- doxytag: member="libshare::SHSK_PEER_VPN_IPV4" ref="ga20cf6d21450c2a6ea79e594e0cb4b7d9" args="" -->
#define&nbsp;</td><td class="memItemRight" valign="bottom"><a class="el" href="group__libshare.html#ga20cf6d21450c2a6ea79e594e0cb4b7d9">SHSK_PEER_VPN_IPV4</a>&nbsp;&nbsp;&nbsp;3</td></tr>
<tr><td class="mdescLeft">&nbsp;</td><td class="mdescRight">A IPv4 network destination on the sharenet VPN. <br/></td></tr>
<tr><td class="memItemLeft" align="right" valign="top"><a class="anchor" id="ga97d907201807e39b840aa3b8c49ac69b"></a><!-- doxytag: member="libshare::SHSK_PEER_VPN_IPV6" ref="ga97d907201807e39b840aa3b8c49ac69b" args="" -->
#define&nbsp;</td><td class="memItemRight" valign="bottom"><a class="el" href="group__libshare.html#ga97d907201807e39b840aa3b8c49ac69b">SHSK_PEER_VPN_IPV6</a>&nbsp;&nbsp;&nbsp;4</td></tr>
<tr><td class="mdescLeft">&nbsp;</td><td class="mdescRight">A IPv6 network destination on the sharenet VPN. <br/></td></tr>
<tr><td class="memItemLeft" align="right" valign="top">#define&nbsp;</td><td class="memItemRight" valign="bottom"><a class="el" href="group__libshare.html#gac6057fb77e1ab69db07b0d4356248c90">SHPREF_BASE_DIR</a>&nbsp;&nbsp;&nbsp;&quot;base-dir&quot;</td></tr>
<tr><td class="mdescLeft">&nbsp;</td><td class="mdescRight">Specifies the preferred location of where the sharefs filesystem is stored on the local file system.  <a href="#gac6057fb77e1ab69db07b0d4356248c90"></a><br/></td></tr>
<tr><td class="memItemLeft" align="right" valign="top">#define&nbsp;</td><td class="memItemRight" valign="bottom"><a class="el" href="group__libshare.html#ga3a0fa502ca8598efb58c4b1aa2d56f27">SHPREF_TRACK</a>&nbsp;&nbsp;&nbsp;&quot;track&quot;</td></tr>
<tr><td class="mdescLeft">&nbsp;</td><td class="mdescRight">Specifies whether to track sharefs filesystem revisions.  <a href="#ga3a0fa502ca8598efb58c4b1aa2d56f27"></a><br/></td></tr>
<tr><td class="memItemLeft" align="right" valign="top">#define&nbsp;</td><td class="memItemRight" valign="bottom"><a class="el" href="group__libshare.html#ga649e519df79a6e54465e5e9f62da48bf">SHPREF_OVERLAY</a>&nbsp;&nbsp;&nbsp;&quot;overlay&quot;</td></tr>
<tr><td class="mdescLeft">&nbsp;</td><td class="mdescRight">Specifies whether the sharefs file system references files on the local filesystem.  <a href="#ga649e519df79a6e54465e5e9f62da48bf"></a><br/></td></tr>
<tr><td class="memItemLeft" align="right" valign="top"><a class="anchor" id="ga4ad827c123ddf4c83eb8c7699623f81b"></a><!-- doxytag: member="libshare::SHPREF_MAX" ref="ga4ad827c123ddf4c83eb8c7699623f81b" args="" -->
#define&nbsp;</td><td class="memItemRight" valign="bottom"><a class="el" href="group__libshare.html#ga4ad827c123ddf4c83eb8c7699623f81b">SHPREF_MAX</a>&nbsp;&nbsp;&nbsp;3</td></tr>
<tr><td class="mdescLeft">&nbsp;</td><td class="mdescRight">Specifies the number of preferences available. <br/></td></tr>
<tr><td class="memItemLeft" align="right" valign="top">#define&nbsp;</td><td class="memItemRight" valign="bottom"><a class="el" href="group__libshare.html#ga4973650c785959f9b4c2e8622ee8f2a1">shpref_track</a>()&nbsp;&nbsp;&nbsp;(0 == strcmp(shpref_get(SHPREF_TRACK), &quot;true&quot;) ? TRUE : FALSE)</td></tr>
<tr><td class="mdescLeft">&nbsp;</td><td class="mdescRight">Specifies whether to track sharefs filesystem revisions.  <a href="#ga4973650c785959f9b4c2e8622ee8f2a1"></a><br/></td></tr>
<tr><td class="memItemLeft" align="right" valign="top">#define&nbsp;</td><td class="memItemRight" valign="bottom"><a class="el" href="group__libshare.html#ga94f87ebcf4097eee2f109642bf02576c">shpref_set_track</a>(opt)&nbsp;&nbsp;&nbsp;(opt ? shpref_set(SHPREF_TRACK, &quot;true&quot;) : shpref_set(SHPREF_TRACK, &quot;false&quot;))</td></tr>
<tr><td class="mdescLeft">&nbsp;</td><td class="mdescRight">Permanently sets the <code>SHPREF_TRACK</code> option.  <a href="#ga94f87ebcf4097eee2f109642bf02576c"></a><br/></td></tr>
<tr><td class="memItemLeft" align="right" valign="top">#define&nbsp;</td><td class="memItemRight" valign="bottom"><a class="el" href="group__libshare.html#ga4e5d56369a63c1083b2830db6fc4ccdf">shpref_overlay</a>()&nbsp;&nbsp;&nbsp;(0 == strcmp(shpref_get(SHPREF_OVERLAY), &quot;true&quot;) ? TRUE : FALSE)</td></tr>
<tr><td class="mdescLeft">&nbsp;</td><td class="mdescRight">Specifies whether to overlay the sharefs filesystem ontop of the work directory on the local filesystem.  <a href="#ga4e5d56369a63c1083b2830db6fc4ccdf"></a><br/></td></tr>
<tr><td class="memItemLeft" align="right" valign="top">#define&nbsp;</td><td class="memItemRight" valign="bottom"><a class="el" href="group__libshare.html#ga0284584c0c5e92ff54e7f3dcf0ac0dcb">shpref_set_overlay</a>(opt)&nbsp;&nbsp;&nbsp;(opt ? shpref_set(SHPREF_OVERLAY, &quot;true&quot;) : shpref_set(SHPREF_OVERLAY, &quot;false&quot;))</td></tr>
<tr><td class="mdescLeft">&nbsp;</td><td class="mdescRight">Permanently sets the <code>SHPREF_OVERLAY</code> option.  <a href="#ga0284584c0c5e92ff54e7f3dcf0ac0dcb"></a><br/></td></tr>
<tr><td class="memItemLeft" align="right" valign="top"><a class="anchor" id="gaf3c16b4a0e4a5387f9933e854f43af2f"></a><!-- doxytag: member="libshare::shpref_unset" ref="gaf3c16b4a0e4a5387f9933e854f43af2f" args="(pref)" -->
#define&nbsp;</td><td class="memItemRight" valign="bottom"><a class="el" href="group__libshare.html#gaf3c16b4a0e4a5387f9933e854f43af2f">shpref_unset</a>(pref)&nbsp;&nbsp;&nbsp;shpref_set(pref, NULL)</td></tr>
<tr><td class="mdescLeft">&nbsp;</td><td class="mdescRight">Persistently unset a libshare configuration option. <br/></td></tr>
<tr><td class="memItemLeft" align="right" valign="top"><a class="anchor" id="gad814ced5efcb4f0cc806f9da0c9bd299"></a><!-- doxytag: member="libshare::shpref_sess_set" ref="gad814ced5efcb4f0cc806f9da0c9bd299" args="(pref, value)" -->
#define&nbsp;</td><td class="memItemRight" valign="bottom"><a class="el" href="group__libshare.html#gad814ced5efcb4f0cc806f9da0c9bd299">shpref_sess_set</a>(pref, value)&nbsp;&nbsp;&nbsp;shmeta_set(_pref, shkey_init_str(pref), value)</td></tr>
<tr><td class="mdescLeft">&nbsp;</td><td class="mdescRight">Overwrite a preference for the current session. <br/></td></tr>
<tr><td class="memItemLeft" align="right" valign="top"><a class="anchor" id="gaafb09ccf6c31da1df1a5e91d2fcc672d"></a><!-- doxytag: member="libshare::shpref_sess_unset" ref="gaafb09ccf6c31da1df1a5e91d2fcc672d" args="(pref)" -->
#define&nbsp;</td><td class="memItemRight" valign="bottom"><a class="el" href="group__libshare.html#gaafb09ccf6c31da1df1a5e91d2fcc672d">shpref_sess_unset</a>(pref)&nbsp;&nbsp;&nbsp;shpref_sess_set(pref, NULL)</td></tr>
<tr><td class="mdescLeft">&nbsp;</td><td class="mdescRight">Temporarily unset a libshare configuration option. <br/></td></tr>
<tr><td colspan="2"><h2>Typedefs</h2></td></tr>
<tr><td class="memItemLeft" align="right" valign="top">typedef uint64_t&nbsp;</td><td class="memItemRight" valign="bottom"><a class="el" href="group__libshare.html#ga8067b5b54c80844067174fba5203a7f9">shsize_t</a></td></tr>
<tr><td class="mdescLeft">&nbsp;</td><td class="mdescRight">A specification of byte size.  <a href="#ga8067b5b54c80844067174fba5203a7f9"></a><br/></td></tr>
<tr><td class="memItemLeft" align="right" valign="top"><a class="anchor" id="ga315bac56fce878a38a2d91ca49c4c374"></a><!-- doxytag: member="libshare::shkey_t" ref="ga315bac56fce878a38a2d91ca49c4c374" args="" -->
typedef uint64_t&nbsp;</td><td class="memItemRight" valign="bottom"><a class="el" href="group__libshare.html#ga315bac56fce878a38a2d91ca49c4c374">shkey_t</a></td></tr>
<tr><td class="mdescLeft">&nbsp;</td><td class="mdescRight">A key used to represent a hash code of an object. <br/></td></tr>
<tr><td class="memItemLeft" align="right" valign="top"><a class="anchor" id="gafde3ec4ad887dabfd0c44fb010c4cd26"></a><!-- doxytag: member="libshare::shtime_t" ref="gafde3ec4ad887dabfd0c44fb010c4cd26" args="" -->
typedef uint64_t&nbsp;</td><td class="memItemRight" valign="bottom"><a class="el" href="group__libshare.html#gafde3ec4ad887dabfd0c44fb010c4cd26">shtime_t</a></td></tr>
<tr><td class="mdescLeft">&nbsp;</td><td class="mdescRight">The libshare representation of a particular time. <br/></td></tr>
<tr><td colspan="2"><h2>Functions</h2></td></tr>
<tr><td class="memItemLeft" align="right" valign="top"><a class="anchor" id="gaac4780f757bd25d31aad817898096d71"></a><!-- doxytag: member="libshare::get_libshare_email" ref="gaac4780f757bd25d31aad817898096d71" args="(void)" -->
char *&nbsp;</td><td class="memItemRight" valign="bottom"><a class="el" href="group__libshare.html#gaac4780f757bd25d31aad817898096d71">get_libshare_email</a> (void)</td></tr>
<tr><td class="mdescLeft">&nbsp;</td><td class="mdescRight">An email address where bug reports can be submitted. <br/></td></tr>
<tr><td class="memItemLeft" align="right" valign="top"><a class="anchor" id="ga14d950f7cefce69f1d0d4efe4497a30c"></a><!-- doxytag: member="libshare::get_libshare_version" ref="ga14d950f7cefce69f1d0d4efe4497a30c" args="(void)" -->
char *&nbsp;</td><td class="memItemRight" valign="bottom"><a class="el" href="group__libshare.html#ga14d950f7cefce69f1d0d4efe4497a30c">get_libshare_version</a> (void)</td></tr>
<tr><td class="mdescLeft">&nbsp;</td><td class="mdescRight">The current libshare library version. <br/></td></tr>
<tr><td class="memItemLeft" align="right" valign="top"><a class="anchor" id="gaa17b46bea0fd510b1b0d59895775018b"></a><!-- doxytag: member="libshare::get_libshare_title" ref="gaa17b46bea0fd510b1b0d59895775018b" args="(void)" -->
char *&nbsp;</td><td class="memItemRight" valign="bottom"><a class="el" href="group__libshare.html#gaa17b46bea0fd510b1b0d59895775018b">get_libshare_title</a> (void)</td></tr>
<tr><td class="mdescLeft">&nbsp;</td><td class="mdescRight">The libshare library package name. <br/></td></tr>
<tr><td class="memItemLeft" align="right" valign="top"><a class="el" href="structshbuf__t.html">shbuf_t</a> *&nbsp;</td><td class="memItemRight" valign="bottom"><a class="el" href="group__libshare.html#gafac914ba170c0f2a9bc8c2add3a63a41">shbuf_init</a> (void)</td></tr>
<tr><td class="mdescLeft">&nbsp;</td><td class="mdescRight">Initialize a memory buffer for use.  <a href="#gafac914ba170c0f2a9bc8c2add3a63a41"></a><br/></td></tr>
<tr><td class="memItemLeft" align="right" valign="top"><a class="anchor" id="ga56d0f66573d2b73619ac9ee6550fda66"></a><!-- doxytag: member="libshare::shbuf_catstr" ref="ga56d0f66573d2b73619ac9ee6550fda66" args="(shbuf_t *buf, char *data)" -->
void&nbsp;</td><td class="memItemRight" valign="bottom"><a class="el" href="group__libshare.html#ga56d0f66573d2b73619ac9ee6550fda66">shbuf_catstr</a> (<a class="el" href="structshbuf__t.html">shbuf_t</a> *buf, char *data)</td></tr>
<tr><td class="mdescLeft">&nbsp;</td><td class="mdescRight">Inserts a string into a <code><a class="el" href="structshbuf__t.html" title="A memory buffer.">shbuf_t</a></code> memory pool. <br/></td></tr>
<tr><td class="memItemLeft" align="right" valign="top"><a class="anchor" id="ga2d5423cb4f2f81c682865273be9b13a4"></a><!-- doxytag: member="libshare::shbuf_cat" ref="ga2d5423cb4f2f81c682865273be9b13a4" args="(shbuf_t *buf, void *data, size_t data_len)" -->
void&nbsp;</td><td class="memItemRight" valign="bottom"><a class="el" href="group__libshare.html#ga2d5423cb4f2f81c682865273be9b13a4">shbuf_cat</a> (<a class="el" href="structshbuf__t.html">shbuf_t</a> *buf, void *data, size_t data_len)</td></tr>
<tr><td class="mdescLeft">&nbsp;</td><td class="mdescRight">Inserts a binary data segment into a <code><a class="el" href="structshbuf__t.html" title="A memory buffer.">shbuf_t</a></code> memory pool. <br/></td></tr>
<tr><td class="memItemLeft" align="right" valign="top"><a class="anchor" id="ga0d2961e1d67436bb57d628fc502d09f5"></a><!-- doxytag: member="libshare::shbuf_size" ref="ga0d2961e1d67436bb57d628fc502d09f5" args="(shbuf_t *buf)" -->
size_t&nbsp;</td><td class="memItemRight" valign="bottom"><a class="el" href="group__libshare.html#ga0d2961e1d67436bb57d628fc502d09f5">shbuf_size</a> (<a class="el" href="structshbuf__t.html">shbuf_t</a> *buf)</td></tr>
<tr><td class="mdescLeft">&nbsp;</td><td class="mdescRight">The current size of the data segement stored in the memory buffer. <br/></td></tr>
<tr><td class="memItemLeft" align="right" valign="top"><a class="anchor" id="ga095137d28832c6e172493dc67d994109"></a><!-- doxytag: member="libshare::shbuf_clear" ref="ga095137d28832c6e172493dc67d994109" args="(shbuf_t *buf)" -->
void&nbsp;</td><td class="memItemRight" valign="bottom"><a class="el" href="group__libshare.html#ga095137d28832c6e172493dc67d994109">shbuf_clear</a> (<a class="el" href="structshbuf__t.html">shbuf_t</a> *buf)</td></tr>
<tr><td class="mdescLeft">&nbsp;</td><td class="mdescRight">Clear the contents of a <code><a class="el" href="structshbuf__t.html" title="A memory buffer.">shbuf_t</a></code> libshare memory buffer. <br/></td></tr>
<tr><td class="memItemLeft" align="right" valign="top"><a class="anchor" id="ga8192e9e8db3fe89bcf93676230f693e9"></a><!-- doxytag: member="libshare::shbuf_free" ref="ga8192e9e8db3fe89bcf93676230f693e9" args="(shbuf_t **buf_p)" -->
void&nbsp;</td><td class="memItemRight" valign="bottom"><a class="el" href="group__libshare.html#ga8192e9e8db3fe89bcf93676230f693e9">shbuf_free</a> (<a class="el" href="structshbuf__t.html">shbuf_t</a> **buf_p)</td></tr>
<tr><td class="mdescLeft">&nbsp;</td><td class="mdescRight">Frees the resources utilizited by the memory buffer. <br/></td></tr>
<tr><td class="memItemLeft" align="right" valign="top"><a class="el" href="group__libshare.html#ga315bac56fce878a38a2d91ca49c4c374">shkey_t</a>&nbsp;</td><td class="memItemRight" valign="bottom"><a class="el" href="group__libshare.html#gad8822a98cbfbde7a9b87de94a83ba08d">shkey_init_str</a> (char *kvalue)</td></tr>
<tr><td class="mdescLeft">&nbsp;</td><td class="mdescRight">Create a <code>shkey_t</code> hashmap key reference from <code>kvalue</code> <em>kvalue</em> The string to generate into a <code>shkey_t</code>.  <a href="#gad8822a98cbfbde7a9b87de94a83ba08d"></a><br/></td></tr>
<tr><td class="memItemLeft" align="right" valign="top"><a class="el" href="group__libshare.html#ga315bac56fce878a38a2d91ca49c4c374">shkey_t</a>&nbsp;</td><td class="memItemRight" valign="bottom"><a class="el" href="group__libshare.html#ga5c7e8d31be37244ea41da5be6f24bc78">shkey_init_num</a> (long kvalue)</td></tr>
<tr><td class="mdescLeft">&nbsp;</td><td class="mdescRight">Create a <code>shkey_t</code> hashmap key reference from a number.  <a href="#ga5c7e8d31be37244ea41da5be6f24bc78"></a><br/></td></tr>
<tr><td class="memItemLeft" align="right" valign="top"><a class="el" href="group__libshare.html#ga315bac56fce878a38a2d91ca49c4c374">shkey_t</a>&nbsp;</td><td class="memItemRight" valign="bottom"><a class="el" href="group__libshare.html#ga23f802b9ef8431b59a416b540377ad8f">shkey_init_unique</a> (void)</td></tr>
<tr><td class="mdescLeft">&nbsp;</td><td class="mdescRight">Create a unique <code>shkey_t</code> hashmap key reference.  <a href="#ga23f802b9ef8431b59a416b540377ad8f"></a><br/></td></tr>
<tr><td class="memItemLeft" align="right" valign="top">char *&nbsp;</td><td class="memItemRight" valign="bottom"><a class="el" href="group__libshare.html#gab2cf93076bc1fad77bdc407d7fe93cad">shpref_base_dir</a> (void)</td></tr>
<tr><td class="mdescLeft">&nbsp;</td><td class="mdescRight">Specifies the preferred location of where the sharefs filesystem is stored on the local file system.  <a href="#gab2cf93076bc1fad77bdc407d7fe93cad"></a><br/></td></tr>
<tr><td class="memItemLeft" align="right" valign="top">char *&nbsp;</td><td class="memItemRight" valign="bottom"><a class="el" href="group__libshare.html#ga28195af1f93270534e0c384c46ebaff7">shpref_path</a> (void)</td></tr>
<tr><td class="mdescLeft">&nbsp;</td><td class="mdescRight">The local filesystem path for storing configuration options.  <a href="#ga28195af1f93270534e0c384c46ebaff7"></a><br/></td></tr>
<tr><td class="memItemLeft" align="right" valign="top">int&nbsp;</td><td class="memItemRight" valign="bottom"><a class="el" href="group__libshare.html#ga583dd5ea0a74721404a2e178bf49b9ec">shpref_init</a> (void)</td></tr>
<tr><td class="mdescLeft">&nbsp;</td><td class="mdescRight">Initialize an instance of configuration options in memory.  <a href="#ga583dd5ea0a74721404a2e178bf49b9ec"></a><br/></td></tr>
<tr><td class="memItemLeft" align="right" valign="top">void&nbsp;</td><td class="memItemRight" valign="bottom"><a class="el" href="group__libshare.html#ga7c66037cd25e63a38c9ed477fb4608ff">shpref_free</a> (void)</td></tr>
<tr><td class="mdescLeft">&nbsp;</td><td class="mdescRight">Free the configuration options loaded into memory.  <a href="#ga7c66037cd25e63a38c9ed477fb4608ff"></a><br/></td></tr>
<tr><td class="memItemLeft" align="right" valign="top">char *&nbsp;</td><td class="memItemRight" valign="bottom"><a class="el" href="group__libshare.html#ga0221a8f89374a82f46630cbf952d33cc">shpref_get</a> (char *pref, char *default_value)</td></tr>
<tr><td class="mdescLeft">&nbsp;</td><td class="mdescRight">Retrieve a configuration option value.  <a href="#ga0221a8f89374a82f46630cbf952d33cc"></a><br/></td></tr>
<tr><td class="memItemLeft" align="right" valign="top">int&nbsp;</td><td class="memItemRight" valign="bottom"><a class="el" href="group__libshare.html#gad179cffb5af017fec361fe74b6902252">shpref_set</a> (char *pref, char *value)</td></tr>
<tr><td class="mdescLeft">&nbsp;</td><td class="mdescRight">Set a persistent value for a particular libshare user-specific configuration option.  <a href="#gad179cffb5af017fec361fe74b6902252"></a><br/></td></tr>
<tr><td class="memItemLeft" align="right" valign="top">double&nbsp;</td><td class="memItemRight" valign="bottom"><a class="el" href="group__libshare.html#ga0036259a5f0a448ea04094a3e950bcb0">shtime</a> (void)</td></tr>
<tr><td class="mdescLeft">&nbsp;</td><td class="mdescRight">Generate a float-point precision representation of the current time.  <a href="#ga0036259a5f0a448ea04094a3e950bcb0"></a><br/></td></tr>
<tr><td class="memItemLeft" align="right" valign="top"><a class="el" href="group__libshare.html#gafde3ec4ad887dabfd0c44fb010c4cd26">shtime_t</a>&nbsp;</td><td class="memItemRight" valign="bottom"><a class="el" href="group__libshare.html#gafaee49915601ccff85078a5ef4f5564b">shtime64</a> (void)</td></tr>
<tr><td class="mdescLeft">&nbsp;</td><td class="mdescRight">Generate a 64bit representation integral of the current time with millisecond precision.  <a href="#gafaee49915601ccff85078a5ef4f5564b"></a><br/></td></tr>
</table>
<hr/><a name="_details"></a><h2>Detailed Description</h2>
<p>Utility functions to generate unique checksums of data. </p>
<p>Time operations are performed in order to store and compare timestamps in the sharefs file system and for network operations.</p>
<p>Handles management of user-specific configuration options for the Share Library.</p>
<p>Hash code token operations.</p>
<dl class="note"><dt><b>Note:</b></dt><dd>See the <a class="el" href="group__libshare.html#gad814ced5efcb4f0cc806f9da0c9bd299" title="Overwrite a preference for the current session.">shpref_sess_set()</a> function for information on overwriting an option values for the current process session. Specify user specific configuration items.</dd></dl>
<p>libshare_time Time calculating operations. </p>
<hr/><h2>Define Documentation</h2>
<a class="anchor" id="gac6057fb77e1ab69db07b0d4356248c90"></a><!-- doxytag: member="shpref.h::SHPREF_BASE_DIR" ref="gac6057fb77e1ab69db07b0d4356248c90" args="" -->
<div class="memitem">
<div class="memproto">
      <table class="memname">
        <tr>
          <td class="memname">#define SHPREF_BASE_DIR&nbsp;&nbsp;&nbsp;&quot;base-dir&quot;</td>
        </tr>
      </table>
</div>
<div class="memdoc">

<p>Specifies the preferred location of where the sharefs filesystem is stored on the local file system. </p>
<dl class="note"><dt><b>Note:</b></dt><dd>The default location is '$HOME/.share'. </dd>
<dd>
Use <code><a class="el" href="group__libshare.html#gad814ced5efcb4f0cc806f9da0c9bd299" title="Overwrite a preference for the current session.">shpref_sess_set()</a></code> to temporarily overwrite this value. </dd></dl>

<p>Definition at line <a class="el" href="shpref_8h_source.html#l00044">44</a> of file <a class="el" href="shpref_8h_source.html">shpref.h</a>.</p>

</div>
</div>
<a class="anchor" id="ga4e5d56369a63c1083b2830db6fc4ccdf"></a><!-- doxytag: member="shpref.h::shpref_overlay" ref="ga4e5d56369a63c1083b2830db6fc4ccdf" args="()" -->
<div class="memitem">
<div class="memproto">
      <table class="memname">
        <tr>
          <td class="memname">#define shpref_overlay</td>
          <td>(</td>
&nbsp;</td>
          <td class="paramname"></td>
          <td>&nbsp;)&nbsp;</td>
          <td>&nbsp;&nbsp;&nbsp;(0 == strcmp(shpref_get(SHPREF_OVERLAY), &quot;true&quot;) ? TRUE : FALSE)</td>
        </tr>
      </table>
</div>
<div class="memdoc">

<p>Specifies whether to overlay the sharefs filesystem ontop of the work directory on the local filesystem. </p>
<dl class="note"><dt><b>Note:</b></dt><dd>Disable this option to prevent libshare from writing outside of the base directory. </dd></dl>
<dl class="return"><dt><b>Returns:</b></dt><dd>A zero (0) when disabled and a non-zero value when enabled. </dd></dl>

<p>Definition at line <a class="el" href="shpref_8h_source.html#l00086">86</a> of file <a class="el" href="shpref_8h_source.html">shpref.h</a>.</p>

</div>
</div>
<a class="anchor" id="ga649e519df79a6e54465e5e9f62da48bf"></a><!-- doxytag: member="shpref.h::SHPREF_OVERLAY" ref="ga649e519df79a6e54465e5e9f62da48bf" args="" -->
<div class="memitem">
<div class="memproto">
      <table class="memname">
        <tr>
          <td class="memname">#define SHPREF_OVERLAY&nbsp;&nbsp;&nbsp;&quot;overlay&quot;</td>
        </tr>
      </table>
</div>
<div class="memdoc">

<p>Specifies whether the sharefs file system references files on the local filesystem. </p>
<dl class="note"><dt><b>Note:</b></dt><dd>Use <code><a class="el" href="group__libshare.html#gad814ced5efcb4f0cc806f9da0c9bd299" title="Overwrite a preference for the current session.">shpref_sess_set()</a></code> to temporarily overwrite this value. </dd></dl>

<p>Definition at line <a class="el" href="shpref_8h_source.html#l00054">54</a> of file <a class="el" href="shpref_8h_source.html">shpref.h</a>.</p>

</div>
</div>
<a class="anchor" id="ga0284584c0c5e92ff54e7f3dcf0ac0dcb"></a><!-- doxytag: member="shpref.h::shpref_set_overlay" ref="ga0284584c0c5e92ff54e7f3dcf0ac0dcb" args="(opt)" -->
<div class="memitem">
<div class="memproto">
      <table class="memname">
        <tr>
          <td class="memname">#define shpref_set_overlay</td>
          <td>(</td>
          <td class="paramtype">opt&nbsp;</td>
          <td class="paramname"></td>
          <td>&nbsp;)&nbsp;</td>
          <td>&nbsp;&nbsp;&nbsp;(opt ? shpref_set(SHPREF_OVERLAY, &quot;true&quot;) : shpref_set(SHPREF_OVERLAY, &quot;false&quot;))</td>
        </tr>
      </table>
</div>
<div class="memdoc">

<p>Permanently sets the <code>SHPREF_OVERLAY</code> option. </p>
<dl><dt><b>Parameters:</b></dt><dd>
  <table border="0" cellspacing="2" cellpadding="0">
    <tr><td valign="top"></td><td valign="top"><em>opt</em>&nbsp;</td><td>A zero to disable the option and a non-zero to enable. </td></tr>
  </table>
  </dd>
</dl>

<p>Definition at line <a class="el" href="shpref_8h_source.html#l00093">93</a> of file <a class="el" href="shpref_8h_source.html">shpref.h</a>.</p>

</div>
</div>
<a class="anchor" id="ga94f87ebcf4097eee2f109642bf02576c"></a><!-- doxytag: member="shpref.h::shpref_set_track" ref="ga94f87ebcf4097eee2f109642bf02576c" args="(opt)" -->
<div class="memitem">
<div class="memproto">
      <table class="memname">
        <tr>
          <td class="memname">#define shpref_set_track</td>
          <td>(</td>
          <td class="paramtype">opt&nbsp;</td>
          <td class="paramname"></td>
          <td>&nbsp;)&nbsp;</td>
          <td>&nbsp;&nbsp;&nbsp;(opt ? shpref_set(SHPREF_TRACK, &quot;true&quot;) : shpref_set(SHPREF_TRACK, &quot;false&quot;))</td>
        </tr>
      </table>
</div>
<div class="memdoc">

<p>Permanently sets the <code>SHPREF_TRACK</code> option. </p>
<dl><dt><b>Parameters:</b></dt><dd>
  <table border="0" cellspacing="2" cellpadding="0">
    <tr><td valign="top"></td><td valign="top"><em>opt</em>&nbsp;</td><td>A zero to disable the option and a non-zero to enable. </td></tr>
  </table>
  </dd>
</dl>

<p>Definition at line <a class="el" href="shpref_8h_source.html#l00078">78</a> of file <a class="el" href="shpref_8h_source.html">shpref.h</a>.</p>

</div>
</div>
<a class="anchor" id="ga4973650c785959f9b4c2e8622ee8f2a1"></a><!-- doxytag: member="shpref.h::shpref_track" ref="ga4973650c785959f9b4c2e8622ee8f2a1" args="()" -->
<div class="memitem">
<div class="memproto">
      <table class="memname">
        <tr>
          <td class="memname">#define shpref_track</td>
          <td>(</td>
&nbsp;</td>
          <td class="paramname"></td>
          <td>&nbsp;)&nbsp;</td>
          <td>&nbsp;&nbsp;&nbsp;(0 == strcmp(shpref_get(SHPREF_TRACK), &quot;true&quot;) ? TRUE : FALSE)</td>
        </tr>
      </table>
</div>
<div class="memdoc">

<p>Specifies whether to track sharefs filesystem revisions. </p>
<dl class="return"><dt><b>Returns:</b></dt><dd>A zero (0) when disabled and a non-zero value when enabled. </dd></dl>

<p>Definition at line <a class="el" href="shpref_8h_source.html#l00071">71</a> of file <a class="el" href="shpref_8h_source.html">shpref.h</a>.</p>

</div>
</div>
<a class="anchor" id="ga3a0fa502ca8598efb58c4b1aa2d56f27"></a><!-- doxytag: member="shpref.h::SHPREF_TRACK" ref="ga3a0fa502ca8598efb58c4b1aa2d56f27" args="" -->
<div class="memitem">
<div class="memproto">
      <table class="memname">
        <tr>
          <td class="memname">#define SHPREF_TRACK&nbsp;&nbsp;&nbsp;&quot;track&quot;</td>
        </tr>
      </table>
</div>
<div class="memdoc">

<p>Specifies whether to track sharefs filesystem revisions. </p>
<dl class="note"><dt><b>Note:</b></dt><dd>Use <code><a class="el" href="group__libshare.html#gad814ced5efcb4f0cc806f9da0c9bd299" title="Overwrite a preference for the current session.">shpref_sess_set()</a></code> to temporarily overwrite this value. </dd></dl>

<p>Definition at line <a class="el" href="shpref_8h_source.html#l00049">49</a> of file <a class="el" href="shpref_8h_source.html">shpref.h</a>.</p>

</div>
</div>
<hr/><h2>Typedef Documentation</h2>
<a class="anchor" id="ga8067b5b54c80844067174fba5203a7f9"></a><!-- doxytag: member="share.h::shsize_t" ref="ga8067b5b54c80844067174fba5203a7f9" args="" -->
<div class="memitem">
<div class="memproto">
      <table class="memname">
        <tr>
          <td class="memname">typedef uint64_t <a class="el" href="group__libshare.html#ga8067b5b54c80844067174fba5203a7f9">shsize_t</a></td>
        </tr>
      </table>
</div>
<div class="memdoc">

<p>A specification of byte size. </p>
  <a class="el" href="structshmeta__value__v1__t.html#ace894b249d2d3bf7034d98f5c591eaab" title="The total size of data segment with a shmeta_value_t header.">shmeta_value_t.sz</a> </p>

<p>Definition at line <a class="el" href="share_8h_source.html#l00094">94</a> of file <a class="el" href="share_8h_source.html">share.h</a>.</p>

</div>
</div>
<hr/><h2>Function Documentation</h2>
<a class="anchor" id="gafac914ba170c0f2a9bc8c2add3a63a41"></a><!-- doxytag: member="shbuf.h::shbuf_init" ref="gafac914ba170c0f2a9bc8c2add3a63a41" args="(void)" -->
<div class="memitem">
<div class="memproto">
      <table class="memname">
        <tr>
          <td class="memname"><a class="el" href="structshbuf__t.html">shbuf_t</a>* shbuf_init </td>
          <td>(</td>
          <td class="paramtype">void&nbsp;</td>
          <td class="paramname"></td>
          <td>&nbsp;)&nbsp;</td>
          <td></td>
        </tr>
      </table>
</div>
<div class="memdoc">

<p>Initialize a memory buffer for use. </p>
<dl class="note"><dt><b>Note:</b></dt><dd>A <code><a class="el" href="structshbuf__t.html" title="A memory buffer.">shbuf_t</a></code> memory buffer handles automatic allocation of memory. </dd></dl>

</div>
</div>
<a class="anchor" id="ga5c7e8d31be37244ea41da5be6f24bc78"></a><!-- doxytag: member="shkey.h::shkey_init_num" ref="ga5c7e8d31be37244ea41da5be6f24bc78" args="(long kvalue)" -->
<div class="memitem">
<div class="memproto">
      <table class="memname">
        <tr>
          <td class="memname"><a class="el" href="group__libshare.html#ga315bac56fce878a38a2d91ca49c4c374">shkey_t</a> shkey_init_num </td>
          <td>(</td>
          <td class="paramtype">long&nbsp;</td>
          <td class="paramname"> <em>kvalue</em></td>
          <td>&nbsp;)&nbsp;</td>
          <td></td>
        </tr>
      </table>
</div>
<div class="memdoc">

<p>Create a <code>shkey_t</code> hashmap key reference from a number. </p>
<p><em>kvalue</em> The number to generate into a <code>shkey_t</code> </p>
<dl class="return"><dt><b>Returns:</b></dt><dd>A statically allocated version of  </dd></dl>

</div>
</div>
<a class="anchor" id="gad8822a98cbfbde7a9b87de94a83ba08d"></a><!-- doxytag: member="shkey.h::shkey_init_str" ref="gad8822a98cbfbde7a9b87de94a83ba08d" args="(char *kvalue)" -->
<div class="memitem">
<div class="memproto">
      <table class="memname">
        <tr>
          <td class="memname"><a class="el" href="group__libshare.html#ga315bac56fce878a38a2d91ca49c4c374">shkey_t</a> shkey_init_str </td>
          <td>(</td>
          <td class="paramtype">char *&nbsp;</td>
          <td class="paramname"> <em>kvalue</em></td>
          <td>&nbsp;)&nbsp;</td>
          <td></td>
        </tr>
      </table>
</div>
<div class="memdoc">

<p>Create a <code>shkey_t</code> hashmap key reference from <code>kvalue</code> <em>kvalue</em> The string to generate into a <code>shkey_t</code>. </p>
<dl class="return"><dt><b>Returns:</b></dt><dd>A <code>shkey_t</code> referencing a kvalue </dd></dl>

</div>
</div>
<a class="anchor" id="ga23f802b9ef8431b59a416b540377ad8f"></a><!-- doxytag: member="shkey.h::shkey_init_unique" ref="ga23f802b9ef8431b59a416b540377ad8f" args="(void)" -->
<div class="memitem">
<div class="memproto">
      <table class="memname">
        <tr>
          <td class="memname"><a class="el" href="group__libshare.html#ga315bac56fce878a38a2d91ca49c4c374">shkey_t</a> shkey_init_unique </td>
          <td>(</td>
          <td class="paramtype">void&nbsp;</td>
          <td class="paramname"></td>
          <td>&nbsp;)&nbsp;</td>
          <td></td>
        </tr>
      </table>
</div>
<div class="memdoc">

<p>Create a unique <code>shkey_t</code> hashmap key reference. </p>
<dl class="return"><dt><b>Returns:</b></dt><dd>A <code>shkey_t</code> containing a unique key value. </dd></dl>

</div>
</div>
<a class="anchor" id="gab2cf93076bc1fad77bdc407d7fe93cad"></a><!-- doxytag: member="shpref.h::shpref_base_dir" ref="gab2cf93076bc1fad77bdc407d7fe93cad" args="(void)" -->
<div class="memitem">
<div class="memproto">
      <table class="memname">
        <tr>
          <td class="memname">char* shpref_base_dir </td>
          <td>(</td>
          <td class="paramtype">void&nbsp;</td>
          <td class="paramname"></td>
          <td>&nbsp;)&nbsp;</td>
          <td></td>
        </tr>
      </table>
</div>
<div class="memdoc">

<p>Specifies the preferred location of where the sharefs filesystem is stored on the local file system. </p>
<dl class="return"><dt><b>Returns:</b></dt><dd>The path to a directory on the local file-system. </dd></dl>

</div>
</div>
<a class="anchor" id="ga7c66037cd25e63a38c9ed477fb4608ff"></a><!-- doxytag: member="shpref.h::shpref_free" ref="ga7c66037cd25e63a38c9ed477fb4608ff" args="(void)" -->
<div class="memitem">
<div class="memproto">
      <table class="memname">
        <tr>
          <td class="memname">void shpref_free </td>
          <td>(</td>
          <td class="paramtype">void&nbsp;</td>
          <td class="paramname"></td>
          <td>&nbsp;)&nbsp;</td>
          <td></td>
        </tr>
      </table>
</div>
<div class="memdoc">

<p>Free the configuration options loaded into memory. </p>
<dl class="note"><dt><b>Note:</b></dt><dd>This will remove all temporar configuration settings that have been made this process session. </dd></dl>

</div>
</div>
<a class="anchor" id="ga0221a8f89374a82f46630cbf952d33cc"></a><!-- doxytag: member="shpref.h::shpref_get" ref="ga0221a8f89374a82f46630cbf952d33cc" args="(char *pref, char *default_value)" -->
<div class="memitem">
<div class="memproto">
      <table class="memname">
        <tr>
          <td class="memname">char* shpref_get </td>
          <td>(</td>
          <td class="paramtype">char *&nbsp;</td>
          <td class="paramname"> <em>pref</em>, </td>
        </tr>
        <tr>
          <td class="paramkey"></td>
          <td></td>
          <td class="paramtype">char *&nbsp;</td>
          <td class="paramname"> <em>default_value</em></td><td>&nbsp;</td>
        </tr>
        <tr>
          <td></td>
          <td>)</td>
          <td></td><td></td><td></td>
        </tr>
      </table>
</div>
<div class="memdoc">

<p>Retrieve a configuration option value. </p>
<dl><dt><b>Parameters:</b></dt><dd>
  <table border="0" cellspacing="2" cellpadding="0">
    <tr><td valign="top"></td><td valign="top"><em>pref</em>&nbsp;</td><td>The name of the preference. </td></tr>
    <tr><td valign="top"></td><td valign="top"><em>default_value</em>&nbsp;</td><td>The default string value to return if the preference is not set. </td></tr>
  </table>
  </dd>
</dl>
<dl class="return"><dt><b>Returns:</b></dt><dd>The configuration option value. </dd></dl>

</div>
</div>
<a class="anchor" id="ga583dd5ea0a74721404a2e178bf49b9ec"></a><!-- doxytag: member="shpref.h::shpref_init" ref="ga583dd5ea0a74721404a2e178bf49b9ec" args="(void)" -->
<div class="memitem">
<div class="memproto">
      <table class="memname">
        <tr>
          <td class="memname">int shpref_init </td>
          <td>(</td>
          <td class="paramtype">void&nbsp;</td>
          <td class="paramname"></td>
          <td>&nbsp;)&nbsp;</td>
          <td></td>
        </tr>
      </table>
</div>
<div class="memdoc">

<p>Initialize an instance of configuration options in memory. </p>
<dl class="note"><dt><b>Note:</b></dt><dd>This function does not need to be called in order to retrieve or set configuration options. </dd></dl>
<dl class="return"><dt><b>Returns:</b></dt><dd>A zero (0) on success and a negative one (-1) on failure. </dd></dl>

</div>
</div>
<a class="anchor" id="ga28195af1f93270534e0c384c46ebaff7"></a><!-- doxytag: member="shpref.h::shpref_path" ref="ga28195af1f93270534e0c384c46ebaff7" args="(void)" -->
<div class="memitem">
<div class="memproto">
      <table class="memname">
        <tr>
          <td class="memname">char* shpref_path </td>
          <td>(</td>
          <td class="paramtype">void&nbsp;</td>
          <td class="paramname"></td>
          <td>&nbsp;)&nbsp;</td>
          <td></td>
        </tr>
      </table>
</div>
<div class="memdoc">

<p>The local filesystem path for storing configuration options. </p>
<dl class="return"><dt><b>Returns:</b></dt><dd>The path to the location on the local file-system that contains user-specific libshare configuration options. </dd></dl>

</div>
</div>
<a class="anchor" id="gad179cffb5af017fec361fe74b6902252"></a><!-- doxytag: member="shpref.h::shpref_set" ref="gad179cffb5af017fec361fe74b6902252" args="(char *pref, char *value)" -->
<div class="memitem">
<div class="memproto">
      <table class="memname">
        <tr>
          <td class="memname">int shpref_set </td>
          <td>(</td>
          <td class="paramtype">char *&nbsp;</td>
          <td class="paramname"> <em>pref</em>, </td>
        </tr>
        <tr>
          <td class="paramkey"></td>
          <td></td>
          <td class="paramtype">char *&nbsp;</td>
          <td class="paramname"> <em>value</em></td><td>&nbsp;</td>
        </tr>
        <tr>
          <td></td>
          <td>)</td>
          <td></td><td></td><td></td>
        </tr>
      </table>
</div>
<div class="memdoc">

<p>Set a persistent value for a particular libshare user-specific configuration option. </p>
<p>Specify user specific configuration items: SHPREF_BASE_DIR The base directory to store sharefs file data. SHPREF_TRACK Whether to automatically track file revisions. SHPREF_OVERLAY Whether to write outside of the base directory. Set a configuration option value. </p>
<dl><dt><b>Parameters:</b></dt><dd>
  <table border="0" cellspacing="2" cellpadding="0">
    <tr><td valign="top"></td><td valign="top"><em>pref</em>&nbsp;</td><td>The name of the preference. </td></tr>
    <tr><td valign="top"></td><td valign="top"><em>value</em>&nbsp;</td><td>The configuration option value. </td></tr>
  </table>
  </dd>
</dl>
<dl class="return"><dt><b>Returns:</b></dt><dd>The configuration option value. </dd></dl>

</div>
</div>
<a class="anchor" id="ga0036259a5f0a448ea04094a3e950bcb0"></a><!-- doxytag: member="shtime.h::shtime" ref="ga0036259a5f0a448ea04094a3e950bcb0" args="(void)" -->
<div class="memitem">
<div class="memproto">
      <table class="memname">
        <tr>
          <td class="memname">double shtime </td>
          <td>(</td>
          <td class="paramtype">void&nbsp;</td>
          <td class="paramname"></td>
          <td>&nbsp;)&nbsp;</td>
          <td></td>
        </tr>
      </table>
</div>
<div class="memdoc">

<p>Generate a float-point precision representation of the current time. </p>
<dl class="return"><dt><b>Returns:</b></dt><dd>an double representing the milliseconds since 2012 UTC. </dd></dl>

</div>
</div>
<a class="anchor" id="gafaee49915601ccff85078a5ef4f5564b"></a><!-- doxytag: member="shtime.h::shtime64" ref="gafaee49915601ccff85078a5ef4f5564b" args="(void)" -->
<div class="memitem">
<div class="memproto">
      <table class="memname">
        <tr>
          <td class="memname"><a class="el" href="group__libshare.html#gafde3ec4ad887dabfd0c44fb010c4cd26">shtime_t</a> shtime64 </td>
          <td>(</td>
          <td class="paramtype">void&nbsp;</td>
          <td class="paramname"></td>
          <td>&nbsp;)&nbsp;</td>
          <td></td>
        </tr>
      </table>
</div>
<div class="memdoc">

<p>Generate a 64bit representation integral of the current time with millisecond precision. </p>
<dl class="return"><dt><b>Returns:</b></dt><dd>an unsigned long representing the milliseconds since 2012 UTC. </dd></dl>

</div>
</div>
</div>

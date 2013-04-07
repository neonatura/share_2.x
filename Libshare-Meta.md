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
<h1>Meta definition hash maps (meta maps).<br/>
<small>
[<a class="el" href="group__libshare.html">Libshare</a>]</small>
</h1>
<p>A meta definition is part of a <code><a class="el" href="structshmeta__t.html" title="The size of the array is always a power of two.">shmeta_t</a></code> hashmap.  
<a href="#_details">More...</a></p>
<table border="0" cellpadding="0" cellspacing="0">
<tr><td colspan="2"><h2>Data Structures</h2></td></tr>
<tr><td class="memItemLeft" align="right" valign="top">struct &nbsp;</td><td class="memItemRight" valign="bottom"><a class="el" href="structshmeta__entry__t.html">shmeta_entry_t</a></td></tr>
<tr><td class="memItemLeft" align="right" valign="top">struct &nbsp;</td><td class="memItemRight" valign="bottom"><a class="el" href="structshmeta__index__t.html">shmeta_index_t</a></td></tr>
<tr><td class="mdescLeft">&nbsp;</td><td class="mdescRight">Data structure for iterating through a hash table.  <a href="structshmeta__index__t.html#_details">More...</a><br/></td></tr>
<tr><td class="memItemLeft" align="right" valign="top">struct &nbsp;</td><td class="memItemRight" valign="bottom"><a class="el" href="structshmeta__t.html">shmeta_t</a></td></tr>
<tr><td class="mdescLeft">&nbsp;</td><td class="mdescRight">The size of the array is always a power of two.  <a href="structshmeta__t.html#_details">More...</a><br/></td></tr>
<tr><td class="memItemLeft" align="right" valign="top">struct &nbsp;</td><td class="memItemRight" valign="bottom"><a class="el" href="structshmeta__value__v1__t.html">shmeta_value_v1_t</a></td></tr>
<tr><td class="mdescLeft">&nbsp;</td><td class="mdescRight">The base of a version 1 shmeta hashmap entry value.  <a href="structshmeta__value__v1__t.html#_details">More...</a><br/></td></tr>
<tr><td colspan="2"><h2>Defines</h2></td></tr>
<tr><td class="memItemLeft" align="right" valign="top"><a class="anchor" id="gaf6536b1963d44ae8dcd548fdceaa38e5"></a><!-- doxytag: member="libshare_meta::INITIAL_MAX" ref="gaf6536b1963d44ae8dcd548fdceaa38e5" args="" -->
#define&nbsp;</td><td class="memItemRight" valign="bottom"><a class="el" href="group__libshare__meta.html#gaf6536b1963d44ae8dcd548fdceaa38e5">INITIAL_MAX</a>&nbsp;&nbsp;&nbsp;15</td></tr>
<tr><td class="mdescLeft">&nbsp;</td><td class="mdescRight">The initial number of hashmap indexes to create. <br/></td></tr>
<tr><td class="memItemLeft" align="right" valign="top"><a class="anchor" id="gac4d51ed11f019d7edb3aa2e467677298"></a><!-- doxytag: member="libshare_meta::SHMETA_VALUE_MAGIC" ref="gac4d51ed11f019d7edb3aa2e467677298" args="" -->
#define&nbsp;</td><td class="memItemRight" valign="bottom"><a class="el" href="group__libshare__meta.html#gac4d51ed11f019d7edb3aa2e467677298">SHMETA_VALUE_MAGIC</a>&nbsp;&nbsp;&nbsp;0x12345678</td></tr>
<tr><td class="mdescLeft">&nbsp;</td><td class="mdescRight">Specifies a hard-coded value that identifies a <code>shmeta_value_t</code> data segment. <br/></td></tr>
<tr><td class="memItemLeft" align="right" valign="top"><a class="anchor" id="ga948a300aa648024544f9321527c038be"></a><!-- doxytag: member="libshare_meta::SHMETA_VALUE_NET_MAGIC" ref="ga948a300aa648024544f9321527c038be" args="" -->
#define&nbsp;</td><td class="memItemRight" valign="bottom"><a class="el" href="group__libshare__meta.html#ga948a300aa648024544f9321527c038be">SHMETA_VALUE_NET_MAGIC</a>&nbsp;&nbsp;&nbsp;htons(0x12345678)</td></tr>
<tr><td class="mdescLeft">&nbsp;</td><td class="mdescRight">The network byte order representation of <code>SHMETA_VALUE_MAGIC</code>. <br/></td></tr>
<tr><td class="memItemLeft" align="right" valign="top">#define&nbsp;</td><td class="memItemRight" valign="bottom"><a class="el" href="group__libshare__meta.html#ga3eab0782de3893d7f532701309e92492">SHMETA_BIG_ENDIAN</a>&nbsp;&nbsp;&nbsp;0</td></tr>
<tr><td class="mdescLeft">&nbsp;</td><td class="mdescRight">Specifies that a machine has a big endian architecture.  <a href="#ga3eab0782de3893d7f532701309e92492"></a><br/></td></tr>
<tr><td class="memItemLeft" align="right" valign="top">#define&nbsp;</td><td class="memItemRight" valign="bottom"><a class="el" href="group__libshare__meta.html#ga61a3dcaf5a99985b7c4442c6ee454028">SHMETA_SMALL_ENDIAN</a>&nbsp;&nbsp;&nbsp;1</td></tr>
<tr><td class="mdescLeft">&nbsp;</td><td class="mdescRight">Specifies that a machine has a small endian architecture.  <a href="#ga61a3dcaf5a99985b7c4442c6ee454028"></a><br/></td></tr>
<tr><td class="memItemLeft" align="right" valign="top">#define&nbsp;</td><td class="memItemRight" valign="bottom"><a class="el" href="group__libshare__meta.html#gabbd81e29e6bbee43df50a9fda916caf5">SHMETA_VALUE_ENDIAN</a>(_val)</td></tr>
<tr><td class="mdescLeft">&nbsp;</td><td class="mdescRight">Determines whether the meta value originated from a big or small endian architecture.  <a href="#gabbd81e29e6bbee43df50a9fda916caf5"></a><br/></td></tr>
<tr><td class="memItemLeft" align="right" valign="top"><a class="anchor" id="ga1ec6324231a047343f49ef5bdc8af75e"></a><!-- doxytag: member="libshare_meta::SHPF_NONE" ref="ga1ec6324231a047343f49ef5bdc8af75e" args="" -->
#define&nbsp;</td><td class="memItemRight" valign="bottom"><a class="el" href="group__libshare__meta.html#ga1ec6324231a047343f49ef5bdc8af75e">SHPF_NONE</a>&nbsp;&nbsp;&nbsp;0</td></tr>
<tr><td class="mdescLeft">&nbsp;</td><td class="mdescRight">A <code>shmeta_value_t</code> parameter specific to a indeterminate data segment. <br/></td></tr>
<tr><td class="memItemLeft" align="right" valign="top"><a class="anchor" id="gaedc9cd2b5f6c08b15cd7d236da0fdcd0"></a><!-- doxytag: member="libshare_meta::SHPF_STRING" ref="gaedc9cd2b5f6c08b15cd7d236da0fdcd0" args="" -->
#define&nbsp;</td><td class="memItemRight" valign="bottom"><a class="el" href="group__libshare__meta.html#gaedc9cd2b5f6c08b15cd7d236da0fdcd0">SHPF_STRING</a>&nbsp;&nbsp;&nbsp;1</td></tr>
<tr><td class="mdescLeft">&nbsp;</td><td class="mdescRight">A <code>shmeta_value_t</code> parameter specific to a null-terminated string value. <br/></td></tr>
<tr><td colspan="2"><h2>Typedefs</h2></td></tr>
<tr><td class="memItemLeft" align="right" valign="top"><a class="anchor" id="ga7c52104d34067c6e2f8266cd216e1b14"></a><!-- doxytag: member="libshare_meta::shmeta_t" ref="ga7c52104d34067c6e2f8266cd216e1b14" args="" -->
typedef struct <a class="el" href="structshmeta__t.html">shmeta_t</a>&nbsp;</td><td class="memItemRight" valign="bottom"><a class="el" href="group__libshare__meta.html#ga7c52104d34067c6e2f8266cd216e1b14">shmeta_t</a></td></tr>
<tr><td class="mdescLeft">&nbsp;</td><td class="mdescRight">A hashmap table. <br/></td></tr>
<tr><td class="memItemLeft" align="right" valign="top"><a class="anchor" id="ga2d6019524fc8e500f9f7426217d265e6"></a><!-- doxytag: member="libshare_meta::shmeta_index_t" ref="ga2d6019524fc8e500f9f7426217d265e6" args="" -->
typedef struct <a class="el" href="structshmeta__index__t.html">shmeta_index_t</a>&nbsp;</td><td class="memItemRight" valign="bottom"><a class="el" href="group__libshare__meta.html#ga2d6019524fc8e500f9f7426217d265e6">shmeta_index_t</a></td></tr>
<tr><td class="mdescLeft">&nbsp;</td><td class="mdescRight">A hashmap index. <br/></td></tr>
<tr><td class="memItemLeft" align="right" valign="top">typedef unsigned int(*&nbsp;</td><td class="memItemRight" valign="bottom"><a class="el" href="group__libshare__meta.html#gabf92a3d43a2585d4cb711e99e616ad8e">shmetafunc_t</a> )(const char *key, ssize_t *klen)</td></tr>
<tr><td class="mdescLeft">&nbsp;</td><td class="mdescRight">Callback functions for calculating hash values.  <a href="#gabf92a3d43a2585d4cb711e99e616ad8e"></a><br/></td></tr>
<tr><td class="memItemLeft" align="right" valign="top"><a class="anchor" id="ga575eb4e33febcc71bcb75f42ff36200e"></a><!-- doxytag: member="libshare_meta::shmeta_entry_t" ref="ga575eb4e33febcc71bcb75f42ff36200e" args="" -->
typedef struct <a class="el" href="structshmeta__entry__t.html">shmeta_entry_t</a>&nbsp;</td><td class="memItemRight" valign="bottom"><a class="el" href="group__libshare__meta.html#ga575eb4e33febcc71bcb75f42ff36200e">shmeta_entry_t</a></td></tr>
<tr><td class="mdescLeft">&nbsp;</td><td class="mdescRight">A hashmap entry. <br/></td></tr>
<tr><td class="memItemLeft" align="right" valign="top"><a class="anchor" id="ga46ac3535e882464250533fb99874e4f1"></a><!-- doxytag: member="libshare_meta::shmeta_value_t" ref="ga46ac3535e882464250533fb99874e4f1" args="" -->
typedef struct <a class="el" href="structshmeta__value__v1__t.html">shmeta_value_v1_t</a>&nbsp;</td><td class="memItemRight" valign="bottom"><a class="el" href="group__libshare__meta.html#ga46ac3535e882464250533fb99874e4f1">shmeta_value_t</a></td></tr>
<tr><td class="mdescLeft">&nbsp;</td><td class="mdescRight">Specifies a reference to the current version of a shmeta hashmap entry value. <br/></td></tr>
<tr><td colspan="2"><h2>Functions</h2></td></tr>
<tr><td class="memItemLeft" align="right" valign="top"><a class="el" href="structshmeta__t.html">shmeta_t</a> *&nbsp;</td><td class="memItemRight" valign="bottom"><a class="el" href="group__libshare__meta.html#ga114784024ebdf65c16461686ff510419">shmeta_init</a> (void)</td></tr>
<tr><td class="mdescLeft">&nbsp;</td><td class="mdescRight">Create an instance of a meta definition hashmap.  <a href="#ga114784024ebdf65c16461686ff510419"></a><br/></td></tr>
<tr><td class="memItemLeft" align="right" valign="top">void&nbsp;</td><td class="memItemRight" valign="bottom"><a class="el" href="group__libshare__meta.html#ga08eb531a3201e24de9049743ce0baae0">shmeta_free</a> (<a class="el" href="structshmeta__t.html">shmeta_t</a> **meta_p)</td></tr>
<tr><td class="mdescLeft">&nbsp;</td><td class="mdescRight">Free an instance of a meta definition hashmap.  <a href="#ga08eb531a3201e24de9049743ce0baae0"></a><br/></td></tr>
<tr><td class="memItemLeft" align="right" valign="top"><a class="anchor" id="gae530ddce42780a4730e440eb57853f5c"></a><!-- doxytag: member="libshare_meta::shmetafunc_default" ref="gae530ddce42780a4730e440eb57853f5c" args="(const char *char_key, ssize_t *klen)" -->
unsigned int&nbsp;</td><td class="memItemRight" valign="bottom"><a class="el" href="group__libshare__meta.html#gae530ddce42780a4730e440eb57853f5c">shmetafunc_default</a> (const char *char_key, ssize_t *klen)</td></tr>
<tr><td class="mdescLeft">&nbsp;</td><td class="mdescRight">The default hashmap indexing function. <br/></td></tr>
<tr><td class="memItemLeft" align="right" valign="top">void&nbsp;</td><td class="memItemRight" valign="bottom"><a class="el" href="group__libshare__meta.html#ga26c427c68302b1737d58449d8d93f629">shmeta_set</a> (<a class="el" href="structshmeta__t.html">shmeta_t</a> *ht, <a class="el" href="group__libshare.html#ga315bac56fce878a38a2d91ca49c4c374">shkey_t</a> sh_k, <a class="el" href="structshmeta__value__v1__t.html">shmeta_value_t</a> *val)</td></tr>
<tr><td class="mdescLeft">&nbsp;</td><td class="mdescRight">Set a meta definition to a particular value.  <a href="#ga26c427c68302b1737d58449d8d93f629"></a><br/></td></tr>
<tr><td class="memItemLeft" align="right" valign="top">void *&nbsp;</td><td class="memItemRight" valign="bottom"><a class="el" href="group__libshare__meta.html#ga0ed283744b26bd867ac12779d1321b56">shmeta_get</a> (<a class="el" href="structshmeta__t.html">shmeta_t</a> *ht, <a class="el" href="group__libshare.html#ga315bac56fce878a38a2d91ca49c4c374">shkey_t</a> sh_k)</td></tr>
<tr><td class="mdescLeft">&nbsp;</td><td class="mdescRight">Get a meta definition value.  <a href="#ga0ed283744b26bd867ac12779d1321b56"></a><br/></td></tr>
<tr><td class="memItemLeft" align="right" valign="top">void&nbsp;</td><td class="memItemRight" valign="bottom"><a class="el" href="group__libshare__meta.html#gaec97dff4c2a4e59ab2720ff37d18b7a3">shmeta_print</a> (<a class="el" href="structshmeta__t.html">shmeta_t</a> *h, <a class="el" href="structshbuf__t.html">shbuf_t</a> *ret_buff)</td></tr>
<tr><td class="mdescLeft">&nbsp;</td><td class="mdescRight">Prints out a JSON representation of a meta definition hashmap.  <a href="#gaec97dff4c2a4e59ab2720ff37d18b7a3"></a><br/></td></tr>
<tr><td class="memItemLeft" align="right" valign="top"><a class="el" href="structshmeta__value__v1__t.html">shmeta_value_t</a> *&nbsp;</td><td class="memItemRight" valign="bottom"><a class="el" href="group__libshare__meta.html#gaa02d74e6f751797747941d593ef1547f">shmeta_str</a> (char *str)</td></tr>
<tr><td class="mdescLeft">&nbsp;</td><td class="mdescRight">Creates a <code>shmeta_value_t</code> hashmap value from a string.  <a href="#gaa02d74e6f751797747941d593ef1547f"></a><br/></td></tr>
</table>
<hr/><a name="_details"></a><h2>Detailed Description</h2>
<p>A meta definition is part of a <code><a class="el" href="structshmeta__t.html" title="The size of the array is always a power of two.">shmeta_t</a></code> hashmap. </p>
<p>The share library meta definitions can be used to hash header information from a socket stream, retaining access to the meta information by a token, and allowing for efficient redelivery or caching.</p>
<p>In reference to the internal form of a hash table:</p>
<p>The table is an array indexed by the hash of the key; collisions are resolved by hanging a linked list of hash entries off each element of the array. Although this is a really simple design it isn't too bad given that pools have a low allocation overhead.</p>
<dl class="note"><dt><b>Note:</b></dt><dd>A <code>shfs_tree</code> sharefs file system associates meta definition information with every <code>shfs_node</code> inode entry. </dd></dl>
<hr/><h2>Define Documentation</h2>
<a class="anchor" id="ga3eab0782de3893d7f532701309e92492"></a><!-- doxytag: member="shmeta.h::SHMETA_BIG_ENDIAN" ref="ga3eab0782de3893d7f532701309e92492" args="" -->
<div class="memitem">
<div class="memproto">
      <table class="memname">
        <tr>
          <td class="memname">#define SHMETA_BIG_ENDIAN&nbsp;&nbsp;&nbsp;0</td>
        </tr>
      </table>
</div>
<div class="memdoc">

<p>Specifies that a machine has a big endian architecture. </p>
<dl class="see"><dt><b>See also:</b></dt><dd><a class="el" href="group__libshare__meta.html#gabbd81e29e6bbee43df50a9fda916caf5" title="Determines whether the meta value originated from a big or small endian architecture...">SHMETA_VALUE_ENDIAN</a> </dd></dl>

<p>Definition at line <a class="el" href="shmeta_8h_source.html#l00070">70</a> of file <a class="el" href="shmeta_8h_source.html">shmeta.h</a>.</p>

</div>
</div>
<a class="anchor" id="ga61a3dcaf5a99985b7c4442c6ee454028"></a><!-- doxytag: member="shmeta.h::SHMETA_SMALL_ENDIAN" ref="ga61a3dcaf5a99985b7c4442c6ee454028" args="" -->
<div class="memitem">
<div class="memproto">
      <table class="memname">
        <tr>
          <td class="memname">#define SHMETA_SMALL_ENDIAN&nbsp;&nbsp;&nbsp;1</td>
        </tr>
      </table>
</div>
<div class="memdoc">

<p>Specifies that a machine has a small endian architecture. </p>
<dl class="see"><dt><b>See also:</b></dt><dd><a class="el" href="group__libshare__meta.html#gabbd81e29e6bbee43df50a9fda916caf5" title="Determines whether the meta value originated from a big or small endian architecture...">SHMETA_VALUE_ENDIAN</a> </dd></dl>

<p>Definition at line <a class="el" href="shmeta_8h_source.html#l00076">76</a> of file <a class="el" href="shmeta_8h_source.html">shmeta.h</a>.</p>

</div>
</div>
<a class="anchor" id="gabbd81e29e6bbee43df50a9fda916caf5"></a><!-- doxytag: member="shmeta.h::SHMETA_VALUE_ENDIAN" ref="gabbd81e29e6bbee43df50a9fda916caf5" args="(_val)" -->
<div class="memitem">
<div class="memproto">
      <table class="memname">
        <tr>
          <td class="memname">#define SHMETA_VALUE_ENDIAN</td>
          <td>(</td>
          <td class="paramtype">_val&nbsp;</td>
          <td class="paramname"></td>
          <td>&nbsp;)&nbsp;</td>
          <td></td>
        </tr>
      </table>
</div>
<div class="memdoc">
<b>Value:</b><div class="fragment"><pre class="fragment">(_val-&gt;magic == <a class="code" href="group__libshare__meta.html#ga948a300aa648024544f9321527c038be" title="The network byte order representation of SHMETA_VALUE_MAGIC.">SHMETA_VALUE_NET_MAGIC</a> ? \
   <a class="code" href="group__libshare__meta.html#ga3eab0782de3893d7f532701309e92492" title="Specifies that a machine has a big endian architecture.">SHMETA_BIG_ENDIAN</a> : <a class="code" href="group__libshare__meta.html#ga61a3dcaf5a99985b7c4442c6ee454028" title="Specifies that a machine has a small endian architecture.">SHMETA_SMALL_ENDIAN</a>)
</pre></div>
<p>Determines whether the meta value originated from a big or small endian architecture. </p>
<dl class="return"><dt><b>Returns:</b></dt><dd>SHMETA_BIG_ENDIAN or SHMETA_SMALL_ENDIAN based on the meta value. </dd></dl>

<p>Definition at line <a class="el" href="shmeta_8h_source.html#l00082">82</a> of file <a class="el" href="shmeta_8h_source.html">shmeta.h</a>.</p>

</div>
</div>
<hr/><h2>Typedef Documentation</h2>
<a class="anchor" id="gabf92a3d43a2585d4cb711e99e616ad8e"></a><!-- doxytag: member="shmeta.h::shmetafunc_t" ref="gabf92a3d43a2585d4cb711e99e616ad8e" args=")(const char *key, ssize_t *klen)" -->
<div class="memitem">
<div class="memproto">
      <table class="memname">
        <tr>
          <td class="memname">typedef unsigned int(* <a class="el" href="group__libshare__meta.html#gabf92a3d43a2585d4cb711e99e616ad8e">shmetafunc_t</a>)(const char *key, ssize_t *klen)</td>
        </tr>
      </table>
</div>
<div class="memdoc">

<p>Callback functions for calculating hash values. </p>
<dl><dt><b>Parameters:</b></dt><dd>
  <table border="0" cellspacing="2" cellpadding="0">
    <tr><td valign="top"></td><td valign="top"><em>key</em>&nbsp;</td><td>The key. </td></tr>
    <tr><td valign="top"></td><td valign="top"><em>klen</em>&nbsp;</td><td>The length of the key. </td></tr>
  </table>
  </dd>
</dl>

<p>Definition at line <a class="el" href="shmeta_8h_source.html#l00111">111</a> of file <a class="el" href="shmeta_8h_source.html">shmeta.h</a>.</p>

</div>
</div>
<hr/><h2>Function Documentation</h2>
<a class="anchor" id="ga08eb531a3201e24de9049743ce0baae0"></a><!-- doxytag: member="shmeta.h::shmeta_free" ref="ga08eb531a3201e24de9049743ce0baae0" args="(shmeta_t **meta_p)" -->
<div class="memitem">
<div class="memproto">
      <table class="memname">
        <tr>
          <td class="memname">void shmeta_free </td>
          <td>(</td>
          <td class="paramtype"><a class="el" href="structshmeta__t.html">shmeta_t</a> **&nbsp;</td>
          <td class="paramname"> <em>meta_p</em></td>
          <td>&nbsp;)&nbsp;</td>
          <td></td>
        </tr>
      </table>
</div>
<div class="memdoc">

<p>Free an instance of a meta definition hashmap. </p>
<dl><dt><b>Parameters:</b></dt><dd>
  <table border="0" cellspacing="2" cellpadding="0">
    <tr><td valign="top"></td><td valign="top"><em>meta_p</em>&nbsp;</td><td>A reference to the meta definition hashmap to be free'd. </td></tr>
  </table>
  </dd>
</dl>

</div>
</div>
<a class="anchor" id="ga0ed283744b26bd867ac12779d1321b56"></a><!-- doxytag: member="shmeta.h::shmeta_get" ref="ga0ed283744b26bd867ac12779d1321b56" args="(shmeta_t *ht, shkey_t sh_k)" -->
<div class="memitem">
<div class="memproto">
      <table class="memname">
        <tr>
          <td class="memname">void* shmeta_get </td>
          <td>(</td>
          <td class="paramtype"><a class="el" href="structshmeta__t.html">shmeta_t</a> *&nbsp;</td>
          <td class="paramname"> <em>ht</em>, </td>
        </tr>
        <tr>
          <td class="paramkey"></td>
          <td></td>
          <td class="paramtype"><a class="el" href="group__libshare.html#ga315bac56fce878a38a2d91ca49c4c374">shkey_t</a>&nbsp;</td>
          <td class="paramname"> <em>sh_k</em></td><td>&nbsp;</td>
        </tr>
        <tr>
          <td></td>
          <td>)</td>
          <td></td><td></td><td></td>
        </tr>
      </table>
</div>
<div class="memdoc">

<p>Get a meta definition value. </p>
<dl><dt><b>Parameters:</b></dt><dd>
  <table border="0" cellspacing="2" cellpadding="0">
    <tr><td valign="top"></td><td valign="top"><em>ht</em>&nbsp;</td><td>The meta definition hashmap to retrieve from. </td></tr>
    <tr><td valign="top"></td><td valign="top"><em>sh_k</em>&nbsp;</td><td>The key of the meta definition value. </td></tr>
  </table>
  </dd>
</dl>

</div>
</div>
<a class="anchor" id="ga114784024ebdf65c16461686ff510419"></a><!-- doxytag: member="shmeta.h::shmeta_init" ref="ga114784024ebdf65c16461686ff510419" args="(void)" -->
<div class="memitem">
<div class="memproto">
      <table class="memname">
        <tr>
          <td class="memname"><a class="el" href="structshmeta__t.html">shmeta_t</a>* shmeta_init </td>
          <td>(</td>
          <td class="paramtype">void&nbsp;</td>
          <td class="paramname"></td>
          <td>&nbsp;)&nbsp;</td>
          <td></td>
        </tr>
      </table>
</div>
<div class="memdoc">

<p>Create an instance of a meta definition hashmap. </p>
<dl class="return"><dt><b>Returns:</b></dt><dd>A <code><a class="el" href="structshmeta__t.html" title="The size of the array is always a power of two.">shmeta_t</a></code> meta definition hashmap. </dd></dl>

</div>
</div>
<a class="anchor" id="gaec97dff4c2a4e59ab2720ff37d18b7a3"></a><!-- doxytag: member="shmeta.h::shmeta_print" ref="gaec97dff4c2a4e59ab2720ff37d18b7a3" args="(shmeta_t *h, shbuf_t *ret_buff)" -->
<div class="memitem">
<div class="memproto">
      <table class="memname">
        <tr>
          <td class="memname">void shmeta_print </td>
          <td>(</td>
          <td class="paramtype"><a class="el" href="structshmeta__t.html">shmeta_t</a> *&nbsp;</td>
          <td class="paramname"> <em>h</em>, </td>
        </tr>
        <tr>
          <td class="paramkey"></td>
          <td></td>
          <td class="paramtype"><a class="el" href="structshbuf__t.html">shbuf_t</a> *&nbsp;</td>
          <td class="paramname"> <em>ret_buff</em></td><td>&nbsp;</td>
        </tr>
        <tr>
          <td></td>
          <td>)</td>
          <td></td><td></td><td></td>
        </tr>
      </table>
</div>
<div class="memdoc">

<p>Prints out a JSON representation of a meta definition hashmap. </p>
<dl class="note"><dt><b>Note:</b></dt><dd>The text buffer must be allocated by <code><a class="el" href="group__libshare.html#gafac914ba170c0f2a9bc8c2add3a63a41" title="Initialize a memory buffer for use.">shbuf_init()</a></code> first. </dd></dl>
<dl><dt><b>Parameters:</b></dt><dd>
  <table border="0" cellspacing="2" cellpadding="0">
    <tr><td valign="top"></td><td valign="top"><em>h</em>&nbsp;</td><td>The meta map to print. </td></tr>
    <tr><td valign="top"></td><td valign="top"><em>ret_buff</em>&nbsp;</td><td>The text buffer to return the JSON string representation. </td></tr>
  </table>
  </dd>
</dl>

</div>
</div>
<a class="anchor" id="ga26c427c68302b1737d58449d8d93f629"></a><!-- doxytag: member="shmeta.h::shmeta_set" ref="ga26c427c68302b1737d58449d8d93f629" args="(shmeta_t *ht, shkey_t sh_k, shmeta_value_t *val)" -->
<div class="memitem">
<div class="memproto">
      <table class="memname">
        <tr>
          <td class="memname">void shmeta_set </td>
          <td>(</td>
          <td class="paramtype"><a class="el" href="structshmeta__t.html">shmeta_t</a> *&nbsp;</td>
          <td class="paramname"> <em>ht</em>, </td>
        </tr>
        <tr>
          <td class="paramkey"></td>
          <td></td>
          <td class="paramtype"><a class="el" href="group__libshare.html#ga315bac56fce878a38a2d91ca49c4c374">shkey_t</a>&nbsp;</td>
          <td class="paramname"> <em>sh_k</em>, </td>
        </tr>
        <tr>
          <td class="paramkey"></td>
          <td></td>
          <td class="paramtype"><a class="el" href="structshmeta__value__v1__t.html">shmeta_value_t</a> *&nbsp;</td>
          <td class="paramname"> <em>val</em></td><td>&nbsp;</td>
        </tr>
        <tr>
          <td></td>
          <td>)</td>
          <td></td><td></td><td></td>
        </tr>
      </table>
</div>
<div class="memdoc">

<p>Set a meta definition to a particular value. </p>
<dl><dt><b>Parameters:</b></dt><dd>
  <table border="0" cellspacing="2" cellpadding="0">
    <tr><td valign="top"></td><td valign="top"><em>ht</em>&nbsp;</td><td>The meta definition hashmap to retrieve from. </td></tr>
    <tr><td valign="top"></td><td valign="top"><em>sh_k</em>&nbsp;</td><td>The key of the meta definition value. </td></tr>
    <tr><td valign="top"></td><td valign="top"><em>val</em>&nbsp;</td><td>The meta definition value using a <code>shmeta_value_t</code> as a header. </td></tr>
  </table>
  </dd>
</dl>

</div>
</div>
<a class="anchor" id="gaa02d74e6f751797747941d593ef1547f"></a><!-- doxytag: member="shmeta_value.h::shmeta_str" ref="gaa02d74e6f751797747941d593ef1547f" args="(char *str)" -->
<div class="memitem">
<div class="memproto">
      <table class="memname">
        <tr>
          <td class="memname"><a class="el" href="structshmeta__value__v1__t.html">shmeta_value_t</a>* shmeta_str </td>
          <td>(</td>
          <td class="paramtype">char *&nbsp;</td>
          <td class="paramname"> <em>str</em></td>
          <td>&nbsp;)&nbsp;</td>
          <td></td>
        </tr>
      </table>
</div>
<div class="memdoc">

<p>Creates a <code>shmeta_value_t</code> hashmap value from a string. </p>
<p>shmeta_set </p>
<dl><dt><b>Parameters:</b></dt><dd>
  <table border="0" cellspacing="2" cellpadding="0">
    <tr><td valign="top"></td><td valign="top"><em>str</em>&nbsp;</td><td>The string to generated into a hashmap value. </td></tr>
  </table>
  </dd>
</dl>
<dl class="return"><dt><b>Returns:</b></dt><dd>A meta definition <code>shmeta_value_t</code> value. </dd></dl>

</div>
</div>
</div>

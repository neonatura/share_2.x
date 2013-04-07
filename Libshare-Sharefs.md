


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
<h1>The 'sharefs' file-system.<br/>
<small>
[<a class="el" href="group__libshare.html">Libshare</a>]</small>
</h1>
<p>The sharefs file system overlays ontop of your current filesystem in order to provide extended file operations.  
<a href="#_details">More...</a></p>
<table border="0" cellpadding="0" cellspacing="0">
<tr><td colspan="2"><h2>Data Structures</h2></td></tr>
<tr><td class="memItemLeft" align="right" valign="top">struct &nbsp;</td><td class="memItemRight" valign="bottom"><a class="el" href="structshfs__inode__hdr__t.html">shfs_inode_hdr_t</a></td></tr>
<tr><td class="memItemLeft" align="right" valign="top">struct &nbsp;</td><td class="memItemRight" valign="bottom"><a class="el" href="structshfs__hdr__t.html">shfs_hdr_t</a></td></tr>
<tr><td class="mdescLeft">&nbsp;</td><td class="mdescRight">A sharefs filesystem inode header.  <a href="structshfs__hdr__t.html#_details">More...</a><br/></td></tr>
<tr><td class="memItemLeft" align="right" valign="top">struct &nbsp;</td><td class="memItemRight" valign="bottom"><a class="el" href="structshfs__ino__t.html">shfs_ino_t</a></td></tr>
<tr><td class="mdescLeft">&nbsp;</td><td class="mdescRight">A sharefs filesystem inode.  <a href="structshfs__ino__t.html#_details">More...</a><br/></td></tr>
<tr><td class="memItemLeft" align="right" valign="top">struct &nbsp;</td><td class="memItemRight" valign="bottom"><a class="el" href="structshfs__t.html">shfs_t</a></td></tr>
<tr><td class="mdescLeft">&nbsp;</td><td class="mdescRight">The sharefs filesystem structure.  <a href="structshfs__t.html#_details">More...</a><br/></td></tr>
<tr><td class="memItemLeft" align="right" valign="top">struct &nbsp;</td><td class="memItemRight" valign="bottom"><a class="el" href="structshfs__journal__data__t.html">shfs_journal_data_t</a></td></tr>
<tr><td class="mdescLeft">&nbsp;</td><td class="mdescRight">A memory segment containing a journal's data.  <a href="structshfs__journal__data__t.html#_details">More...</a><br/></td></tr>
<tr><td class="memItemLeft" align="right" valign="top">struct &nbsp;</td><td class="memItemRight" valign="bottom"><a class="el" href="structshfs__journal__t.html">shfs_journal_t</a></td></tr>
<tr><td class="mdescLeft">&nbsp;</td><td class="mdescRight">A sharefs filesystem journal.  <a href="structshfs__journal__t.html#_details">More...</a><br/></td></tr>
<tr><td class="memItemLeft" align="right" valign="top">struct &nbsp;</td><td class="memItemRight" valign="bottom"><a class="el" href="structshrev__t.html">shrev_t</a></td></tr>
<tr><td class="mdescLeft">&nbsp;</td><td class="mdescRight">Describes a particular revision of a data segment.  <a href="structshrev__t.html#_details">More...</a><br/></td></tr>
<tr><td colspan="2"><h2>Defines</h2></td></tr>
<tr><td class="memItemLeft" align="right" valign="top"><a class="anchor" id="ga938ffbca7ed4bec6bfc1b4ce098ad307"></a><!-- doxytag: member="libshare_fs::SHFS_LEVEL_PUBLIC" ref="ga938ffbca7ed4bec6bfc1b4ce098ad307" args="" -->
#define&nbsp;</td><td class="memItemRight" valign="bottom"><b>SHFS_LEVEL_PUBLIC</b>&nbsp;&nbsp;&nbsp;0</td></tr>
<tr><td class="memItemLeft" align="right" valign="top"><a class="anchor" id="ga95763c19fc8a4a7cd2ce9daa099e5ed8"></a><!-- doxytag: member="libshare_fs::SHFS_MAX_LEVELS" ref="ga95763c19fc8a4a7cd2ce9daa099e5ed8" args="" -->
#define&nbsp;</td><td class="memItemRight" valign="bottom"><b>SHFS_MAX_LEVELS</b>&nbsp;&nbsp;&nbsp;1</td></tr>
<tr><td class="memItemLeft" align="right" valign="top"><a class="anchor" id="gac64541bdd81c961304b9babef1402640"></a><!-- doxytag: member="libshare_fs::NAME_MAX" ref="gac64541bdd81c961304b9babef1402640" args="" -->
#define&nbsp;</td><td class="memItemRight" valign="bottom"><b>NAME_MAX</b>&nbsp;&nbsp;&nbsp;4095</td></tr>
<tr><td class="memItemLeft" align="right" valign="top"><a class="anchor" id="gae688d728e1acdfe5988c7db45d6f0166"></a><!-- doxytag: member="libshare_fs::PATH_MAX" ref="gae688d728e1acdfe5988c7db45d6f0166" args="" -->
#define&nbsp;</td><td class="memItemRight" valign="bottom"><b>PATH_MAX</b>&nbsp;&nbsp;&nbsp;NAME_MAX</td></tr>
<tr><td class="memItemLeft" align="right" valign="top">#define&nbsp;</td><td class="memItemRight" valign="bottom"><a class="el" href="group__libshare__fs.html#gab69f59bea3a7e3124601e23c89ff1e25">SHFS_OVERLAY</a>&nbsp;&nbsp;&nbsp;(1 &lt;&lt; 0)</td></tr>
<tr><td class="mdescLeft">&nbsp;</td><td class="mdescRight">Overlay sharefs on top of current filesystem.  <a href="#gab69f59bea3a7e3124601e23c89ff1e25"></a><br/></td></tr>
<tr><td class="memItemLeft" align="right" valign="top">#define&nbsp;</td><td class="memItemRight" valign="bottom"><a class="el" href="group__libshare__fs.html#ga10a2f5d6b075e362dd6a8dd2c78f1d9e">SHFS_TRACK</a>&nbsp;&nbsp;&nbsp;(1 &lt;&lt; 1)</td></tr>
<tr><td class="mdescLeft">&nbsp;</td><td class="mdescRight">Track all revisions of file modifications.  <a href="#ga10a2f5d6b075e362dd6a8dd2c78f1d9e"></a><br/></td></tr>
<tr><td class="memItemLeft" align="right" valign="top">#define&nbsp;</td><td class="memItemRight" valign="bottom"><a class="el" href="group__libshare__fs.html#gaa85ad75a7a5722b59b0510189c84397f">SHFS_PRIVATE</a>&nbsp;&nbsp;&nbsp;(1 &lt;&lt; 2)</td></tr>
<tr><td class="mdescLeft">&nbsp;</td><td class="mdescRight">A sharefs filesystem that is externally unaccessible beyond the scope of this application.  <a href="#gaa85ad75a7a5722b59b0510189c84397f"></a><br/></td></tr>
<tr><td class="memItemLeft" align="right" valign="top"><a class="anchor" id="ga553e6e0e8b1e538fb58a6211770447ee"></a><!-- doxytag: member="libshare_fs::SHFS_SYNC" ref="ga553e6e0e8b1e538fb58a6211770447ee" args="" -->
#define&nbsp;</td><td class="memItemRight" valign="bottom"><a class="el" href="group__libshare__fs.html#ga553e6e0e8b1e538fb58a6211770447ee">SHFS_SYNC</a>&nbsp;&nbsp;&nbsp;(1 &lt;&lt; 3)</td></tr>
<tr><td class="mdescLeft">&nbsp;</td><td class="mdescRight">Disabling caching and asynchronous file operations. <br/></td></tr>
<tr><td class="memItemLeft" align="right" valign="top"><a class="anchor" id="gabccc61b0f2adbb6bf1923f96465435d6"></a><!-- doxytag: member="libshare_fs::SHFS_REMOTE" ref="gabccc61b0f2adbb6bf1923f96465435d6" args="" -->
#define&nbsp;</td><td class="memItemRight" valign="bottom"><a class="el" href="group__libshare__fs.html#gabccc61b0f2adbb6bf1923f96465435d6">SHFS_REMOTE</a>&nbsp;&nbsp;&nbsp;(1 &lt;&lt; 4)</td></tr>
<tr><td class="mdescLeft">&nbsp;</td><td class="mdescRight">The partition is located on a remote machine. <br/></td></tr>
<tr><td class="memItemLeft" align="right" valign="top">#define&nbsp;</td><td class="memItemRight" valign="bottom"><a class="el" href="group__libshare__fs.html#ga3b6387d0121c9b53ca2ab7b64bf562f7">SHINODE_APP</a>&nbsp;&nbsp;&nbsp;100</td></tr>
<tr><td class="mdescLeft">&nbsp;</td><td class="mdescRight">Inode is in reference to an application-specific directory.  <a href="#ga3b6387d0121c9b53ca2ab7b64bf562f7"></a><br/></td></tr>
<tr><td class="memItemLeft" align="right" valign="top">#define&nbsp;</td><td class="memItemRight" valign="bottom"><a class="el" href="group__libshare__fs.html#gae208dedcc607d946af45d314e61b49f5">SHINODE_PARTITION</a>&nbsp;&nbsp;&nbsp;101</td></tr>
<tr><td class="mdescLeft">&nbsp;</td><td class="mdescRight">Inode is the root of an entire sharefs partition.  <a href="#gae208dedcc607d946af45d314e61b49f5"></a><br/></td></tr>
<tr><td class="memItemLeft" align="right" valign="top">#define&nbsp;</td><td class="memItemRight" valign="bottom"><a class="el" href="group__libshare__fs.html#gaaa353fda426344c36955ae79e48a476c">SHINODE_PEER</a>&nbsp;&nbsp;&nbsp;102</td></tr>
<tr><td class="mdescLeft">&nbsp;</td><td class="mdescRight">Inode is a reference to a remote sharefs partition.  <a href="#gaaa353fda426344c36955ae79e48a476c"></a><br/></td></tr>
<tr><td class="memItemLeft" align="right" valign="top">#define&nbsp;</td><td class="memItemRight" valign="bottom"><a class="el" href="group__libshare__fs.html#gac91e09d8c655938646d617e796376ce7">SHINODE_DELTA</a>&nbsp;&nbsp;&nbsp;103</td></tr>
<tr><td class="mdescLeft">&nbsp;</td><td class="mdescRight">Inode is a reference to a binary delta revision.  <a href="#gac91e09d8c655938646d617e796376ce7"></a><br/></td></tr>
<tr><td class="memItemLeft" align="right" valign="top">#define&nbsp;</td><td class="memItemRight" valign="bottom"><a class="el" href="group__libshare__fs.html#ga05263e06afee937131ee9237d4d9d6ae">SHINODE_ARCHIVE</a>&nbsp;&nbsp;&nbsp;104</td></tr>
<tr><td class="mdescLeft">&nbsp;</td><td class="mdescRight">An archive of files and/or directories.  <a href="#ga05263e06afee937131ee9237d4d9d6ae"></a><br/></td></tr>
<tr><td class="memItemLeft" align="right" valign="top">#define&nbsp;</td><td class="memItemRight" valign="bottom"><a class="el" href="group__libshare__fs.html#ga533c7bab1475c3fe1968b95a876622ec">SHINODE_REFERENCE</a>&nbsp;&nbsp;&nbsp;105</td></tr>
<tr><td class="mdescLeft">&nbsp;</td><td class="mdescRight">A reference to another inode.  <a href="#ga533c7bab1475c3fe1968b95a876622ec"></a><br/></td></tr>
<tr><td class="memItemLeft" align="right" valign="top">#define&nbsp;</td><td class="memItemRight" valign="bottom"><a class="el" href="group__libshare__fs.html#gac272c3c37d0280fc1f6342367ce0de62">SHINODE_META</a>&nbsp;&nbsp;&nbsp;106</td></tr>
<tr><td class="mdescLeft">&nbsp;</td><td class="mdescRight">A meta definition hashmap (meta map).  <a href="#gac272c3c37d0280fc1f6342367ce0de62"></a><br/></td></tr>
<tr><td class="memItemLeft" align="right" valign="top"><a class="anchor" id="ga62bf7e2ff7a06bc708680c35be8147cd"></a><!-- doxytag: member="libshare_fs::SHINODE_DIRECTORY" ref="ga62bf7e2ff7a06bc708680c35be8147cd" args="" -->
#define&nbsp;</td><td class="memItemRight" valign="bottom"><a class="el" href="group__libshare__fs.html#ga62bf7e2ff7a06bc708680c35be8147cd">SHINODE_DIRECTORY</a>&nbsp;&nbsp;&nbsp;107</td></tr>
<tr><td class="mdescLeft">&nbsp;</td><td class="mdescRight">A directory containing multiple file references. <br/></td></tr>
<tr><td class="memItemLeft" align="right" valign="top">#define&nbsp;</td><td class="memItemRight" valign="bottom"><a class="el" href="group__libshare__fs.html#ga3c95adeae2a2c38c07c1ad38e38b9d52">SHFS_BLOCK_SIZE</a>&nbsp;&nbsp;&nbsp;1024</td></tr>
<tr><td class="mdescLeft">&nbsp;</td><td class="mdescRight">The maximum size a single block can contain.  <a href="#ga3c95adeae2a2c38c07c1ad38e38b9d52"></a><br/></td></tr>
<tr><td class="memItemLeft" align="right" valign="top">#define&nbsp;</td><td class="memItemRight" valign="bottom"><a class="el" href="group__libshare__fs.html#gafc2f59a63cee2ee46e2518ff9b249bfd">SHFS_BLOCK_DATA_SIZE</a>&nbsp;&nbsp;&nbsp;992</td></tr>
<tr><td class="mdescLeft">&nbsp;</td><td class="mdescRight">The size of the data segment each inode contains.  <a href="#gafc2f59a63cee2ee46e2518ff9b249bfd"></a><br/></td></tr>
<tr><td class="memItemLeft" align="right" valign="top"><a class="anchor" id="ga084ddc22301ef21fa71fa264a3dec544"></a><!-- doxytag: member="libshare_fs::SHFS_MAX_BLOCK" ref="ga084ddc22301ef21fa71fa264a3dec544" args="" -->
#define&nbsp;</td><td class="memItemRight" valign="bottom"><a class="el" href="group__libshare__fs.html#ga084ddc22301ef21fa71fa264a3dec544">SHFS_MAX_BLOCK</a>&nbsp;&nbsp;&nbsp;57344</td></tr>
<tr><td class="mdescLeft">&nbsp;</td><td class="mdescRight">The maximum number of blocks in a sharefs journal. <br/></td></tr>
<tr><td class="memItemLeft" align="right" valign="top">#define&nbsp;</td><td class="memItemRight" valign="bottom"><a class="el" href="group__libshare__fs.html#gaf398ee868884365b5944044329885e82">SHFS_MAX_JOURNAL</a>&nbsp;&nbsp;&nbsp;65536</td></tr>
<tr><td class="mdescLeft">&nbsp;</td><td class="mdescRight">The number of journals a sharefs filesystem contains.  <a href="#gaf398ee868884365b5944044329885e82"></a><br/></td></tr>
<tr><td class="memItemLeft" align="right" valign="top"><a class="anchor" id="ga8cbeb2464b33b307d25420adaae7342c"></a><!-- doxytag: member="libshare_fs::SHFS_MAX_JOURNAL_SIZE" ref="ga8cbeb2464b33b307d25420adaae7342c" args="" -->
#define&nbsp;</td><td class="memItemRight" valign="bottom"><a class="el" href="group__libshare__fs.html#ga8cbeb2464b33b307d25420adaae7342c">SHFS_MAX_JOURNAL_SIZE</a>&nbsp;&nbsp;&nbsp;(SHFS_MAX_BLOCK * SHFS_BLOCK_SIZE)</td></tr>
<tr><td class="mdescLeft">&nbsp;</td><td class="mdescRight">The maximum number of bytes in a sharefs file-system journal. <br/></td></tr>
<tr><td class="memItemLeft" align="right" valign="top">#define&nbsp;</td><td class="memItemRight" valign="bottom"><a class="el" href="group__libshare__fs.html#gab002cbbe5d0ca1ae7b085a0e526eae51">shfs_journal_index</a>(_inode)&nbsp;&nbsp;&nbsp;((<a class="el" href="group__libshare__fs.html#ga8b78f611c0595600d3941e9550b919eb">shfs_inode_off_t</a>)(shcrc(_inode-&gt;d_raw.name, NAME_MAX) % SHFS_MAX_JOURNAL))</td></tr>
<tr><td class="mdescLeft">&nbsp;</td><td class="mdescRight">Identify the default journal number for a inode's name.  <a href="#gab002cbbe5d0ca1ae7b085a0e526eae51"></a><br/></td></tr>
<tr><td class="memItemLeft" align="right" valign="top">#define&nbsp;</td><td class="memItemRight" valign="bottom"><a class="el" href="group__libshare__fs.html#ga4bfc4769e7ea3d8cceef55dffcdfa2b7">shfs_meta_free</a>(_meta_p)&nbsp;&nbsp;&nbsp;shmeta_free(_meta_p)</td></tr>
<tr><td class="mdescLeft">&nbsp;</td><td class="mdescRight">Free an instance to a sharedfs meta definition hashmap.  <a href="#ga4bfc4769e7ea3d8cceef55dffcdfa2b7"></a><br/></td></tr>
<tr><td colspan="2"><h2>Typedefs</h2></td></tr>
<tr><td class="memItemLeft" align="right" valign="top"><a class="anchor" id="ga3a648e59bbf7308fe1102f571b41cbe6"></a><!-- doxytag: member="libshare_fs::shfs_t" ref="ga3a648e59bbf7308fe1102f571b41cbe6" args="" -->
typedef struct <a class="el" href="structshfs__t.html">shfs_t</a>&nbsp;</td><td class="memItemRight" valign="bottom"><a class="el" href="group__libshare__fs.html#ga3a648e59bbf7308fe1102f571b41cbe6">shfs_t</a></td></tr>
<tr><td class="mdescLeft">&nbsp;</td><td class="mdescRight">A type defintion for the sharefs filesytem structure. <br/></td></tr>
<tr><td class="memItemLeft" align="right" valign="top"><a class="anchor" id="ga8b78f611c0595600d3941e9550b919eb"></a><!-- doxytag: member="libshare_fs::shfs_inode_off_t" ref="ga8b78f611c0595600d3941e9550b919eb" args="" -->
typedef __uint16_t&nbsp;</td><td class="memItemRight" valign="bottom"><a class="el" href="group__libshare__fs.html#ga8b78f611c0595600d3941e9550b919eb">shfs_inode_off_t</a></td></tr>
<tr><td class="mdescLeft">&nbsp;</td><td class="mdescRight">A sharefs filesystem inode or journal reference. <br/></td></tr>
<tr><td class="memItemLeft" align="right" valign="top"><a class="anchor" id="ga78f67ee32491298042ff55c27169da03"></a><!-- doxytag: member="libshare_fs::shfs_ino_type_t" ref="ga78f67ee32491298042ff55c27169da03" args="" -->
typedef __uint32_t&nbsp;</td><td class="memItemRight" valign="bottom"><a class="el" href="group__libshare__fs.html#ga78f67ee32491298042ff55c27169da03">shfs_ino_type_t</a></td></tr>
<tr><td class="mdescLeft">&nbsp;</td><td class="mdescRight">A sharefs inode type definition. <br/></td></tr>
<tr><td class="memItemLeft" align="right" valign="top"><a class="anchor" id="gad222e037ed9c1075ba83239f5926d4e0"></a><!-- doxytag: member="libshare_fs::shfs_size_t" ref="gad222e037ed9c1075ba83239f5926d4e0" args="" -->
typedef __uint64_t&nbsp;</td><td class="memItemRight" valign="bottom"><a class="el" href="group__libshare__fs.html#gad222e037ed9c1075ba83239f5926d4e0">shfs_size_t</a></td></tr>
<tr><td class="mdescLeft">&nbsp;</td><td class="mdescRight">A sharefs inode data size definition. <br/></td></tr>
<tr><td class="memItemLeft" align="right" valign="top"><a class="anchor" id="ga5d2fdab1ef6bc4307c27a82f2f8abcde"></a><!-- doxytag: member="libshare_fs::shfs_crc_t" ref="ga5d2fdab1ef6bc4307c27a82f2f8abcde" args="" -->
typedef __uint32_t&nbsp;</td><td class="memItemRight" valign="bottom"><a class="el" href="group__libshare__fs.html#ga5d2fdab1ef6bc4307c27a82f2f8abcde">shfs_crc_t</a></td></tr>
<tr><td class="mdescLeft">&nbsp;</td><td class="mdescRight">A sharefs inode data checksum type definition. <br/></td></tr>
<tr><td class="memItemLeft" align="right" valign="top">typedef uint8_t&nbsp;</td><td class="memItemRight" valign="bottom"><a class="el" href="group__libshare__fs.html#ga199a1e097e9098a687fed48175cdbda3">shfs_block_t</a> [1024]</td></tr>
<tr><td class="mdescLeft">&nbsp;</td><td class="mdescRight">A single block of data inside a journal.  <a href="#ga199a1e097e9098a687fed48175cdbda3"></a><br/></td></tr>
<tr><td colspan="2"><h2>Functions</h2></td></tr>
<tr><td class="memItemLeft" align="right" valign="top">char *&nbsp;</td><td class="memItemRight" valign="bottom"><a class="el" href="group__libshare__fs.html#ga9622da70b278afed11d40cdc206c4b1b">shfs_app_name</a> (char *app_name)</td></tr>
<tr><td class="mdescLeft">&nbsp;</td><td class="mdescRight">Strips the absolute parent from <em>app_name</em>.  <a href="#ga9622da70b278afed11d40cdc206c4b1b"></a><br/></td></tr>
<tr><td class="memItemLeft" align="right" valign="top"><a class="el" href="structshfs__t.html">shfs_t</a> *&nbsp;</td><td class="memItemRight" valign="bottom"><a class="el" href="group__libshare__fs.html#ga7f8fbad7732af7b76a3e056172365680">shfs_init</a> (char *app_name, int flags)</td></tr>
<tr><td class="mdescLeft">&nbsp;</td><td class="mdescRight">Creates a reference to a sharefs filesystem.  <a href="#ga7f8fbad7732af7b76a3e056172365680"></a><br/></td></tr>
<tr><td class="memItemLeft" align="right" valign="top">void&nbsp;</td><td class="memItemRight" valign="bottom"><a class="el" href="group__libshare__fs.html#ga63ae4952fe99614b3fc4821c23485c74">shfs_free</a> (<a class="el" href="structshfs__t.html">shfs_t</a> **tree_p)</td></tr>
<tr><td class="mdescLeft">&nbsp;</td><td class="mdescRight">Free a reference to a sharefs partition.  <a href="#ga63ae4952fe99614b3fc4821c23485c74"></a><br/></td></tr>
<tr><td class="memItemLeft" align="right" valign="top"><a class="el" href="structshfs__ino__t.html">shfs_ino_t</a> *&nbsp;</td><td class="memItemRight" valign="bottom"><a class="el" href="group__libshare__fs.html#gae2058cc7d0edcdab4953e2a25eb0be4a">shfs_inode</a> (<a class="el" href="structshfs__ino__t.html">shfs_ino_t</a> *parent, char *name, int mode)</td></tr>
<tr><td class="mdescLeft">&nbsp;</td><td class="mdescRight">Retrieve a sharefs inode directory entry based on a given parent inode and path name.  <a href="#gae2058cc7d0edcdab4953e2a25eb0be4a"></a><br/></td></tr>
<tr><td class="memItemLeft" align="right" valign="top"><a class="anchor" id="gad498f525bda53dea7fdc119085bfa0bf"></a><!-- doxytag: member="libshare_fs::shfs_inode_write_block" ref="gad498f525bda53dea7fdc119085bfa0bf" args="(shfs_t *tree, shfs_hdr_t *scan_hdr, shfs_hdr_t *hdr, char *data, size_t data_len)" -->
int&nbsp;</td><td class="memItemRight" valign="bottom"><a class="el" href="group__libshare__fs.html#gad498f525bda53dea7fdc119085bfa0bf">shfs_inode_write_block</a> (<a class="el" href="structshfs__t.html">shfs_t</a> *tree, <a class="el" href="structshfs__hdr__t.html">shfs_hdr_t</a> *scan_hdr, <a class="el" href="structshfs__hdr__t.html">shfs_hdr_t</a> *hdr, char *data, size_t data_len)</td></tr>
<tr><td class="mdescLeft">&nbsp;</td><td class="mdescRight">Writes a single inode block to a sharefs filesystem journal. <br/></td></tr>
<tr><td class="memItemLeft" align="right" valign="top">ssize_t&nbsp;</td><td class="memItemRight" valign="bottom"><a class="el" href="group__libshare__fs.html#gae7450b1c214a9b72f3287c6dc75a8f80">shfs_inode_write</a> (<a class="el" href="structshfs__t.html">shfs_t</a> *tree, <a class="el" href="structshfs__ino__t.html">shfs_ino_t</a> *inode, char *data, <a class="el" href="group__libshare__fs.html#gad222e037ed9c1075ba83239f5926d4e0">shfs_size_t</a> data_of, <a class="el" href="group__libshare__fs.html#gad222e037ed9c1075ba83239f5926d4e0">shfs_size_t</a> data_len)</td></tr>
<tr><td class="mdescLeft">&nbsp;</td><td class="mdescRight">Stores a data segment to a sharefs filesystem inode.  <a href="#gae7450b1c214a9b72f3287c6dc75a8f80"></a><br/></td></tr>
<tr><td class="memItemLeft" align="right" valign="top">int&nbsp;</td><td class="memItemRight" valign="bottom"><a class="el" href="group__libshare__fs.html#ga2e3935bfd16c44b1342e4453a4a2a77d">shfs_inode_read_block</a> (<a class="el" href="structshfs__t.html">shfs_t</a> *tree, <a class="el" href="structshfs__hdr__t.html">shfs_hdr_t</a> *hdr, <a class="el" href="structshfs__ino__t.html">shfs_ino_t</a> *inode)</td></tr>
<tr><td class="mdescLeft">&nbsp;</td><td class="mdescRight">Retrieve a single data block from a sharefs filesystem inode.  <a href="#ga2e3935bfd16c44b1342e4453a4a2a77d"></a><br/></td></tr>
<tr><td class="memItemLeft" align="right" valign="top">ssize_t&nbsp;</td><td class="memItemRight" valign="bottom"><a class="el" href="group__libshare__fs.html#ga87461a63830557b4cc3d409a2c6968a8">shfs_inode_read</a> (<a class="el" href="structshfs__t.html">shfs_t</a> *tree, <a class="el" href="structshfs__ino__t.html">shfs_ino_t</a> *inode, <a class="el" href="structshbuf__t.html">shbuf_t</a> *ret_buff, size_t data_of, size_t data_len)</td></tr>
<tr><td class="mdescLeft">&nbsp;</td><td class="mdescRight">Retrieve a data segment of a sharefs filesystem inode.  <a href="#ga87461a63830557b4cc3d409a2c6968a8"></a><br/></td></tr>
<tr><td class="memItemLeft" align="right" valign="top"><a class="anchor" id="ga39280cc9143a14b44b257211fb358350"></a><!-- doxytag: member="libshare_fs::shfs_journal_path" ref="ga39280cc9143a14b44b257211fb358350" args="(shfs_t *tree, int index)" -->
char *&nbsp;</td><td class="memItemRight" valign="bottom"><a class="el" href="group__libshare__fs.html#ga39280cc9143a14b44b257211fb358350">shfs_journal_path</a> (<a class="el" href="structshfs__t.html">shfs_t</a> *tree, int index)</td></tr>
<tr><td class="mdescLeft">&nbsp;</td><td class="mdescRight">The local file-system path where a sharefs journal is stored. <br/></td></tr>
<tr><td class="memItemLeft" align="right" valign="top"><a class="anchor" id="ga4407d17c0f770ca5ab568951d5d7a20b"></a><!-- doxytag: member="libshare_fs::shfs_journal_open" ref="ga4407d17c0f770ca5ab568951d5d7a20b" args="(shfs_t *tree, int index)" -->
<a class="el" href="structshfs__journal__t.html">shfs_journal_t</a> *&nbsp;</td><td class="memItemRight" valign="bottom"><a class="el" href="group__libshare__fs.html#ga4407d17c0f770ca5ab568951d5d7a20b">shfs_journal_open</a> (<a class="el" href="structshfs__t.html">shfs_t</a> *tree, int index)</td></tr>
<tr><td class="mdescLeft">&nbsp;</td><td class="mdescRight">Returns an instance to a sharefs filesystem journal. <br/></td></tr>
<tr><td class="memItemLeft" align="right" valign="top">void&nbsp;</td><td class="memItemRight" valign="bottom"><a class="el" href="group__libshare__fs.html#ga2f67b30a6c6913333b0b6a0a194132db">shfs_journal_free</a> (<a class="el" href="structshfs__t.html">shfs_t</a> *tree, <a class="el" href="structshfs__journal__t.html">shfs_journal_t</a> **jrnl_p)</td></tr>
<tr><td class="mdescLeft">&nbsp;</td><td class="mdescRight">Initializes a sharefs filesystem journal for use.  <a href="#ga2f67b30a6c6913333b0b6a0a194132db"></a><br/></td></tr>
<tr><td class="memItemLeft" align="right" valign="top">int&nbsp;</td><td class="memItemRight" valign="bottom"><a class="el" href="group__libshare__fs.html#ga587c3da4d703e7d3652d5279bc5c59f5">shfs_journal_scan</a> (<a class="el" href="structshfs__t.html">shfs_t</a> *tree, int jno)</td></tr>
<tr><td class="mdescLeft">&nbsp;</td><td class="mdescRight">Search for the first empty inode entry in a journal.  <a href="#ga587c3da4d703e7d3652d5279bc5c59f5"></a><br/></td></tr>
<tr><td class="memItemLeft" align="right" valign="top">int&nbsp;</td><td class="memItemRight" valign="bottom"><a class="el" href="group__libshare__fs.html#gad3d3a478684d1f33e3813f8b13d90b61">shfs_journal_write</a> (<a class="el" href="structshfs__t.html">shfs_t</a> *tree, <a class="el" href="structshfs__journal__t.html">shfs_journal_t</a> *jrnl)</td></tr>
<tr><td class="mdescLeft">&nbsp;</td><td class="mdescRight">Sync a sharefs journal to the local file-system.  <a href="#gad3d3a478684d1f33e3813f8b13d90b61"></a><br/></td></tr>
<tr><td class="memItemLeft" align="right" valign="top">int&nbsp;</td><td class="memItemRight" valign="bottom"><a class="el" href="group__libshare__fs.html#gac54920a0bddc3e8c589910f8c83ce230">shfs_meta</a> (<a class="el" href="structshfs__t.html">shfs_t</a> *tree, <a class="el" href="structshfs__ino__t.html">shfs_ino_t</a> *ent, <a class="el" href="structshmeta__t.html">shmeta_t</a> **val_p)</td></tr>
<tr><td class="mdescLeft">&nbsp;</td><td class="mdescRight">Obtain a reference to the meta definition hashmap associated with the inode entry.  <a href="#gac54920a0bddc3e8c589910f8c83ce230"></a><br/></td></tr>
<tr><td class="memItemLeft" align="right" valign="top">int&nbsp;</td><td class="memItemRight" valign="bottom"><a class="el" href="group__libshare__fs.html#ga51eeed961236c0f20c8dc8e8c3ae183e">shfs_meta_save</a> (<a class="el" href="structshfs__t.html">shfs_t</a> *tree, <a class="el" href="structshfs__ino__t.html">shfs_ino_t</a> *ent, <a class="el" href="structshmeta__t.html">shmeta_t</a> *h)</td></tr>
<tr><td class="mdescLeft">&nbsp;</td><td class="mdescRight">Flush the inode's meta map to disk.  <a href="#ga51eeed961236c0f20c8dc8e8c3ae183e"></a><br/></td></tr>
<tr><td class="memItemLeft" align="right" valign="top">int&nbsp;</td><td class="memItemRight" valign="bottom"><a class="el" href="group__libshare__fs.html#ga5b810eff4fc98882fc93f4f2b6fb7674">shfs_proc_lock</a> (char *process_path, char *runtime_mode)</td></tr>
<tr><td class="mdescLeft">&nbsp;</td><td class="mdescRight">Obtain an exclusive lock to a process with the same <code>process_path</code> and <code>runtime_mode</code>.  <a href="#ga5b810eff4fc98882fc93f4f2b6fb7674"></a><br/></td></tr>
<tr><td class="memItemLeft" align="right" valign="top"><a class="anchor" id="ga798a665577cac11875cf67616059c067"></a><!-- doxytag: member="libshare_fs::shfs_read_mem" ref="ga798a665577cac11875cf67616059c067" args="(char *path, char **data_p, size_t *data_len_p)" -->
int&nbsp;</td><td class="memItemRight" valign="bottom"><a class="el" href="group__libshare__fs.html#ga798a665577cac11875cf67616059c067">shfs_read_mem</a> (char *path, char **data_p, size_t *data_len_p)</td></tr>
<tr><td class="mdescLeft">&nbsp;</td><td class="mdescRight">Read a file from the local filesystem into memory. <br/></td></tr>
<tr><td class="memItemLeft" align="right" valign="top"><a class="anchor" id="ga5c5c0246ca52adbe670c48a3d398c73e"></a><!-- doxytag: member="libshare_fs::shfs_write_mem" ref="ga5c5c0246ca52adbe670c48a3d398c73e" args="(char *path, void *data, size_t data_len)" -->
int&nbsp;</td><td class="memItemRight" valign="bottom"><b>shfs_write_mem</b> (char *path, void *data, size_t data_len)</td></tr>
<tr><td class="memItemLeft" align="right" valign="top">int&nbsp;</td><td class="memItemRight" valign="bottom"><a class="el" href="group__libshare__fs.html#ga2ab916536afac9c7029cdacf921b764b">shfs_write_print</a> (<a class="el" href="structshfs__t.html">shfs_t</a> *tree, <a class="el" href="structshfs__ino__t.html">shfs_ino_t</a> *inode, int fd)</td></tr>
<tr><td class="mdescLeft">&nbsp;</td><td class="mdescRight">Writes the file contents of the inode to the file stream.  <a href="#ga2ab916536afac9c7029cdacf921b764b"></a><br/></td></tr>
</table>
<hr/><a name="_details"></a><h2>Detailed Description</h2>
<p>The sharefs file system overlays ontop of your current filesystem in order to provide extended file operations. </p>
<p>libshare_fs_inode The 'sharefs' inode sub-system.</p>
<p>Filesystem Modes  libshare_fs_mode The sharefs file system modes.</p>
<p>The sharefs file system. </p>
<hr/><h2>Define Documentation</h2>
<a class="anchor" id="gafc2f59a63cee2ee46e2518ff9b249bfd"></a><!-- doxytag: member="shfs.h::SHFS_BLOCK_DATA_SIZE" ref="gafc2f59a63cee2ee46e2518ff9b249bfd" args="" -->
<div class="memitem">
<div class="memproto">
      <table class="memname">
        <tr>
          <td class="memname">#define SHFS_BLOCK_DATA_SIZE&nbsp;&nbsp;&nbsp;992</td>
        </tr>
      </table>
</div>
<div class="memdoc">

<p>The size of the data segment each inode contains. </p>
<dl class="note"><dt><b>Note:</b></dt><dd>992 = (<code>SHFS_BLOCK_SIZE</code> - sizeof(<code><a class="el" href="structshfs__hdr__t.html" title="A sharefs filesystem inode header.">shfs_hdr_t</a></code>)) </dd></dl>

<p>Definition at line <a class="el" href="shfs_8h_source.html#l00162">162</a> of file <a class="el" href="shfs_8h_source.html">shfs.h</a>.</p>

</div>
</div>
<a class="anchor" id="ga3c95adeae2a2c38c07c1ad38e38b9d52"></a><!-- doxytag: member="shfs.h::SHFS_BLOCK_SIZE" ref="ga3c95adeae2a2c38c07c1ad38e38b9d52" args="" -->
<div class="memitem">
<div class="memproto">
      <table class="memname">
        <tr>
          <td class="memname">#define SHFS_BLOCK_SIZE&nbsp;&nbsp;&nbsp;1024</td>
        </tr>
      </table>
</div>
<div class="memdoc">

<p>The maximum size a single block can contain. </p>
<dl class="note"><dt><b>Note:</b></dt><dd>Each block segment is 1024 bytes which is equal to the size of <code><a class="el" href="structshfs__ino__t.html" title="A sharefs filesystem inode.">shfs_ino_t</a></code> structure. Blocks are kept at 1k in order to reduce overhead on the IP protocol. </dd></dl>

<p>Definition at line <a class="el" href="shfs_8h_source.html#l00156">156</a> of file <a class="el" href="shfs_8h_source.html">shfs.h</a>.</p>

</div>
</div>
<a class="anchor" id="gab002cbbe5d0ca1ae7b085a0e526eae51"></a><!-- doxytag: member="shfs_journal.h::shfs_journal_index" ref="gab002cbbe5d0ca1ae7b085a0e526eae51" args="(_inode)" -->
<div class="memitem">
<div class="memproto">
      <table class="memname">
        <tr>
          <td class="memname">#define shfs_journal_index</td>
          <td>(</td>
          <td class="paramtype">_inode&nbsp;</td>
          <td class="paramname"></td>
          <td>&nbsp;)&nbsp;</td>
          <td>&nbsp;&nbsp;&nbsp;((<a class="el" href="group__libshare__fs.html#ga8b78f611c0595600d3941e9550b919eb">shfs_inode_off_t</a>)(shcrc(_inode-&gt;d_raw.name, NAME_MAX) % SHFS_MAX_JOURNAL))</td>
        </tr>
      </table>
</div>
<div class="memdoc">

<p>Identify the default journal number for a inode's name. </p>
<dl class="return"><dt><b>Returns:</b></dt><dd>A sharefs filesystem journal index number. </dd></dl>

<p>Definition at line <a class="el" href="shfs__journal_8h_source.html#l00113">113</a> of file <a class="el" href="shfs__journal_8h_source.html">shfs_journal.h</a>.</p>

</div>
</div>
<a class="anchor" id="gaf398ee868884365b5944044329885e82"></a><!-- doxytag: member="shfs_journal.h::SHFS_MAX_JOURNAL" ref="gaf398ee868884365b5944044329885e82" args="" -->
<div class="memitem">
<div class="memproto">
      <table class="memname">
        <tr>
          <td class="memname">#define SHFS_MAX_JOURNAL&nbsp;&nbsp;&nbsp;65536</td>
        </tr>
      </table>
</div>
<div class="memdoc">

<p>The number of journals a sharefs filesystem contains. </p>
<p><a class="el" href="structshfs__journal__t.html#aa57dd3556ed79f58879abf613a9bebd6" title="The index number of the journal.">shfs_journal_t.index</a> </p>

<p>Definition at line <a class="el" href="shfs__journal_8h_source.html#l00039">39</a> of file <a class="el" href="shfs__journal_8h_source.html">shfs_journal.h</a>.</p>

</div>
</div>
<a class="anchor" id="ga4bfc4769e7ea3d8cceef55dffcdfa2b7"></a><!-- doxytag: member="shfs_meta.h::shfs_meta_free" ref="ga4bfc4769e7ea3d8cceef55dffcdfa2b7" args="(_meta_p)" -->
<div class="memitem">
<div class="memproto">
      <table class="memname">
        <tr>
          <td class="memname">#define shfs_meta_free</td>
          <td>(</td>
          <td class="paramtype">_meta_p&nbsp;</td>
          <td class="paramname"></td>
          <td>&nbsp;)&nbsp;</td>
          <td>&nbsp;&nbsp;&nbsp;shmeta_free(_meta_p)</td>
        </tr>
      </table>
</div>
<div class="memdoc">

<p>Free an instance to a sharedfs meta definition hashmap. </p>
<dl class="note"><dt><b>Note:</b></dt><dd>Directly calls <code><a class="el" href="group__libshare__meta.html#ga08eb531a3201e24de9049743ce0baae0" title="Free an instance of a meta definition hashmap.">shmeta_free()</a></code>. </dd></dl>

<p>Definition at line <a class="el" href="shfs__meta_8h_source.html#l00056">56</a> of file <a class="el" href="shfs__meta_8h_source.html">shfs_meta.h</a>.</p>

</div>
</div>
<a class="anchor" id="gab69f59bea3a7e3124601e23c89ff1e25"></a><!-- doxytag: member="shfs.h::SHFS_OVERLAY" ref="gab69f59bea3a7e3124601e23c89ff1e25" args="" -->
<div class="memitem">
<div class="memproto">
      <table class="memname">
        <tr>
          <td class="memname">#define SHFS_OVERLAY&nbsp;&nbsp;&nbsp;(1 &lt;&lt; 0)</td>
        </tr>
      </table>
</div>
<div class="memdoc">

<p>Overlay sharefs on top of current filesystem. </p>
<dl class="note"><dt><b>Note:</b></dt><dd>Use 'shnet --nosync' for example behavior of this flag. </dd></dl>

<p>Definition at line <a class="el" href="shfs_8h_source.html#l00061">61</a> of file <a class="el" href="shfs_8h_source.html">shfs.h</a>.</p>

</div>
</div>
<a class="anchor" id="gaa85ad75a7a5722b59b0510189c84397f"></a><!-- doxytag: member="shfs.h::SHFS_PRIVATE" ref="gaa85ad75a7a5722b59b0510189c84397f" args="" -->
<div class="memitem">
<div class="memproto">
      <table class="memname">
        <tr>
          <td class="memname">#define SHFS_PRIVATE&nbsp;&nbsp;&nbsp;(1 &lt;&lt; 2)</td>
        </tr>
      </table>
</div>
<div class="memdoc">

<p>A sharefs filesystem that is externally unaccessible beyond the scope of this application. </p>
<dl class="note"><dt><b>Note:</b></dt><dd>Use 'shnet --hidden' for example behavior of this flag. </dd></dl>

<p>Definition at line <a class="el" href="shfs_8h_source.html#l00074">74</a> of file <a class="el" href="shfs_8h_source.html">shfs.h</a>.</p>

</div>
</div>
<a class="anchor" id="ga10a2f5d6b075e362dd6a8dd2c78f1d9e"></a><!-- doxytag: member="shfs.h::SHFS_TRACK" ref="ga10a2f5d6b075e362dd6a8dd2c78f1d9e" args="" -->
<div class="memitem">
<div class="memproto">
      <table class="memname">
        <tr>
          <td class="memname">#define SHFS_TRACK&nbsp;&nbsp;&nbsp;(1 &lt;&lt; 1)</td>
        </tr>
      </table>
</div>
<div class="memdoc">

<p>Track all revisions of file modifications. </p>
<dl class="note"><dt><b>Note:</b></dt><dd>Use 'shnet --track' for example behavior of this flag. </dd></dl>

<p>Definition at line <a class="el" href="shfs_8h_source.html#l00067">67</a> of file <a class="el" href="shfs_8h_source.html">shfs.h</a>.</p>

</div>
</div>
<a class="anchor" id="ga3b6387d0121c9b53ca2ab7b64bf562f7"></a><!-- doxytag: member="shfs.h::SHINODE_APP" ref="ga3b6387d0121c9b53ca2ab7b64bf562f7" args="" -->
<div class="memitem">
<div class="memproto">
      <table class="memname">
        <tr>
          <td class="memname">#define SHINODE_APP&nbsp;&nbsp;&nbsp;100</td>
        </tr>
      </table>
</div>
<div class="memdoc">

<p>Inode is in reference to an application-specific directory. </p>
<dl class="note"><dt><b>Note:</b></dt><dd>See also: <code>shfs_node.d_type</code> </dd></dl>

<p>Definition at line <a class="el" href="shfs_8h_source.html#l00109">109</a> of file <a class="el" href="shfs_8h_source.html">shfs.h</a>.</p>

</div>
</div>
<a class="anchor" id="ga05263e06afee937131ee9237d4d9d6ae"></a><!-- doxytag: member="shfs.h::SHINODE_ARCHIVE" ref="ga05263e06afee937131ee9237d4d9d6ae" args="" -->
<div class="memitem">
<div class="memproto">
      <table class="memname">
        <tr>
          <td class="memname">#define SHINODE_ARCHIVE&nbsp;&nbsp;&nbsp;104</td>
        </tr>
      </table>
</div>
<div class="memdoc">

<p>An archive of files and/or directories. </p>
<dl class="note"><dt><b>Note:</b></dt><dd>See also: <code>shfs_node.d_type</code> </dd></dl>

<p>Definition at line <a class="el" href="shfs_8h_source.html#l00133">133</a> of file <a class="el" href="shfs_8h_source.html">shfs.h</a>.</p>

</div>
</div>
<a class="anchor" id="gac91e09d8c655938646d617e796376ce7"></a><!-- doxytag: member="shfs.h::SHINODE_DELTA" ref="gac91e09d8c655938646d617e796376ce7" args="" -->
<div class="memitem">
<div class="memproto">
      <table class="memname">
        <tr>
          <td class="memname">#define SHINODE_DELTA&nbsp;&nbsp;&nbsp;103</td>
        </tr>
      </table>
</div>
<div class="memdoc">

<p>Inode is a reference to a binary delta revision. </p>
<dl class="note"><dt><b>Note:</b></dt><dd>See also: <code>shfs_node.d_type</code> </dd></dl>

<p>Definition at line <a class="el" href="shfs_8h_source.html#l00127">127</a> of file <a class="el" href="shfs_8h_source.html">shfs.h</a>.</p>

</div>
</div>
<a class="anchor" id="gac272c3c37d0280fc1f6342367ce0de62"></a><!-- doxytag: member="shfs.h::SHINODE_META" ref="gac272c3c37d0280fc1f6342367ce0de62" args="" -->
<div class="memitem">
<div class="memproto">
      <table class="memname">
        <tr>
          <td class="memname">#define SHINODE_META&nbsp;&nbsp;&nbsp;106</td>
        </tr>
      </table>
</div>
<div class="memdoc">

<p>A meta definition hashmap (meta map). </p>
<dl class="note"><dt><b>Note:</b></dt><dd>The referenced inode may be local or remote. </dd></dl>

<p>Definition at line <a class="el" href="shfs_8h_source.html#l00145">145</a> of file <a class="el" href="shfs_8h_source.html">shfs.h</a>.</p>

</div>
</div>
<a class="anchor" id="gae208dedcc607d946af45d314e61b49f5"></a><!-- doxytag: member="shfs.h::SHINODE_PARTITION" ref="gae208dedcc607d946af45d314e61b49f5" args="" -->
<div class="memitem">
<div class="memproto">
      <table class="memname">
        <tr>
          <td class="memname">#define SHINODE_PARTITION&nbsp;&nbsp;&nbsp;101</td>
        </tr>
      </table>
</div>
<div class="memdoc">

<p>Inode is the root of an entire sharefs partition. </p>
<dl class="note"><dt><b>Note:</b></dt><dd>See also: <code>shfs_node.d_type</code> </dd></dl>

<p>Definition at line <a class="el" href="shfs_8h_source.html#l00115">115</a> of file <a class="el" href="shfs_8h_source.html">shfs.h</a>.</p>

</div>
</div>
<a class="anchor" id="gaaa353fda426344c36955ae79e48a476c"></a><!-- doxytag: member="shfs.h::SHINODE_PEER" ref="gaaa353fda426344c36955ae79e48a476c" args="" -->
<div class="memitem">
<div class="memproto">
      <table class="memname">
        <tr>
          <td class="memname">#define SHINODE_PEER&nbsp;&nbsp;&nbsp;102</td>
        </tr>
      </table>
</div>
<div class="memdoc">

<p>Inode is a reference to a remote sharefs partition. </p>
<dl class="note"><dt><b>Note:</b></dt><dd>See also: <code>shfs_node.d_type</code> </dd></dl>

<p>Definition at line <a class="el" href="shfs_8h_source.html#l00121">121</a> of file <a class="el" href="shfs_8h_source.html">shfs.h</a>.</p>

</div>
</div>
<a class="anchor" id="ga533c7bab1475c3fe1968b95a876622ec"></a><!-- doxytag: member="shfs.h::SHINODE_REFERENCE" ref="ga533c7bab1475c3fe1968b95a876622ec" args="" -->
<div class="memitem">
<div class="memproto">
      <table class="memname">
        <tr>
          <td class="memname">#define SHINODE_REFERENCE&nbsp;&nbsp;&nbsp;105</td>
        </tr>
      </table>
</div>
<div class="memdoc">

<p>A reference to another inode. </p>
<dl class="note"><dt><b>Note:</b></dt><dd>The referenced inode may be local or remote. </dd></dl>
<dl><dt><b>Examples: </b></dt><dd><a class="el" href="shfs__inode__remote__link_8c-example.html#a5">shfs_inode_remote_link.c</a>.</dd>
</dl>
<p>Definition at line <a class="el" href="shfs_8h_source.html#l00139">139</a> of file <a class="el" href="shfs_8h_source.html">shfs.h</a>.</p>

</div>
</div>
<hr/><h2>Typedef Documentation</h2>
<a class="anchor" id="ga199a1e097e9098a687fed48175cdbda3"></a><!-- doxytag: member="shfs_journal.h::shfs_block_t" ref="ga199a1e097e9098a687fed48175cdbda3" args="[1024]" -->
<div class="memitem">
<div class="memproto">
      <table class="memname">
        <tr>
          <td class="memname">typedef uint8_t <a class="el" href="group__libshare__fs.html#ga199a1e097e9098a687fed48175cdbda3">shfs_block_t</a>[1024]</td>
        </tr>
      </table>
</div>
<div class="memdoc">

<p>A single block of data inside a journal. </p>
<p><a class="el" href="structshfs__journal__t.html#a1bba79a77543569e36f2dc943c094383" title="The data segment of the journaled sharefs file system.">shfs_journal_t.data</a> </p>

<p>Definition at line <a class="el" href="shfs__journal_8h_source.html#l00050">50</a> of file <a class="el" href="shfs__journal_8h_source.html">shfs_journal.h</a>.</p>

</div>
</div>
<hr/><h2>Function Documentation</h2>
<a class="anchor" id="ga9622da70b278afed11d40cdc206c4b1b"></a><!-- doxytag: member="shfs.h::shfs_app_name" ref="ga9622da70b278afed11d40cdc206c4b1b" args="(char *app_name)" -->
<div class="memitem">
<div class="memproto">
      <table class="memname">
        <tr>
          <td class="memname">char* shfs_app_name </td>
          <td>(</td>
          <td class="paramtype">char *&nbsp;</td>
          <td class="paramname"> <em>app_name</em></td>
          <td>&nbsp;)&nbsp;</td>
          <td></td>
        </tr>
      </table>
</div>
<div class="memdoc">

<p>Strips the absolute parent from <em>app_name</em>. </p>
<dl class="note"><dt><b>Note:</b></dt><dd>"/test/one/two" becomes "two" </dd></dl>
<dl><dt><b>Parameters:</b></dt><dd>
  <table border="0" cellspacing="2" cellpadding="0">
    <tr><td valign="top"></td><td valign="top"><em>app_name</em>&nbsp;</td><td>The running application's executable path </td></tr>
  </table>
  </dd>
</dl>
<dl class="return"><dt><b>Returns:</b></dt><dd>Relative filename of executable. </dd></dl>

</div>
</div>
<a class="anchor" id="ga63ae4952fe99614b3fc4821c23485c74"></a><!-- doxytag: member="shfs.h::shfs_free" ref="ga63ae4952fe99614b3fc4821c23485c74" args="(shfs_t **tree_p)" -->
<div class="memitem">
<div class="memproto">
      <table class="memname">
        <tr>
          <td class="memname">void shfs_free </td>
          <td>(</td>
          <td class="paramtype"><a class="el" href="structshfs__t.html">shfs_t</a> **&nbsp;</td>
          <td class="paramname"> <em>tree_p</em></td>
          <td>&nbsp;)&nbsp;</td>
          <td></td>
        </tr>
      </table>
</div>
<div class="memdoc">

<p>Free a reference to a sharefs partition. </p>
<dl><dt><b>Parameters:</b></dt><dd>
  <table border="0" cellspacing="2" cellpadding="0">
    <tr><td valign="top"></td><td valign="top"><em>tree_p</em>&nbsp;</td><td>A reference to the sharefs partition instance to free. </td></tr>
  </table>
  </dd>
</dl>
<dl><dt><b>Examples: </b></dt><dd><a class="el" href="shfs__inode__remote__link_8c-example.html#a7">shfs_inode_remote_link.c</a>.</dd>
</dl>
</div>
</div>
<a class="anchor" id="ga7f8fbad7732af7b76a3e056172365680"></a><!-- doxytag: member="shfs.h::shfs_init" ref="ga7f8fbad7732af7b76a3e056172365680" args="(char *app_name, int flags)" -->
<div class="memitem">
<div class="memproto">
      <table class="memname">
        <tr>
          <td class="memname"><a class="el" href="structshfs__t.html">shfs_t</a>* shfs_init </td>
          <td>(</td>
          <td class="paramtype">char *&nbsp;</td>
          <td class="paramname"> <em>app_name</em>, </td>
        </tr>
        <tr>
          <td class="paramkey"></td>
          <td></td>
          <td class="paramtype">int&nbsp;</td>
          <td class="paramname"> <em>flags</em></td><td>&nbsp;</td>
        </tr>
        <tr>
          <td></td>
          <td>)</td>
          <td></td><td></td><td></td>
        </tr>
      </table>
</div>
<div class="memdoc">

<p>Creates a reference to a sharefs filesystem. </p>
<p><em>app_name</em> The application's executable name. <em>flags</em> A combination of SHFS_XXX flags. </p>
<dl class="return"><dt><b>Returns:</b></dt><dd><a class="el" href="structshfs__t.html" title="The sharefs filesystem structure.">shfs_t</a> The sharefs filesystem. </dd></dl>
<dl><dt><b>Examples: </b></dt><dd><a class="el" href="shfs__inode__mkdir_8c-example.html#a2">shfs_inode_mkdir.c</a>, <a class="el" href="shfs__inode__remote__copy_8c-example.html#a2">shfs_inode_remote_copy.c</a>, and <a class="el" href="shfs__inode__remote__link_8c-example.html#a2">shfs_inode_remote_link.c</a>.</dd>
</dl>
</div>
</div>
<a class="anchor" id="gae2058cc7d0edcdab4953e2a25eb0be4a"></a><!-- doxytag: member="shfs_inode.h::shfs_inode" ref="gae2058cc7d0edcdab4953e2a25eb0be4a" args="(shfs_ino_t *parent, char *name, int mode)" -->
<div class="memitem">
<div class="memproto">
      <table class="memname">
        <tr>
          <td class="memname"><a class="el" href="structshfs__ino__t.html">shfs_ino_t</a>* shfs_inode </td>
          <td>(</td>
          <td class="paramtype"><a class="el" href="structshfs__ino__t.html">shfs_ino_t</a> *&nbsp;</td>
          <td class="paramname"> <em>parent</em>, </td>
        </tr>
        <tr>
          <td class="paramkey"></td>
          <td></td>
          <td class="paramtype">char *&nbsp;</td>
          <td class="paramname"> <em>name</em>, </td>
        </tr>
        <tr>
          <td class="paramkey"></td>
          <td></td>
          <td class="paramtype">int&nbsp;</td>
          <td class="paramname"> <em>mode</em></td><td>&nbsp;</td>
        </tr>
        <tr>
          <td></td>
          <td>)</td>
          <td></td><td></td><td></td>
        </tr>
      </table>
</div>
<div class="memdoc">

<p>Retrieve a sharefs inode directory entry based on a given parent inode and path name. </p>
<dl class="note"><dt><b>Note:</b></dt><dd>Searches for a reference to a sharefs inode labelled "name" in the <em>parent</em> inode. </dd>
<dd>
A new inode is created if a pre-existing one is not found. </dd></dl>
<dl><dt><b>Parameters:</b></dt><dd>
  <table border="0" cellspacing="2" cellpadding="0">
    <tr><td valign="top"></td><td valign="top"><em>parent</em>&nbsp;</td><td>The parent inode such as a directory where the file presides. </td></tr>
    <tr><td valign="top"></td><td valign="top"><em>name</em>&nbsp;</td><td>The relational pathname of the file being referenced. </td></tr>
    <tr><td valign="top"></td><td valign="top"><em>mode</em>&nbsp;</td><td>The type of information that this inode is referencing (SHINODE_XX). </td></tr>
  </table>
  </dd>
</dl>
<dl class="return"><dt><b>Returns:</b></dt><dd>A <code>shfs_node</code> is returned based on the <code>parent</code>, <code>name</code>, <code>and</code> mode specified. If one already exists it will be returned, and otherwise a new entry will be created. </dd></dl>
<dl><dt><b>Examples: </b></dt><dd><a class="el" href="shfs__inode__mkdir_8c-example.html#a3">shfs_inode_mkdir.c</a>, <a class="el" href="shfs__inode__remote__copy_8c-example.html#a5">shfs_inode_remote_copy.c</a>, and <a class="el" href="shfs__inode__remote__link_8c-example.html#a3">shfs_inode_remote_link.c</a>.</dd>
</dl>
</div>
</div>
<a class="anchor" id="ga87461a63830557b4cc3d409a2c6968a8"></a><!-- doxytag: member="shfs_inode.h::shfs_inode_read" ref="ga87461a63830557b4cc3d409a2c6968a8" args="(shfs_t *tree, shfs_ino_t *inode, shbuf_t *ret_buff, size_t data_of, size_t data_len)" -->
<div class="memitem">
<div class="memproto">
      <table class="memname">
        <tr>
          <td class="memname">ssize_t shfs_inode_read </td>
          <td>(</td>
          <td class="paramtype"><a class="el" href="structshfs__t.html">shfs_t</a> *&nbsp;</td>
          <td class="paramname"> <em>tree</em>, </td>
        </tr>
        <tr>
          <td class="paramkey"></td>
          <td></td>
          <td class="paramtype"><a class="el" href="structshfs__ino__t.html">shfs_ino_t</a> *&nbsp;</td>
          <td class="paramname"> <em>inode</em>, </td>
        </tr>
        <tr>
          <td class="paramkey"></td>
          <td></td>
          <td class="paramtype"><a class="el" href="structshbuf__t.html">shbuf_t</a> *&nbsp;</td>
          <td class="paramname"> <em>ret_buff</em>, </td>
        </tr>
        <tr>
          <td class="paramkey"></td>
          <td></td>
          <td class="paramtype">size_t&nbsp;</td>
          <td class="paramname"> <em>data_of</em>, </td>
        </tr>
        <tr>
          <td class="paramkey"></td>
          <td></td>
          <td class="paramtype">size_t&nbsp;</td>
          <td class="paramname"> <em>data_len</em></td><td>&nbsp;</td>
        </tr>
        <tr>
          <td></td>
          <td>)</td>
          <td></td><td></td><td></td>
        </tr>
      </table>
</div>
<div class="memdoc">

<p>Retrieve a data segment of a sharefs filesystem inode. </p>
<dl><dt><b>Parameters:</b></dt><dd>
  <table border="0" cellspacing="2" cellpadding="0">
    <tr><td valign="top"></td><td valign="top"><em>tree</em>&nbsp;</td><td>The sharefs partition allocated by <code><a class="el" href="group__libshare__fs.html#ga7f8fbad7732af7b76a3e056172365680" title="Creates a reference to a sharefs filesystem.">shfs_init()</a></code>. </td></tr>
    <tr><td valign="top"></td><td valign="top"><em>inode</em>&nbsp;</td><td>The inode whose data is being retrieved. </td></tr>
    <tr><td valign="top"></td><td valign="top"><em>ret_buff</em>&nbsp;</td><td>The <code><a class="el" href="structshbuf__t.html" title="A memory buffer.">shbuf_t</a></code> return buffer. </td></tr>
    <tr><td valign="top"></td><td valign="top"><em>data_of</em>&nbsp;</td><td>The offset to begin reading data from the inode. </td></tr>
    <tr><td valign="top"></td><td valign="top"><em>data_len</em>&nbsp;</td><td>The length of data to be read. </td></tr>
  </table>
  </dd>
</dl>
<dl class="return"><dt><b>Returns:</b></dt><dd>The number of bytes read on success, and a (-1) if the file does not exist. </dd></dl>

</div>
</div>
<a class="anchor" id="ga2e3935bfd16c44b1342e4453a4a2a77d"></a><!-- doxytag: member="shfs_inode.h::shfs_inode_read_block" ref="ga2e3935bfd16c44b1342e4453a4a2a77d" args="(shfs_t *tree, shfs_hdr_t *hdr, shfs_ino_t *inode)" -->
<div class="memitem">
<div class="memproto">
      <table class="memname">
        <tr>
          <td class="memname">int shfs_inode_read_block </td>
          <td>(</td>
          <td class="paramtype"><a class="el" href="structshfs__t.html">shfs_t</a> *&nbsp;</td>
          <td class="paramname"> <em>tree</em>, </td>
        </tr>
        <tr>
          <td class="paramkey"></td>
          <td></td>
          <td class="paramtype"><a class="el" href="structshfs__hdr__t.html">shfs_hdr_t</a> *&nbsp;</td>
          <td class="paramname"> <em>hdr</em>, </td>
        </tr>
        <tr>
          <td class="paramkey"></td>
          <td></td>
          <td class="paramtype"><a class="el" href="structshfs__ino__t.html">shfs_ino_t</a> *&nbsp;</td>
          <td class="paramname"> <em>inode</em></td><td>&nbsp;</td>
        </tr>
        <tr>
          <td></td>
          <td>)</td>
          <td></td><td></td><td></td>
        </tr>
      </table>
</div>
<div class="memdoc">

<p>Retrieve a single data block from a sharefs filesystem inode. </p>
<dl><dt><b>Parameters:</b></dt><dd>
  <table border="0" cellspacing="2" cellpadding="0">
    <tr><td valign="top"></td><td valign="top"><em>tree</em>&nbsp;</td><td>The sharefs partition allocated by <code><a class="el" href="group__libshare__fs.html#ga7f8fbad7732af7b76a3e056172365680" title="Creates a reference to a sharefs filesystem.">shfs_init()</a></code>. </td></tr>
    <tr><td valign="top"></td><td valign="top"><em>inode</em>&nbsp;</td><td>The inode whose data is being retrieved. </td></tr>
    <tr><td valign="top"></td><td valign="top"><em>hdr</em>&nbsp;</td><td>A specification of where the block is location in the sharefs filesystem partition. </td></tr>
    <tr><td valign="top"></td><td valign="top"><em>inode</em>&nbsp;</td><td>The inode block data to be filled in. </td></tr>
  </table>
  </dd>
</dl>
<dl class="return"><dt><b>Returns:</b></dt><dd>Returns 0 on success and -1 on failure. Check the errno for additional information. </dd></dl>

</div>
</div>
<a class="anchor" id="gae7450b1c214a9b72f3287c6dc75a8f80"></a><!-- doxytag: member="shfs_inode.h::shfs_inode_write" ref="gae7450b1c214a9b72f3287c6dc75a8f80" args="(shfs_t *tree, shfs_ino_t *inode, char *data, shfs_size_t data_of, shfs_size_t data_len)" -->
<div class="memitem">
<div class="memproto">
      <table class="memname">
        <tr>
          <td class="memname">ssize_t shfs_inode_write </td>
          <td>(</td>
          <td class="paramtype"><a class="el" href="structshfs__t.html">shfs_t</a> *&nbsp;</td>
          <td class="paramname"> <em>tree</em>, </td>
        </tr>
        <tr>
          <td class="paramkey"></td>
          <td></td>
          <td class="paramtype"><a class="el" href="structshfs__ino__t.html">shfs_ino_t</a> *&nbsp;</td>
          <td class="paramname"> <em>inode</em>, </td>
        </tr>
        <tr>
          <td class="paramkey"></td>
          <td></td>
          <td class="paramtype">char *&nbsp;</td>
          <td class="paramname"> <em>data</em>, </td>
        </tr>
        <tr>
          <td class="paramkey"></td>
          <td></td>
          <td class="paramtype"><a class="el" href="group__libshare__fs.html#gad222e037ed9c1075ba83239f5926d4e0">shfs_size_t</a>&nbsp;</td>
          <td class="paramname"> <em>data_of</em>, </td>
        </tr>
        <tr>
          <td class="paramkey"></td>
          <td></td>
          <td class="paramtype"><a class="el" href="group__libshare__fs.html#gad222e037ed9c1075ba83239f5926d4e0">shfs_size_t</a>&nbsp;</td>
          <td class="paramname"> <em>data_len</em></td><td>&nbsp;</td>
        </tr>
        <tr>
          <td></td>
          <td>)</td>
          <td></td><td></td><td></td>
        </tr>
      </table>
</div>
<div class="memdoc">

<p>Stores a data segment to a sharefs filesystem inode. </p>
<dl><dt><b>Parameters:</b></dt><dd>
  <table border="0" cellspacing="2" cellpadding="0">
    <tr><td valign="top"></td><td valign="top"><em>tree</em>&nbsp;</td><td>The sharefs partition allocated by <code><a class="el" href="group__libshare__fs.html#ga7f8fbad7732af7b76a3e056172365680" title="Creates a reference to a sharefs filesystem.">shfs_init()</a></code>. </td></tr>
    <tr><td valign="top"></td><td valign="top"><em>inode</em>&nbsp;</td><td>The inode whose data is being retrieved. </td></tr>
    <tr><td valign="top"></td><td valign="top"><em>data</em>&nbsp;</td><td>The data segment to write to the inode. </td></tr>
    <tr><td valign="top"></td><td valign="top"><em>data_of</em>&nbsp;</td><td>The offset to begin reading data from the inode. </td></tr>
    <tr><td valign="top"></td><td valign="top"><em>data_len</em>&nbsp;</td><td>The length of data to be read. </td></tr>
  </table>
  </dd>
</dl>
<dl class="return"><dt><b>Returns:</b></dt><dd>The number of bytes written on success, and a (-1) if the file cannot be written to. </dd></dl>

</div>
</div>
<a class="anchor" id="ga2f67b30a6c6913333b0b6a0a194132db"></a><!-- doxytag: member="shfs_journal.h::shfs_journal_free" ref="ga2f67b30a6c6913333b0b6a0a194132db" args="(shfs_t *tree, shfs_journal_t **jrnl_p)" -->
<div class="memitem">
<div class="memproto">
      <table class="memname">
        <tr>
          <td class="memname">void shfs_journal_free </td>
          <td>(</td>
          <td class="paramtype"><a class="el" href="structshfs__t.html">shfs_t</a> *&nbsp;</td>
          <td class="paramname"> <em>tree</em>, </td>
        </tr>
        <tr>
          <td class="paramkey"></td>
          <td></td>
          <td class="paramtype"><a class="el" href="structshfs__journal__t.html">shfs_journal_t</a> **&nbsp;</td>
          <td class="paramname"> <em>jrnl_p</em></td><td>&nbsp;</td>
        </tr>
        <tr>
          <td></td>
          <td>)</td>
          <td></td><td></td><td></td>
        </tr>
      </table>
</div>
<div class="memdoc">

<p>Initializes a sharefs filesystem journal for use. </p>
<dl class="note"><dt><b>Note:</b></dt><dd>This may not free resources if cached in a <code><a class="el" href="structshfs__t.html" title="The sharefs filesystem structure.">shfs_t</a></code> partition. </dd></dl>
<dl><dt><b>Parameters:</b></dt><dd>
  <table border="0" cellspacing="2" cellpadding="0">
    <tr><td valign="top"></td><td valign="top"><em>tree</em>&nbsp;</td><td>The sharefs partition. </td></tr>
    <tr><td valign="top"></td><td valign="top"><em>jrnl_p</em>&nbsp;</td><td>A reference to the journal instance to be free'd. </td></tr>
  </table>
  </dd>
</dl>

</div>
</div>
<a class="anchor" id="ga587c3da4d703e7d3652d5279bc5c59f5"></a><!-- doxytag: member="shfs_journal.h::shfs_journal_scan" ref="ga587c3da4d703e7d3652d5279bc5c59f5" args="(shfs_t *tree, int jno)" -->
<div class="memitem">
<div class="memproto">
      <table class="memname">
        <tr>
          <td class="memname">int shfs_journal_scan </td>
          <td>(</td>
          <td class="paramtype"><a class="el" href="structshfs__t.html">shfs_t</a> *&nbsp;</td>
          <td class="paramname"> <em>tree</em>, </td>
        </tr>
        <tr>
          <td class="paramkey"></td>
          <td></td>
          <td class="paramtype">int&nbsp;</td>
          <td class="paramname"> <em>jno</em></td><td>&nbsp;</td>
        </tr>
        <tr>
          <td></td>
          <td>)</td>
          <td></td><td></td><td></td>
        </tr>
      </table>
</div>
<div class="memdoc">

<p>Search for the first empty inode entry in a journal. </p>
<dl><dt><b>Parameters:</b></dt><dd>
  <table border="0" cellspacing="2" cellpadding="0">
    <tr><td valign="top"></td><td valign="top"><em>tree</em>&nbsp;</td><td>The sharefs filesystem partition. </td></tr>
    <tr><td valign="top"></td><td valign="top"><em>jno</em>&nbsp;</td><td>The index number of the journal. </td></tr>
  </table>
  </dd>
</dl>
<dl class="return"><dt><b>Returns:</b></dt><dd>A inode index number or (-1) on failure. </dd></dl>

</div>
</div>
<a class="anchor" id="gad3d3a478684d1f33e3813f8b13d90b61"></a><!-- doxytag: member="shfs_journal.h::shfs_journal_write" ref="gad3d3a478684d1f33e3813f8b13d90b61" args="(shfs_t *tree, shfs_journal_t *jrnl)" -->
<div class="memitem">
<div class="memproto">
      <table class="memname">
        <tr>
          <td class="memname">int shfs_journal_write </td>
          <td>(</td>
          <td class="paramtype"><a class="el" href="structshfs__t.html">shfs_t</a> *&nbsp;</td>
          <td class="paramname"> <em>tree</em>, </td>
        </tr>
        <tr>
          <td class="paramkey"></td>
          <td></td>
          <td class="paramtype"><a class="el" href="structshfs__journal__t.html">shfs_journal_t</a> *&nbsp;</td>
          <td class="paramname"> <em>jrnl</em></td><td>&nbsp;</td>
        </tr>
        <tr>
          <td></td>
          <td>)</td>
          <td></td><td></td><td></td>
        </tr>
      </table>
</div>
<div class="memdoc">

<p>Sync a sharefs journal to the local file-system. </p>
<dl class="note"><dt><b>Note:</b></dt><dd>Check errno for additional error-state information on failure. </dd></dl>
<dl><dt><b>Parameters:</b></dt><dd>
  <table border="0" cellspacing="2" cellpadding="0">
    <tr><td valign="top"></td><td valign="top"><em>tree</em>&nbsp;</td><td>The sharefs partition. </td></tr>
    <tr><td valign="top"></td><td valign="top"><em>jrnl</em>&nbsp;</td><td>The sharefs journal. </td></tr>
  </table>
  </dd>
</dl>
<dl class="return"><dt><b>Returns:</b></dt><dd>A zero (0) on success and a negative one (-1) on failure. </dd></dl>

</div>
</div>
<a class="anchor" id="gac54920a0bddc3e8c589910f8c83ce230"></a><!-- doxytag: member="shfs_meta.h::shfs_meta" ref="gac54920a0bddc3e8c589910f8c83ce230" args="(shfs_t *tree, shfs_ino_t *ent, shmeta_t **val_p)" -->
<div class="memitem">
<div class="memproto">
      <table class="memname">
        <tr>
          <td class="memname">int shfs_meta </td>
          <td>(</td>
          <td class="paramtype"><a class="el" href="structshfs__t.html">shfs_t</a> *&nbsp;</td>
          <td class="paramname"> <em>tree</em>, </td>
        </tr>
        <tr>
          <td class="paramkey"></td>
          <td></td>
          <td class="paramtype"><a class="el" href="structshfs__ino__t.html">shfs_ino_t</a> *&nbsp;</td>
          <td class="paramname"> <em>ent</em>, </td>
        </tr>
        <tr>
          <td class="paramkey"></td>
          <td></td>
          <td class="paramtype"><a class="el" href="structshmeta__t.html">shmeta_t</a> **&nbsp;</td>
          <td class="paramname"> <em>val_p</em></td><td>&nbsp;</td>
        </tr>
        <tr>
          <td></td>
          <td>)</td>
          <td></td><td></td><td></td>
        </tr>
      </table>
</div>
<div class="memdoc">

<p>Obtain a reference to the meta definition hashmap associated with the inode entry. </p>
<dl class="note"><dt><b>Note:</b></dt><dd>The <code><a class="el" href="structshfs__ino__t.html" title="A sharefs filesystem inode.">shfs_ino_t</a></code> inode will cache the hashmap reference. </dd></dl>
<dl><dt><b>Parameters:</b></dt><dd>
  <table border="0" cellspacing="2" cellpadding="0">
    <tr><td valign="top"></td><td valign="top"><em>ent</em>&nbsp;</td><td>The inode entry. </td></tr>
    <tr><td valign="top"></td><td valign="top"><em>val_p</em>&nbsp;</td><td>A memory reference to the meta definition hashmap being filled in. </td></tr>
  </table>
  </dd>
</dl>

</div>
</div>
<a class="anchor" id="ga51eeed961236c0f20c8dc8e8c3ae183e"></a><!-- doxytag: member="shfs_meta.h::shfs_meta_save" ref="ga51eeed961236c0f20c8dc8e8c3ae183e" args="(shfs_t *tree, shfs_ino_t *ent, shmeta_t *h)" -->
<div class="memitem">
<div class="memproto">
      <table class="memname">
        <tr>
          <td class="memname">int shfs_meta_save </td>
          <td>(</td>
          <td class="paramtype"><a class="el" href="structshfs__t.html">shfs_t</a> *&nbsp;</td>
          <td class="paramname"> <em>tree</em>, </td>
        </tr>
        <tr>
          <td class="paramkey"></td>
          <td></td>
          <td class="paramtype"><a class="el" href="structshfs__ino__t.html">shfs_ino_t</a> *&nbsp;</td>
          <td class="paramname"> <em>ent</em>, </td>
        </tr>
        <tr>
          <td class="paramkey"></td>
          <td></td>
          <td class="paramtype"><a class="el" href="structshmeta__t.html">shmeta_t</a> *&nbsp;</td>
          <td class="paramname"> <em>h</em></td><td>&nbsp;</td>
        </tr>
        <tr>
          <td></td>
          <td>)</td>
          <td></td><td></td><td></td>
        </tr>
      </table>
</div>
<div class="memdoc">

<p>Flush the inode's meta map to disk. </p>
<dl><dt><b>Parameters:</b></dt><dd>
  <table border="0" cellspacing="2" cellpadding="0">
    <tr><td valign="top"></td><td valign="top"><em>The</em>&nbsp;</td><td>inode associated with the meta map. </td></tr>
    <tr><td valign="top"></td><td valign="top"><em>val</em>&nbsp;</td><td>The meta map to store to disk. </td></tr>
  </table>
  </dd>
</dl>
<dl class="return"><dt><b>Returns:</b></dt><dd>A zero (0) on success and a negative one (-1) on failure. </dd></dl>

</div>
</div>
<a class="anchor" id="ga5b810eff4fc98882fc93f4f2b6fb7674"></a><!-- doxytag: member="shfs_proc.h::shfs_proc_lock" ref="ga5b810eff4fc98882fc93f4f2b6fb7674" args="(char *process_path, char *runtime_mode)" -->
<div class="memitem">
<div class="memproto">
      <table class="memname">
        <tr>
          <td class="memname">int shfs_proc_lock </td>
          <td>(</td>
          <td class="paramtype">char *&nbsp;</td>
          <td class="paramname"> <em>process_path</em>, </td>
        </tr>
        <tr>
          <td class="paramkey"></td>
          <td></td>
          <td class="paramtype">char *&nbsp;</td>
          <td class="paramname"> <em>runtime_mode</em></td><td>&nbsp;</td>
        </tr>
        <tr>
          <td></td>
          <td>)</td>
          <td></td><td></td><td></td>
        </tr>
      </table>
</div>
<div class="memdoc">

<p>Obtain an exclusive lock to a process with the same <code>process_path</code> and <code>runtime_mode</code>. </p>
<dl><dt><b>Parameters:</b></dt><dd>
  <table border="0" cellspacing="2" cellpadding="0">
    <tr><td valign="top"></td><td valign="top"><em>process_path</em>&nbsp;</td><td>The path to the process's executable file. (i.e. argv[0] in main() or static string) </td></tr>
    <tr><td valign="top"></td><td valign="top"><em>runtime_mode</em>&nbsp;</td><td>An optional method to clarify between multiple process locks. </td></tr>
  </table>
  </dd>
</dl>
<dl class="return"><dt><b>Returns:</b></dt><dd>A zero (0) on success and a negative one (-1) on failure. </dd></dl>

</div>
</div>
<a class="anchor" id="ga2ab916536afac9c7029cdacf921b764b"></a><!-- doxytag: member="shfs_write.h::shfs_write_print" ref="ga2ab916536afac9c7029cdacf921b764b" args="(shfs_t *tree, shfs_ino_t *inode, int fd)" -->
<div class="memitem">
<div class="memproto">
      <table class="memname">
        <tr>
          <td class="memname">int shfs_write_print </td>
          <td>(</td>
          <td class="paramtype"><a class="el" href="structshfs__t.html">shfs_t</a> *&nbsp;</td>
          <td class="paramname"> <em>tree</em>, </td>
        </tr>
        <tr>
          <td class="paramkey"></td>
          <td></td>
          <td class="paramtype"><a class="el" href="structshfs__ino__t.html">shfs_ino_t</a> *&nbsp;</td>
          <td class="paramname"> <em>inode</em>, </td>
        </tr>
        <tr>
          <td class="paramkey"></td>
          <td></td>
          <td class="paramtype">int&nbsp;</td>
          <td class="paramname"> <em>fd</em></td><td>&nbsp;</td>
        </tr>
        <tr>
          <td></td>
          <td>)</td>
          <td></td><td></td><td></td>
        </tr>
      </table>
</div>
<div class="memdoc">

<p>Writes the file contents of the inode to the file stream. </p>
<dl><dt><b>Parameters:</b></dt><dd>
  <table border="0" cellspacing="2" cellpadding="0">
    <tr><td valign="top"></td><td valign="top"><em>tree</em>&nbsp;</td><td>The sharefs filesystem partition allocation by <code><a class="el" href="group__libshare__fs.html#ga7f8fbad7732af7b76a3e056172365680" title="Creates a reference to a sharefs filesystem.">shfs_init()</a></code>. </td></tr>
    <tr><td valign="top"></td><td valign="top"><em>inode</em>&nbsp;</td><td>The sharefs filesystem inode to print from. </td></tr>
    <tr><td valign="top"></td><td valign="top"><em>fd</em>&nbsp;</td><td>A posix file descriptor number representing a socket or local filesystem file reference. </td></tr>
  </table>
  </dd>
</dl>
<dl class="return"><dt><b>Returns:</b></dt><dd>A zero (0) on success and a negative one (-1) on error. On error one of the following error codes will be set: EBADF fd is not a valid file descriptor or is not open for writing. </dd></dl>
<dl><dt><b>Examples: </b></dt><dd><a class="el" href="shfs__inode__remote__copy_8c-example.html#a7">shfs_inode_remote_copy.c</a>, and <a class="el" href="shfs__inode__remote__link_8c-example.html#a6">shfs_inode_remote_link.c</a>.</dd>
</dl>
</div>
</div>
</div>

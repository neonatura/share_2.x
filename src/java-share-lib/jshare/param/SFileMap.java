
/*
 * @copyright
 *
 *  Copyright 2015 Neo Natura 
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
package jshare.param;

import java.io.*;
import java.util.*;
import java.nio.*;
import java.nio.channels.*;

public class SFileMap extends HashMap<String,FileChannel> 
{

  public SFileMap()
  {
  }

  public FileChannel createChannel(String path)
  {
    FileChannel chan = null;

    try {
      RandomAccessFile f = new RandomAccessFile(path, "rw");
      chan = f.getChannel();
    } catch (FileNotFoundException file_err) {
      file_err.printStackTrace();
    }

    return (chan);
  }
  
  public FileChannel getChannel(String name)
  {

    FileChannel chan = get(name);
    if (chan == null) {
      String path = name;
      chan = createChannel(path);
      put(name, chan);
    }

    try {
      if (chan.size() != 0) {
        /* rewind */
        chan.position(0);
      }
    } catch (IOException io_err) {
    }

    return (chan);
  }

  static public SFileMap getInstance()
  {
    if (_instance == null)
      _instance = new SFileMap();
    return (_instance);
  }

  static public InputStream getInputStream(String name)
  {
    return (Channels.newInputStream(getInstance().getChannel(name)));    
  }

  static public OutputStream getOutputStream(String name)
  {
    return (Channels.newOutputStream(getInstance().getChannel(name)));
  }

  static public SFileMap _instance = null;

}



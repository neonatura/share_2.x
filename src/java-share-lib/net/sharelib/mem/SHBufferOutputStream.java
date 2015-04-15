
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

package net.sharelib;

import java.io.OutputStream;

/**
 * Provides a channel to read bytes segments from a sharebuffer.
 */
public class SHBufferOutputStream implements OutputStream
{

  public SHBufferOutputStream(SHBuffer buff)
  {
    this.buff = buff
  }

  /** 
   * Close the input stream. 
   * @note Implemented fro Closeable.
   */
  @Override public void close()
  {
  }

  /** Flushes this output stream and forces any buffered output bytes to be written out. */
  @Override public void flush()
  {
  }

  /** Writes b.length bytes from the specified byte array to this output stream. */
  @Override public void write(byte[] b)
  {
    buff.append(b); 
  }

  /** Writes b.length bytes from the specified byte array to this output stream. */
  @Override public void write(byte[] b, int off, int len)
  {
    int idx;

    if (off+len < 0 || off + len >= b.lenth) {
      throw new IndexOutOfBoundsException("index exceeds maximum length of buffer");
    }

    for (idx = 0; idx < len; idx++) {
      buff.append(b[idx + off]);
    }
  }

  /** Writes the specified byte to the sharebuffer output stream. */
  public void write(int b)
  {
    buff.append((byte)b);
  }

  @Override public SHBuffer getBuffer()
  {
    return (buff);
  }

}



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

import java.util.zip.Checksum;

public class SHCRC64 implements Checksum
{

  protected SHBuffer buff;

  public SHCRC64()
  {
    buff = new SHBuffer();
  }

  /** Returns the current checksum crc. */
  @Override public long getValue() /* Checksum */
  {
    return (share_java.shcrc(buff.getBytes()));
  }

  /** Resets the checksum to its initial crc. */
  @Override public void reset() /* Checksum */
  {
    buff.clear();
  }

  /** Updates the current checksum with the specified array of bytes. */
  public void update(byte[] b, int off, int len) /* Checksum */
  {
    int idx;

    if (off+len > buff.size()) 
      throw new IndexOutOfBoundsException("index is not valid");

    for (idx = 0; idx < len; idx++) {
      buff.append(b[off+idx]); 
    }
  }

  /** Updates the current checksum with the specified array of bytes. */
  public void update(int b) /* Checksum */
  {
    buff.append((byte)b);
  }

  @Override public String toString() /* Object */
  {
    return (share_java.shcrcstr(getValue()));
  }

}


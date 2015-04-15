
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

public class SHTime
{

  protected SWIGTYPE_p_shtime_t time;

  public SHTime()
  {
    time = shtime64();
  }

/*
  public SHTime(long stamp)
  {
    
  }
*/

  /**
   * Convert a SHTime into an epoch unix-style timestamp.
   */
  public long utime()
  {
    return (share_java.shutime64(this.time));
  } 

  /**
   * Print the time being represented in a standard format.
   */
  public String toString()
  {
    return (share_java.shctime64(this.time));
  }

}

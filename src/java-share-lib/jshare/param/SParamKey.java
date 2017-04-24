
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
import java.security.*;

/**
 * A "primary key value" for a table record.
 */
public class SParamKey implements Serializable, Guard
{

  static final String NONE = "";
  static final String TABLE = "table:";

  public final static long serialVersionUID = 46L;

  public SParamKey()
  {
  }

  public SParamKey(String label)
  {
    this.label = label;
  }

  public SParamKey(String label, String type)
  {
    this.label = type+label;
  }

  public void setLabel(String label)
  {
    this.label = label;
  }

  public String getLabel()
  {
    return (label);
  }

  public int getCrc()
  {
    return (hashCode());
  }

  public void setTable(int index)
  {
    this.index = index;
  }

  public int getTable()
  {
    return (index);
  }

  public byte[] toBytes()
  {
    return (toString().getBytes());
  }

  @Override public void checkGuard(Object obj) throws SecurityException
  {
System.out.println("DEBUG: SParamKey: checkGuard(): " + obj.getClass().getName()); 
/*
    SPermTable perm = SPermTable.getContext(obj);
    if (perm != null)
      perm.checkGuard(obj);
*/
  }

  @Override public String toString()
  {
    return (label);
  }

  protected String label = "";
  protected int index;
}



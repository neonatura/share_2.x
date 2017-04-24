
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
import javax.crypto.*;


public class SGeoTable extends SParamTable
{

  public final static long serialVersionUID = 52L;

  static final String GEO_TABLE = "geo";


  public SGeoTable(Key key)
  {
    super(GEO_TABLE, key);
  }

  public void setGeo(SGeo geo)
  {
    add(geo);
  }

  public SGeo getGeo(SLocation loc)
  {
    return ((SGeo)find(loc));
  }

  public SGeo[] getRecent()
  {
    return ((SGeo[])cache.toArray(new SGeo[cache.size()]));
  }

  static public SGeoTable restore(Key key)
  {
    SGeoTable ret = null;

    try {
      ret = (SGeoTable)restore(GEO_TABLE, key);
    } catch(InvalidKeyException key_err) {
      ret = null;
    }

    return (ret);
  }

  static public SGeoTable getContext(Object obj)
  {

    if (_instance == null) {
      Key key = SKeyTable.getContext(obj).getTableKey(GEO_TABLE);
      _instance = restore(key);
      if (_instance == null) {
        /* initial creation. */
        _instance = new SGeoTable(key);
      }
    }

    return (_instance);
  }

  static private SGeoTable _instance;

}



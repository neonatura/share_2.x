
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


public class SPermTable extends SParamTable implements Guard
{

  public final static long serialVersionUID = 52L;

  static final String PERM_TABLE = "perm";


  public SPermTable(Key key)
  {
    super(PERM_TABLE, key);
  }

  public void setPerm(SPerm perm)
  {
    add(perm);
  }

  public SPerm getPerm(SParamKey name)
  {
    return ((SPerm)find(name));
  }

  public SPerm getPerm(String label)
  {
    SParamKey name = new SParamKey(label);
    return (getPerm(name));
  }

  @Override public void checkGuard(Object obj)
  {
    SPerm perm = null;

    if (obj instanceof SPerm) {
      /* a literal permission was passed in to verify itself. */
      //SPerm perm = (SPerm)obj;
    } else if (obj instanceof SParam) {
      /* check for a permission referencing this aux table record */
      SParam param = (SParam)obj;
      perm = getPerm(param.getName());
    }

    if (perm != null)
      perm.checkGuard(obj);

  }

  static public SPermTable restore(Key key)
  {
    SPermTable ret = null;

System.out.println("DEBUG: restoring perm table..");
    try {
      ret = (SPermTable)restore(PERM_TABLE, key);
    } catch(InvalidKeyException key_err) {
      ret = null;
    }
System.out.println("DEBUG: restored perm table");

    return (ret);
  }

  static public SPermTable getContext(Object obj)
  {

    if (_instance == null) {
      Key key = SKeyTable.getContext(obj).getTableKey(PERM_TABLE);
      _instance = restore(key);
      if (_instance == null) {
        /* initial creation. */
        _instance = new SPermTable(key);
      }
    }

    return (_instance);
  }

  static private SPermTable _instance;

}



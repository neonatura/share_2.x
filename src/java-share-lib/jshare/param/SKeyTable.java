
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

public class SKeyTable extends SParamTable
{

  public final static long serialVersionUID = 50L;

  static final String KEY_TABLE = "key";


  public SKeyTable(Key key)
  {
    super(KEY_TABLE, key);
  }

  public void setKey(SParamKey name, Key key)
  {
    SParam param;

    param = find(name);
    if (param == null) {
      param = new SParam(name);
      add(param);
    }

    param.setValue("java.security.Key", key);
  }

  public Key getKey(SParamKey name)
  {
    SParam param;
    Key key;

    param = find(name);
    if (param == null) {
System.out.println("DEBUG: SKeyTable: getKey: unknown record: " + name.toString());
      return (null);
}

    key = (Key)param.getValue("java.security.Key");
    if (key == null) {
System.out.println("DEBUG: SKeyTable: getKey: found record but no java key inside");
      return (null);
}

    return (key);
  }

  public void setKey(String label, String type, Key key)
  {
    setKey(new SParamKey(label, type), key);
  }

  public void setKey(String label, Key key)
  {
    setKey(label, "", key);
  }

  public Key getKey(String label, String type)
  {
    return (getKey(new SParamKey(label, type)));
  }

  public Key getKey(String label)
  {
    return (getKey(label, ""));
  }

  public Key getTableKey(String label)
  {
    SParamKey name = new SParamKey(label, SParamKey.TABLE);
    Key key;

    key = getKey(name);
    if (key == null) {
      /* generate new secret */
      key = createSecret(name); 
      setKey(name, key);
    }

    return (key);
  }

  static public SKeyTable getContext(Object obj)
  {

    if (_instance == null) {
      try {
        _instance = (SKeyTable)restore(KEY_TABLE, getMasterKey());
      } catch (InvalidKeyException key_err) {
        key_err.printStackTrace();
        return (null);
      }
      if (_instance == null) {
        /* initial creation. */
        _instance = new SKeyTable(getMasterKey());
System.out.println("DEBUG: SKeyTable.getContext(): fresh instance"); 
      } else {
System.out.println("DEBUG: SKeyTable.getContext(): restored instance");
      }
    }

    return (_instance);
  }


  static private SKeyTable _instance;

}



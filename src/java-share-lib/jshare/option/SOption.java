
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
package jshare.option;

import java.util.HashMap;

public class SOption extends HashMap<Object,String>
{

  public final static long serialVersionUID = 42L;

  public static final String OPT_MAIN_TITLE = "MAIN_TITLE";
  public static final String OPT_MAIN_DESC = "MAIN_DESC";
  public static final String OPT_MAIN_WIDTH = "MAIN_WIDTH";
  public static final String OPT_MAIN_HEIGHT = "MAIN_HEIGHT";

  static private SOption _instance;

  public SOption()
  {
    setDefaultValues();
  }

  public void setDefaultValues()
  {
    put(OPT_MAIN_TITLE, "Share Administration Utility");
    put(OPT_MAIN_DESC, "Java interface for the share library suite.");
    put(OPT_MAIN_WIDTH, "920");
    put(OPT_MAIN_HEIGHT, "740");
  }

  public int getInt(Object key, int def)
  {
    Object obj = get(key);
    if (obj == null)
      return (def);
    return (Integer.valueOf(obj.toString()));
  }

  public String get(Object key, String def)
  {
    Object obj = get(key);
    if (obj == null)
      return (def);
    return (obj.toString());
  }

  static public SOption initInstance()
  {
    if (_instance == null)
      _instance = new SOption();
    return (_instance);
  }

  static public String getValue(String name)
  {
    SOption opt = initInstance();
    return (opt.get(name, ""));
  }

  static public int getIntValue(String name)
  {
    SOption opt = initInstance();
    return (opt.getInt(name, 0));
  }


}

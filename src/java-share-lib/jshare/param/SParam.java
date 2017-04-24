
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

public class SParam extends HashMap implements Serializable, ObjectInputValidation
{

  public final static long serialVersionUID = 42L;

//  static private SParam _instance;

  public SParam()
  {
  }

  public SParam(SParamKey name)
  {
    this.name = name;
  }

  public SParam(String label)
  {
    name = new SParamKey(label);
  }

  public void setValue(Object key, Object value)
  {
    put(key, value);
System.out.println("DEBUG: SParam: setValue: '" + key.toString() + "' = '" + value.toString() + "'");
  }

  public Object getValue(Object key)
  {
    Object ret;

    ret = get(key);

    if (ret == null) {
System.out.println("DEBUG: SParam: getValue: '" + key.toString() + "<null>");
    } else {
System.out.println("DEBUG: SParam: getValue: '" + key.toString() + "' = '" + ret.toString() + "'");
    }

    return (ret);
  }

  public boolean hasValue(Object key)
  {
    return (containsKey(key));
  }

  public double num(Object key)
  {
    Object obj = get(key);
    if (obj == null)
      return (0.0);
    if (obj instanceof Double) {
      return ( ((Double)obj).doubleValue() );
    }
    return (Double.valueOf(obj.toString()));
  }

  public String str(Object key)
  {
    Object obj = get(key);
    if (obj == null)
      return ("");
    return (obj.toString());
  }

  public int getCrc()
  {
    return (hashCode());
  }

  public String getLabel()
  {
    return (name.getLabel());
  }

  public void setLabel(String label)
  {
    name.setLabel(label);
  }

  public SParamKey getName()
  {
    return (name);
  }

  @Override public void validateObject() throws InvalidObjectException
  {
System.out.println("DEBUG: validateObject()");
    if (getCrc() != crc)
      throw new InvalidObjectException("checksum integrity");
  }

  private void writeObject(ObjectOutputStream stream) throws IOException
  {
    crc = getCrc();
    stream.defaultWriteObject();
  }

  private void readObject(ObjectInputStream stream) throws IOException, ClassNotFoundException
  {
    stream.defaultReadObject();
    validateObject();
  }

  protected int crc;
  protected SParamKey name;

}



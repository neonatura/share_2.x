
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
import javax.swing.*;

import jshare.gui.*;

public class SGeo extends SParam implements Guard, SListItem 
//extends Geoission
{

  static final String TITLE = "name";
  static final String DESC = "desc";
  static final String ZONE = "zone";

  public SGeo()
  {
  }

  public SGeo(SLocation location)
  {
    this.name = location;
  }
  public SGeo(double latitude, double longitude)
  {
    this(new SLocation(latitude, longitude));
  }

  @Override public void checkGuard(Object obj)
  {
    //
  }

  public void setTitle(String str)
  {
    setValue(TITLE, str);
  }

  @Override public String getTitle() /* SListItem */
  {
    return (str(TITLE));
  }

  public void setDesc(String str)
  {
    setValue(DESC, str);
  }

  @Override public String getDesc() /* SListItem */
  {
    return (str(DESC));
  }

  public void setZone(String str)
  {
    setValue(ZONE, str);
  }

  public String getZone()
  {
    return (str(ZONE));
  }

  public SLocation getLocation()
  {
    return ((SLocation)name);
  }

  public double getLatitude()
  {
    return (getLocation().getLatitude());
  }

  public double getLongitude()
  {
    return (getLocation().getLongitude());
  }

  @Override public JComponent getComponent() /* SListItem */
  {
    return (null);
  }

  static public SGeo getContext(SParamKey name)
  {
    return (null);
  }

  @Override public String toString()
  {
    return (getTitle() + " (" + getLatitude() + ", " + getLongitude() + ")");
  }

}



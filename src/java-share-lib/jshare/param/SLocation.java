
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
public class SLocation extends SParamKey
{

  public final static long serialVersionUID = 62L;

  public SLocation(double latitude, double longitude)
  {
    this.latitude = latitude;
    this.longitude = longitude;
    this.label = "geo:" + latitude + "," + longitude;
  }

  public double getLatitude()
  {
    return (latitude);
  }

  public double getLongitude()
  {
    return (longitude);
  }

  @Override public String toString()
  {
    return (this.label);
  }

  protected double latitude;
  protected double longitude;
}



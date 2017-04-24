
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
package jshare.action;

import java.awt.*;
import java.awt.event.*;
import java.security.*;
import javax.swing.*;

import jshare.param.*;
import jshare.gui.*;
import jshare.gui.menu.*;
import jshare.gui.panel.*;

public class SGeoAction extends SAction
{


  public SGeoAction()
  {

    setName("Geo");
    setDesc("Manage geodetic locations.");

  }

/*
  public void runFgTask(String cmd, SPanel c)
  {
  }
*/

  public void runBgTask(String cmd, SPanel c)
  {
    System.out.println("DEBUG: System.runBgTask[" + cmd + "]:");
  }

  public void runFgTask(String cmd, SPanel c)
  {
/*
    SMenuPanel p = (SMenuPanel)c;
    super.runFgTask(cmd, c);
*/

    System.out.println("DEBUG: System.runFgTask[" + cmd + "]:");


/*
    SKeyTable keys = SKeyTable.getContext(this);
    Key key = keys.getKey("test key");
    if (key == null) {
 System.out.println("DEBUG: ACTION: no pre-existing key found"); 
    } else {
 System.out.println("DEBUG: ACTION: found pre-existing key");
    }
    key = SParamTable.createSecret();
    keys.setKey("test key", key);
System.out.println("DEBUG: ACTION: wrote new key");
    keys.flush();
System.out.println("DEBUG: ACTION: flushed key db");
    key = keys.getKey("test key");
    if (key == null ) {
System.out.println("DEBUG: ACTION: verify failure for new key");
    }
*/

    SGeoTable geo = SGeoTable.getContext(this);
    SGeo rec;
    int i;

    rec = new SGeo(1, 1);

    rec.setTitle("title");
    rec.setDesc("desc");
    rec.setDesc("zone");
    geo.setGeo(rec);


    if (c instanceof SMenu) {
      SMenu main = (SMenu)c;
      SList list = new SList(geo.getCache());
      main.setContent(list);
      main.expandContent();
    }

  }

}

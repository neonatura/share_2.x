
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

import java.awt.Window;
import java.awt.event.ActionEvent;
import javax.swing.AbstractAction;
import javax.swing.Action;
import jshare.gui.panel.*;

public class SExitAction extends SAction
{

  protected Window window;

  public SExitAction()
  {
    setName("Exit");
    setDesc("Close the application");
  }

  public SExitAction(Window window)
  {
    this.window = window;

    setName("Exit");
    setDesc("Close the application");
  }

  @Override public void actionPerformed(ActionEvent e)
  {

    // Get rid of main display.
    if (window != null) {
      window.setVisible(false);
    }

    if (window != null) {
      window.dispose();
    }

    System.exit(0);
 
  }
}

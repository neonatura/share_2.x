
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

import java.awt.Component;
import java.awt.event.ActionEvent;
import javax.swing.AbstractAction;
import javax.swing.JLayeredPane;
import jshare.gui.menu.SIndexMenu;
import jshare.gui.panel.SMenuPanel;

public class SPeerAction extends SViewLayerAction
{

protected JLayeredPane targetPane;

  public SPeerAction(JLayeredPane targetPane)
  {
    super(targetPane);

    this.targetPane = targetPane;
    setName("Peers");
    setDesc("Manage your associated peer identities.");

  }

/*
  public void runFgTask(String cmd, Component c)
  {
  }
  public void runBgTask(String cmd, Component c)
  {
  }
*/

  public void runFgTask(String cmd, Component c)
  {
    SMenuPanel p = (SMenuPanel)c;

    super.runFgTask(cmd, c);

    System.out.println("DEBUG: SYstem.runBgTask[" + cmd + "]: c = " + c.getClass().getName());

SAction peerIndexAction = new SAction();
p.addMenu(new SIndexMenu(peerIndexAction));


  }

}

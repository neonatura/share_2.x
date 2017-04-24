
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

import java.util.HashMap;
import java.awt.*;
import java.awt.event.*;
import javax.swing.AbstractAction;
import javax.swing.Action;
import jshare.gui.*;
import jshare.gui.panel.*;

public class SAction extends AbstractAction implements Runnable
{
  public static final long serialVersionUID = 42L;

  protected HashMap map = new HashMap();

  private volatile SPanel _task_component;
  private volatile String _task_command;

  public SAction()
  {
    setName("Defalt");
    setDesc("Defalt");
  }

  public void runFgTask(String cmd, SPanel c)
  {
  }

  public void runBgTask(String cmd, SPanel c)
  {
  }

  public void runTask(String cmd, SPanel c)
  {

    _task_command = cmd;
    _task_component = c;

    runFgTask(cmd, c);
    new Thread(this).start();
  }

  public void actionPerformed(ActionEvent e)
  {
    String cmd = e.getActionCommand();
    SPanel c = (SPanel)map.get(cmd);
    if (c == null)
      c = (SPanel)e.getSource();
    runTask(cmd, c); 
  }
  public void actionPerformed(MouseEvent e)
  {
    String cmd = e.getSource().getClass().getName();
    SPanel c = (SPanel)map.get(cmd);
    if (c == null)
      c = (SPanel)e.getSource();
    runTask(cmd, c);
  }

/*
  @Override public void actionPerformed(ActionEvent e)
  {
    SActionState state = getState();
    SwingUtilties.invokeLater(state.getTask(e));
  }

  abstract SActionState getState();
*/

  public String getName()
  {
    return (getValue(NAME).toString());
  }

  public String getDesc()
  {
    return (getValue(SHORT_DESCRIPTION).toString());
  }

  public String getTooltipText()
  {
    return (getDesc());
  }

  public String getTitle()
  {
    return (getName());
  }

  public void setName(String name)
  {
    putValue(NAME, name);
  }
  public void setDesc(String name)
  {
    putValue(SHORT_DESCRIPTION, name);
  }


  public void add(String name, SPanel c)
  {
    //pane.add(c, -1);
    map.put(name, c);
  }

  public String toString()
  {
    return (getName());
  }

  public void run()
  {
    runBgTask(_task_command, _task_component);
  }
}

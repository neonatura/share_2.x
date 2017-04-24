
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

import java.util.*;
import java.awt.*;
import java.awt.event.*;
import javax.swing.*;

import jshare.gui.*;
import jshare.gui.panel.*;

public class SViewLayerAction extends SAction
{

  protected SDesktopPane pane;

  public SViewLayerAction(SDesktopPane pane)
  {
    this.pane = pane;
  }

  public void runFgTask(String cmd, SPanel c)
  {

    if (c == null)
      return;

    pane.moveToFront(c);
System.out.println("DEBUG: SViewLayerAction: runFgTask: pane(" + pane.getClass().getName() + ").moveToFront(" + c.getClass().getName() + ")");
  }


}

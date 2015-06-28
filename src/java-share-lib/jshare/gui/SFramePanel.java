
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
package jshare.gui;

import java.awt.Color;
import java.awt.GridLayout;
import java.awt.event.ComponentListener;
import java.awt.event.ComponentEvent;
import javax.swing.JComponent;
import javax.swing.SwingConstants;

public class SFramePanel extends SPanel 
{

  static public final int NONE = 0;
  static public final int X_AXIS = 1;
  static public final int Y_AXIS = 2;
  static public final int BOTH = 3;

  protected int floating;
  protected int gravity;
  protected JComponent parent;

  public SFramePanel(JComponent parent)
  {
    super();

    this.floating = SwingConstants.LEFT;

    setLayout(new GridLayout(1, 1));

    this.parent = parent;
    parent.addComponentListener(new SFramePanelListener(parent, this));
  }

  public void setGravity(int gravity)
  {
    this.gravity = gravity;
  }

  public int getGravity()
  {
    return (gravity);
  }

  public void setFloating(int type)
  {
    this.floating = type;
  }

  public int getFloating()
  {
    return (floating);
  }

}

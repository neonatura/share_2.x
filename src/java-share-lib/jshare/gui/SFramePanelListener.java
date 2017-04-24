

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

import java.awt.Point;
import java.awt.Dimension;
import java.awt.event.ComponentListener;
import java.awt.event.ComponentEvent;
import javax.swing.JComponent;
import javax.swing.SwingUtilities;
import javax.swing.SwingConstants;

public class SFramePanelListener implements ComponentListener, Runnable
{

  protected JComponent parent;

  protected SFramePanel panel;

  public SFramePanelListener(JComponent parent, SFramePanel panel)
  {
    this.parent = parent;
    this.panel = panel;

    panel.setVisible(false);
    SwingUtilities.invokeLater(this);
  }
  public void run() {
    calculateSize();
    calculatePosition();
  }

  public void calculatePosition()
  {
    Point p = panel.getLocation(null);
    int tot_width = parent.getSize().width;
    int width = panel.getSize().width;
    
    if (panel.getFloating() == SwingConstants.CENTER) {
      p.x = Math.max((tot_width - width) / 2, p.x);
      panel.setLocation(p);
    } 

System.out.println("DEBUG: SFramePanelListener: position("+p+") panel("+panel+")");
  }

  public void calculateSize()
  {
    double height;

    height = parent.getSize().height;
if (height < 100)
return;
panel.setVisible(true);
    Dimension size = panel.getSize(); 
    size.height = (int)height;
    panel.setSize(size);
//    panel.setPreferredSize(size);
System.out.println("DEBUG: height: " + height + ", vis: " + panel.isVisible());

  }

  /** Invoked when the component has been made invisible. */ 
  public void componentHidden(ComponentEvent e)
  {
  }

  /** Invoked when the component's position changes. */
  public void componentMoved(ComponentEvent e)
  {
  }

  /** Invoked when the component's size changes. */
  public void componentResized(ComponentEvent e)
  {
    calculateSize();
    calculatePosition();
  }

  /** Invoked when the component has been made visible. */ 
  public void componentShown(ComponentEvent e)
  {
  }
}

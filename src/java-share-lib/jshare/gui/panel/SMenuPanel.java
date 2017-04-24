
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
package jshare.gui.panel;

import java.util.Vector;
import java.awt.Dimension;
import java.awt.GridBagConstraints;
import java.awt.Color;
import java.awt.ComponentOrientation;
import java.awt.event.MouseListener;
import java.awt.event.MouseEvent;
import javax.swing.JLayeredPane;
import javax.swing.JScrollPane;
import javax.swing.BoxLayout;
import jshare.action.SAction;
import jshare.style.SToolTip;
import jshare.style.SFont;
import jshare.gui.*;
import jshare.gui.menu.SMenu;
import jshare.option.SOption;

public class SMenuPanel extends SFramePanel implements MouseListener
{

  protected Vector list_data = new Vector();

  protected SPanel panel;

  protected SDesktopPane desktopPane;
 
  public SMenuPanel(SDesktopPane desktopPane)
  {
    super(desktopPane);

    this.desktopPane = desktopPane;
//    desktopPane.add(getClass().getName(), this);


    panel = new SPanel();
panel.addMouseListener(this);
/*
    BoxLayout l = new BoxLayout(panel, BoxLayout.Y_AXIS);
    panel.setLayout(l);
*/
    JScrollPane scrollPanel = new JScrollPane();
scrollPanel.setOpaque(false);
    scrollPanel.setViewportView(panel);
    add(scrollPanel);


    int width = SOption.getIntValue(SOption.OPT_MAIN_WIDTH) / 5;
    setSize(new Dimension(width, 100));
setLocation( (idx++ * width), 0 );

    setGravity(Y_AXIS);



    Color backgroundColor = Color.CYAN;//SOption.getColorValue(SOption.MAIN_COLOR_BACKGROUND); 
    Color lightBackgroundColor = Color.LIGHT_GRAY;//SOption.getColorValue(SOption.MAIN_COLOR_LIGHT_BACKGROUND); 
    setGradient(ComponentOrientation.LEFT_TO_RIGHT, backgroundColor, lightBackgroundColor); 



    desktopPane.add(this, JLayeredPane.DEFAULT_LAYER);
//    desktopPane.moveToFront(this);
  }
static int idx;
  public void addMenu(SMenu menu)
  {
    panel.add(menu);
System.out.println("DEBUG: addMenu: panel '" + panel.getClass().getName() + "', menu '" + menu.getClass().getName() + "'.");

  }


public void mousePressed(MouseEvent me) {
    requestFocus();
    System.out.println("Mouse Pressed in JPanel");
}
public void mouseReleased(MouseEvent me) {}
public void mouseClicked(MouseEvent me) {}
public void mouseEntered(MouseEvent me) {}
public void mouseExited(MouseEvent me) {}
}

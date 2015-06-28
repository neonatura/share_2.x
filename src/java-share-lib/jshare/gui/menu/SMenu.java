
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
package jshare.gui.menu;

import java.awt.Dimension;
import java.awt.Component;
import java.awt.BorderLayout;
import java.awt.Color;
import java.awt.ComponentOrientation;
import java.awt.event.MouseListener;
import java.awt.event.MouseEvent;
import javax.swing.BoxLayout;
import javax.swing.SwingConstants;
import javax.swing.JLabel;
import javax.swing.JComponent;
import javax.swing.border.EmptyBorder;
import javax.swing.border.EtchedBorder;
import javax.swing.border.CompoundBorder;
import jshare.gui.SPanel;
import jshare.action.SAction;
import jshare.gui.SListItem;
import jshare.gui.SLabel;
import jshare.gui.SButton;

public class SMenu extends SPanel implements MouseListener
{

  protected SAction action;

  protected JComponent target;

  public SMenu(SAction action)
  {
    this.action = action;

    setBorder(new EmptyBorder(8, 8, 8, 8));

    setAlignmentX(Component.CENTER_ALIGNMENT);

    BoxLayout layout = new BoxLayout(this, BoxLayout.Y_AXIS);
    setLayout(layout);

    addMouseListener(this);


    Color backgroundColor = new Color(5, 30, 70);
    Color lightBackgroundColor = new Color(10, 60, 140);
    setGradient(ComponentOrientation.LEFT_TO_RIGHT, backgroundColor, lightBackgroundColor); 
    //    name.setAlignmentX(Component.CENTER_ALIGNMENT);

    backgroundColor = new Color(30, 60, 70);
    lightBackgroundColor = Color.GRAY;
    SLabel name = new SLabel(action.getName());
    name.setForeground(Color.LIGHT_GRAY);
    //name.setBackground(backgroundColor);
    name.setBorder(
        new CompoundBorder(
          new EtchedBorder(EtchedBorder.RAISED,
            lightBackgroundColor, backgroundColor),
          new EmptyBorder(2, 2, 2, 2)
          ));
    add(name);

    SLabel desc = new SLabel(action.getDesc());
    desc.setForeground(Color.LIGHT_GRAY);
    add(desc);
    //    desc.setAlignmentX(Component.CENTER_ALIGNMENT);

  }

  public Component getComponent()
  {
    return (this);
  }

  public void mouseClicked(MouseEvent event) 
  {
    System.out.println("entered");
  }
  public void mouseEntered(MouseEvent event) 
  {
    System.out.println("entered");
  }
  public void mouseExited(MouseEvent event)
  {
    System.out.println("exited");
  }
  public void mousePressed(MouseEvent event) 
  {
    System.out.println("DEBUG: action: " + action.getClass().getName());
    action.actionPerformed(event);
  }
  public void mouseReleased(MouseEvent event) 
  {
    System.out.println("released");
  }

  public SAction getAction()
  {
    return (action);
  }

  public void setTarget(JComponent c)
  {
    getAction().add(getClass().getName(), c);
    c.setVisible(false);
    this.target = target;
  }

/*
  public void createSubPanel(SMenu subPanel)
  {
    target.add(subPanel);
  }
*/

}


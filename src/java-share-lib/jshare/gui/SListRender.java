
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

import java.util.*;
import java.awt.*;
import javax.swing.*;
import jshare.action.*;
import jshare.style.*;

import jshare.gui.menu.SMenu;

public class SListRender extends DefaultListCellRenderer
{

  public SListRender(SListModel model)
  {
    this.model = model;
//add(new SMenu(new SAction()));
  }
//  final static ImageIcon longIcon = new ImageIcon("long.gif");
//  final static ImageIcon shortIcon = new ImageIcon("short.gif");


  @Override public Component getListCellRendererComponent(
      JList list,           // the list
      Object value,            // value to display
      int index,               // cell index
      boolean isSelected,      // is the cell selected
      boolean cellHasFocus)    // does the cell have focus
  {
    int i;

System.out.println("DEBUG: getListCellRendererComponent(): issel(" + isSelected + ") hasfoc(" + cellHasFocus + ")");

    if (value instanceof SListItem) {
      SListItem item = (SListItem)value;
      if (item.getComponent() != null)
        return (item.getComponent());
    }

    if (value instanceof JComponent) {
      return ((JComponent)value);
    }
    
    JLabel label;
    int cur_size = components.size();
    if (index >= cur_size) {
      for (i = cur_size; i <= index; i++) {
        label = new JLabel(" ");
        components.add(i, label);

        label.setOpaque(true);
        label.setHorizontalAlignment(SwingConstants.LEFT);
        label.setMinimumSize(new Dimension(32, 32));
      }
    }

    label = (JLabel)components.elementAt(index);

    if (!isSelected) {
      label.setForeground(Color.LIGHT_GRAY);
      label.setBackground(Color.BLACK);
    } else {
      label.setForeground(Color.BLACK);
      label.setBackground(Color.LIGHT_GRAY);
    }

    if (value instanceof SListItem) {
      SListItem item = (SListItem)value;
      if (cellHasFocus && !item.getDesc().equals("")) {
        label.setText(item.getTitle()+": "+item.getDesc());
      } else {
        label.setText(item.getTitle());
      }
    } else {
      label.setText(value.toString());
    }
System.out.println("DEBUG: RENDER[" + index + "]: text = " + label.getText());

    return (label);
  }

  public void setElement(int index, Object obj)
  {
    model.set(index, obj);
  }

  public void addElement(Object obj)
  {
    model.addElement(obj);
    invalidate();
  }

  protected SListModel model;
  protected Vector components = new Vector();
}

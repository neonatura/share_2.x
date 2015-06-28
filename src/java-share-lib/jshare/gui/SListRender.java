
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

import java.awt.Dimension;
import java.awt.Component;
import java.util.Vector;
import javax.swing.JLabel;
import javax.swing.JPanel;
import javax.swing.JList;
import javax.swing.ListCellRenderer;
import javax.swing.DefaultListCellRenderer;
import jshare.action.SAction;
import jshare.style.SToolTip;
import jshare.style.SFont;

import jshare.gui.menu.SMenu;

public class SListRender extends Vector implements ListCellRenderer
{

  public SListRender()
  {
add(new SMenu(new SAction()));
  }
//  final static ImageIcon longIcon = new ImageIcon("long.gif");
//  final static ImageIcon shortIcon = new ImageIcon("short.gif");


  public Component getListCellRendererComponent(
      JList list,           // the list
      Object value,            // value to display
      int index,               // cell index
      boolean isSelected,      // is the cell selected
      boolean cellHasFocus)    // does the cell have focus
  {
System.out.println("DEBUG: getListCellRenderComponent: index " + index + ", value " + value.toString() + " ..");
    if (index < 0 || index >= size())
      return (new JLabel(value.toString()));
      //return (null); /* blank_label = new SLabel() */

return (new JLabel("test"));
/*
    SListItem item = (SListItem)elementAt(index);
    return (item.getComponent());
*/
  }

}

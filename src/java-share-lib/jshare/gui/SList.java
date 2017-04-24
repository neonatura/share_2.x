
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

import java.util.Vector;
import java.awt.*;
import javax.swing.*;
import jshare.action.SAction;
import jshare.style.SToolTip;
import jshare.style.SFont;

public class SList extends JList
{

  protected SListRender render;

  public SList()
  {
    super(new SListModel());

    initList();
  }

  public SList(Vector v)
  {
    super(new SListModel());

    int i;

    initList();

    for (i = 0; i < v.size(); i++) {
      Object obj = v.elementAt(i);
      addListData(obj);
    }

  }

  public void initList()
  {
    setOpaque(false);
    setFont(SFont.getFont(this));
    render = new SListRender((SListModel)getModel());
    setCellRenderer(render);

setSize(new Dimension(80, 400));
  }


  public void addListData(Object obj)
  {
    render.addElement(obj);
  }

}

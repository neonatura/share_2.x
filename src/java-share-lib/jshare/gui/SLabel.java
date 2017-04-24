
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

import javax.swing.JLabel;
import jshare.style.SFont;

public class SLabel extends JLabel
{

  public SLabel(String labelText, int alignment)
  {
    super(labelText, alignment);
    setFont(SFont.getFont(this));
    setOpaque(false);
  }
  public SLabel(String labelText)
  {
    super(labelText);
    setFont(SFont.getFont(this));
    setOpaque(false);
  }
  public SLabel()
  {
    this("");
  } 
}

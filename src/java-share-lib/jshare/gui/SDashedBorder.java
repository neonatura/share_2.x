
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

import java.awt.Insets; 
import java.awt.ComponentOrientation;
import java.awt.Color;
import java.awt.Component;
import java.awt.Graphics;
import javax.swing.border.Border;
import java.util.Locale;

public class SDashedBorder implements Border
{
  public static final int THICKNESS = 2;
  Color color;
  int dashWidth;
  int dashHeight;

  Locale locale;

  public SDashedBorder()
  {
    this(Color.LIGHT_GRAY, 4, 4);
    this.locale = Locale.getDefault();
  }

  public SDashedBorder(Color c, int width, int height) 
  {
    if (width < 1) {
      throw new IllegalArgumentException("Invalid width: " + width);
    }
    if (height < 1) {
      throw new IllegalArgumentException("Invalid height: " + height);
    }
    color = c;
    dashWidth = width;
    dashHeight = height;
  }

  @Override
    public void paintBorder(Component c, Graphics g, int x, int y, int width, int height) {
      Insets insets = getBorderInsets(c);
      g.setColor(color);
      int numHigh = Math.round(height / dashHeight);
      int startPoint;

      for (int i = 0; i <= numHigh; i += 2) {
        startPoint = x + dashHeight * i;
        if (ComponentOrientation.getOrientation(locale).isLeftToRight()) {
          g.fillRect(x + width - insets.right, startPoint, THICKNESS, dashHeight);
        } else {
          g.fillRect(x + insets.left, startPoint, THICKNESS, dashHeight);

        }
      }
    }

  @Override
    public Insets getBorderInsets(Component c) {
      return new Insets(THICKNESS, THICKNESS, THICKNESS, THICKNESS);
    }

  @Override
    public boolean isBorderOpaque() {
      return false;
    }
}

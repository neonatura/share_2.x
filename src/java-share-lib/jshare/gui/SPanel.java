
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

import java.awt.GradientPaint;
import java.awt.ComponentOrientation;
import java.awt.Dimension;
import java.awt.Graphics2D;
import java.awt.Graphics;
import java.awt.Color;
import javax.swing.JPanel;
import javax.swing.border.TitledBorder;

public class SPanel extends JPanel
{

  private static final double PROPORTION_TO_FILL = 0.618;

  protected Color backgroundColor;
  protected Color lightBackgroundColor;
  protected ComponentOrientation componentOrientation;

  public SPanel()
  {
    setOpaque(false);
  }

  public void setBackground(Color backgroundColor)
  {
    this.backgroundColor = backgroundColor;
    this.lightBackgroundColor = null;
    super.setBackground(backgroundColor);
  }

  public void setGradient(ComponentOrientation componentOrientation, Color backgroundColor, Color lightBackgroundColor)
  {
    this.componentOrientation = componentOrientation;
    this.backgroundColor = backgroundColor;
    this.lightBackgroundColor = lightBackgroundColor;
  } 
  
  @Override protected void paintComponent(Graphics g) 
  {
    if (backgroundColor != null && lightBackgroundColor != null) {
      Graphics2D g2d = (Graphics2D)g;
      Dimension d = this.getSize();

      if (ComponentOrientation.LEFT_TO_RIGHT == componentOrientation) {
        g2d.setPaint(new GradientPaint(0, 0,  backgroundColor,
              ((int)(d.width * PROPORTION_TO_FILL)), 0, lightBackgroundColor, false));
      } else {
        g2d.setPaint(new GradientPaint(((int)(d.width * (1 - PROPORTION_TO_FILL))), 0, lightBackgroundColor,
              d.width, 0,  backgroundColor, false));
      }

      g2d.fillRect(0, 0, d.width , d.height);
    }

    super.paintComponent(g);
  }

  public void setTitle(String titleText)
  {

    if (titleText != null) {
      setBorder(new TitledBorder(titleText));
    } else {
      setBorder(null);
    }

  }

}

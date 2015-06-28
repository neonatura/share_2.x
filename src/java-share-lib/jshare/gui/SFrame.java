

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

import java.awt.Rectangle;
import java.awt.Toolkit;
import java.awt.Dimension;
import java.awt.GraphicsEnvironment;
import java.lang.reflect.Method;
import javax.swing.JFrame;
import jshare.action.SAction;

public class SFrame extends JFrame
{

  protected JFrame mainFrame;

  public SFrame(SAction action)
  {
    super(action.getTitle());
  }

  /**
   * Positions the specified dialog at a position relative to its parent.
   * 
   * @param dialog
   *            the dialog to be positioned.
   * @param horizontalPercent
   *            the relative location.
   * @param verticalPercent
   *            the relative location.
   */
  public void positionFrameRelativeToParent(JFrame mainFrame,
      final double horizontalPercent, final double verticalPercent) 
  {
    if (mainFrame != null) {
      JFrame dialog = this;
      final Dimension d = dialog.getSize();
      final Dimension p = mainFrame.getSize();

      final int baseX = mainFrame.getX() - d.width;
      final int baseY = mainFrame.getY() - d.height;
      final int w = d.width + p.width;
      final int h = d.height + p.height;
      int x = baseX + (int) (horizontalPercent * w);
      int y = baseY + (int) (verticalPercent * h);

      // make sure the dialog fits completely on the screen...
      final Rectangle s = getMaximumWindowBounds();
      x = Math.min(x, (s.width - d.width));
      x = Math.max(x, 0);
      y = Math.min(y, (s.height - d.height));
      y = Math.max(y, 0);
      dialog.setBounds(x + s.x, y + s.y, d.width, d.height);
    }

  }

  /**
   * Computes the maximum bounds of the current screen device. If this method
   * is called on JDK 1.4, Xinerama-aware results are returned. (See
   * Sun-Bug-ID 4463949 for details).
   * 
   * @return the maximum bounds of the current screen.
   */
  protected Rectangle getMaximumWindowBounds() 
  {
    final GraphicsEnvironment localGraphicsEnvironment = GraphicsEnvironment
      .getLocalGraphicsEnvironment();
    try {
      final Method method = GraphicsEnvironment.class.getMethod("getMaximumWindowBounds",
          (Class[]) null);
      return (Rectangle) method.invoke(localGraphicsEnvironment, (Object[]) null);
    } catch (Exception e) {
      // ignore ... will fail if this is not a JDK 1.4 ..
    }

    final Dimension s = Toolkit.getDefaultToolkit().getScreenSize();
    return new Rectangle(0, 0, s.width, s.height);
  }

}

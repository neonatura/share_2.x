

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

import jshare.action.*;
import jshare.net.*;
import jshare.engine.*;
import jshare.gui.panel.*;

public class SMainFrame extends SFrame
{

  public SMainFrame()
  {
    super(new SMainAction());

    setDefaultCloseOperation(SFrame.EXIT_ON_CLOSE);

    mainLayer = new SDesktopPane();
    getContentPane().add(mainLayer);

    initLayers();
  }


  public void initEngine(SEngine engine, SClient cli)
  {
    SActionPanel p;
    SAction[] actions;
    int i;

    actions = engine.getActions();
    p = new SActionPanel(mainLayer);
    for (i = 0; i < actions.length; i++) {
      p.addAction(actions[i]);
    }

    cli.start();
  }

  public void initLayers()
  {
    SActionPanel p;
    SAction[] actions;
    SEngine engine;
    SClient cli;
    int i;

    
    engine = new SSharenetEngine();
    cli = new SHttpClient(engine);
    initEngine(engine, cli);

    engine = new SSharecoinEngine();
    actions = engine.getActions();
    p = new SActionPanel(mainLayer);
    for (i = 0; i < actions.length; i++) {
      p.addAction(actions[i]);
    }
    cli = new SNetClient(engine);
    cli.start();

    engine = new SSystemEngine();
    actions = engine.getActions();
    p = new SActionPanel(mainLayer);
    for (i = 0; i < actions.length; i++) {
      p.addAction(actions[i]);
    }
    cli = new SLocalClient(engine);
    cli.start();
  }

  public SDesktopPane getMainPanel()
  {
    return (mainLayer);
  }

  protected SDesktopPane mainLayer;

}



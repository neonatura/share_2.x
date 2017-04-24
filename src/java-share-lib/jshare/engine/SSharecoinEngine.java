package jshare.engine;

import jshare.action.*;

public class SSharecoinEngine extends SNetEngine 
{

  @Override public SAction[] getActions()
  {
    SAction[] actions = new SAction[1];

    actions[0] = new SGeoAction(); 

    return (actions);
  }

}

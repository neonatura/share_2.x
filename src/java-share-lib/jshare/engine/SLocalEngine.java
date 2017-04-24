package jshare.engine;

import jshare.action.*;

public class SLocalEngine implements SEngine
{

  @Override public SAction[] getActions()
  {
    SAction[] actions = new SAction[2];

    actions[0] = new SExitAction(null);
    actions[1] = new SAccountAction();

    return (actions);
  }

}

package jshare.net;

import jshare.engine.*;

public class SClient extends SByteStream
{

  public SClient(SEngine engine)
  {
    this.engine = engine;
  }


  SEngine engine;
}



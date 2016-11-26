package jshare.net;

import jshare.engine.*;

public class SNetClient extends SClient
{
  
  public SNetClient(SEngine engine)
  {
    super(engine);
  }

  public void setHost(String hostname)
  {
    this.hostname = hostname;
  }
 
  public String getHost()
  {
    return (this.hostname);
  }

  public void setPort(int port)
  {
    this.port = port;
  }

  public int getPort()
  {
    return (port);
  }

  protected String hostname;
  protected int port;
}



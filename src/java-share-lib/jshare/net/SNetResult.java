package jshare.net;

import java.util.prefs.*;

public class SNetResult extends Properties implements SResult
{

  public SNetResult()
  {
  }

  @Override public String get(String key)
  {
    return (getProperty(key));
  }

  @Override public void set(String key, String val)
  {
    setProperty(key, val);
  }

} 

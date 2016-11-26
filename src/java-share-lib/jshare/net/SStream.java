package jshare.net;


public interface SStream extends java.io.Flushable
{

  /* ByteBuffer */
  public int read(byte[] b);

  public int write(byte[] b);

  public int read(byte[] b, int off, int len);

  public int write(byte[] b, int off, int len);


  /* Thread */
  public void start();

}


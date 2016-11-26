package jshare.net;

import java.io.*;
import java.nio.*;

public class SByteStream extends Thread implements SStream, java.io.Closeable
{

  public SByteStream()
  {
    input = ByteBuffer.allocate(4096);
    output = ByteBuffer.allocate(4096);
  }

  @Override public int write(byte[] b, int off, int len)
  {
    output.put(b, off, len);
    return (len);
  }

  @Override public int write(byte[] b)
  {
    return (write(b, 0, b.length));
  }

  @Override public int read(byte[] b, int off, int len)
  {
    input.get(b, off, len);
    return (len);
  }

  @Override public int read(byte[] b)
  {
    return (read(b, 0, b.length));
  }

  @Override public void close() throws IOException
  {
  }

  @Override public void flush() throws IOException
  {
  }

  @Override public void run()
  {
  }

  protected java.nio.ByteBuffer input;

  protected java.nio.ByteBuffer output;

}

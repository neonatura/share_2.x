
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
package jshare.param;

import java.io.*;
import java.util.*;
import javax.crypto.*;
import java.security.*;
import javax.crypto.*;
import javax.crypto.spec.*;

public class SParamTable extends HashMap<String,GuardedObject> implements Serializable
{

  public final static long serialVersionUID = 44L;

  public final static int MAX_CACHE_SIZE = 10;

  public SParamTable()
  {
    cache = new Vector();
  }

  public SParamTable(String label, Key key)
  {
    this.label = label;
    this.key = key;
    cache = new Vector();
  }

  public void addCache(SParam param)
  {
    if (cache.size() >= MAX_CACHE_SIZE)
      cache.setSize(MAX_CACHE_SIZE - 1);
    cache.remove(param);
    cache.add(0, param); /* insert at beginning */
  }

  public Vector getCache()
  {
    return (cache);
  }

  public void add(SParam param)
  {
    SParamKey name = param.getName();
    name.setTable(getLabel().hashCode());
    GuardedObject obj = new GuardedObject(param, (Guard)name);
    String label = name.toString();
    remove(label);
    super.put(label, obj);
System.out.println("DEBUG: SParamTable: add[" + getLabel() + "]: stored record: " + name);

    addCache(param);
  }

  public SParam find(SParamKey name)
  {
    String label = name.toString();
    GuardedObject obj = (GuardedObject)super.get(label);
    SParam param = null;

    if (obj == null) {
System.out.println("DEBUG: SParamTable: find[" + getLabel() + "]: no record found: " + name);
      return (null);
    }

    try {
      param = (SParam)obj.getObject(); 
    } catch (SecurityException sec_err) {
      param = new SParamLock(param);
System.out.println("DEBUG: SParamTable: SecurityException!");
    }

    return (param);
  }
 
  public boolean exists(SParamKey name)
  {
    return (containsKey(name));
  }

  protected void setKey(Key key)
  {
    this.key = key;
  }

  protected Key getKey()
  {
    return (key);    
  }

  public String getLabel()
  {
    return (label);
  }

  public void setLabel()
  {
    this.label = label;
  }

  public void flush()
  {
    SParamTable.store(this); 
  }

  static public void store(SParamTable obj)
  {
    Key key = obj.getKey();

System.out.println("DEBUG: SParamTable.store[" + obj.getLabel() + "]: " + bytesToHex(key.getEncoded())); 

    try {
      String path = obj.getLabel();
      OutputStream fout = SFileMap.getOutputStream(path);
      ObjectOutputStream oos = new ObjectOutputStream(fout);

      Cipher c = Cipher.getInstance("AES/CBC/PKCS5Padding");
      c.init(Cipher.ENCRYPT_MODE, key);
      SealedObject sobj = new SealedObject(obj, c);

      oos.writeObject(sobj);
//      oos.close();
    } catch (IOException io_err) {
      io_err.printStackTrace();
    } catch (IllegalBlockSizeException blk_err) {
      blk_err.printStackTrace();
    } catch (InvalidKeyException key_err) {
      key_err.printStackTrace();
    } catch (NoSuchAlgorithmException alg_err) {
      alg_err.printStackTrace();
    } catch (NoSuchPaddingException pad_err) {
      pad_err.printStackTrace();
    }

  }

  /** 
   * Retrieve a stored set of table records with derived key.
   */
  static public SParamTable restore(String name, Key key) throws InvalidKeyException
  {
    SParamTable t = null;

System.out.println("DEBUG: SParamTable.restore[" + name + "]: " + bytesToHex(key.getEncoded())); 

    try {
      String path = name;
      InputStream in = SFileMap.getInputStream(path);
      ObjectInputStream oos = new ObjectInputStream(in);
      SealedObject sobj = (SealedObject)oos.readObject();
      t = (SParamTable)sobj.getObject(key);
    } catch (IOException io_err) {
//      io_err.printStackTrace();
    } catch (ClassNotFoundException class_err) {
      class_err.printStackTrace();
    } catch (NoSuchAlgorithmException alg_err) {
      alg_err.printStackTrace();
    }

    if (t != null) {
      t.key = key;
    }

    return (t);
  }

  @Override public String toString()
  {
    return (getLabel());
  }

  static public SecretKey createSecret()
  {
    SecretKey secretKey = null;

    try {
      KeyGenerator keyGen = KeyGenerator.getInstance("AES");
      keyGen.init(256);
      secretKey = keyGen.generateKey();
    } catch (NoSuchAlgorithmException alg_err) {
      alg_err.printStackTrace();
    }

    return (secretKey);
  }

  static public byte[] byte_xor(int len, byte[] a, byte[] b)
  {
    byte[] ret;
    byte a_val;
    byte b_val;
    int i;

    ret = new byte[len];
    for (i = 0; i < len; i++) {
      if (i < a.length)
        a_val = a[i];
      else
        a_val = 1;
      if (i < b.length)
        b_val = b[i];
      else
        b_val = 1;
      ret[i] = (byte)(a_val ^ b_val);
    }

    return (ret);
  }

  /**
   * Create a secret key based on the combination of a table record 'key' name and the master key.
   */
  static public SecretKey createSecret(SParamKey name)
  {
    SecretKey mkey = getMasterKey(); 
    SecretKeySpec key;
    byte m_b[];
    byte n_b[];
    byte b[];

    m_b = mkey.getEncoded(); 
    n_b = name.toBytes();
    b = byte_xor(32, m_b, n_b);

    key = new SecretKeySpec(b, "AES/CBC/PKCS5Padding");
    return ((SecretKey)key);
  }

  static public SecretKey getMasterKey()
  {
    SecretKey key = null;

    try {
      File path = new File(".master");
      FileInputStream f = new FileInputStream(path);
      ObjectInputStream in = new ObjectInputStream(f);
      key = (SecretKey)in.readObject();
      in.close();
    } catch (IOException io_err) {
      key = null;
    } catch (ClassNotFoundException class_err) {
      return (null);
    }

    if (key == null) {
      try {
        File path = new File(".master");
        FileOutputStream f = new FileOutputStream(path);
        ObjectOutputStream out = new ObjectOutputStream(f);

        key = createSecret();
        out.writeObject(key);

        out.close();

        path.setReadOnly();
      } catch (IOException io_err) {
        io_err.printStackTrace();
      }
    }

    if (key != null) {
System.out.println("DEBUG: MASTER KEY: " + bytesToHex(key.getEncoded()));
    } else {
System.out.println("DEBUG: MASTER KEY: <NULL>");
    }

    return (key);
  }

  final protected static char[] hexArray = "0123456789ABCDEF".toCharArray();
  static public String bytesToHex(byte[] bytes) {
      char[] hexChars = new char[bytes.length * 2];
      for ( int j = 0; j < bytes.length; j++ ) {
          int v = bytes[j] & 0xFF;
          hexChars[j * 2] = hexArray[v >>> 4];
          hexChars[j * 2 + 1] = hexArray[v & 0x0F];
      }
      return new String(hexChars);
  }



  protected String label = "";
  protected Vector cache;

  transient protected Key key;

}



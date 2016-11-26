package jshare.net;

import java.io.*;
import java.net.*;
import java.util.*;

import jshare.engine.*;

public class SHttpClient extends SNetClient
{

  public SHttpClient(SEngine engine)
  {
    super(engine);
  }

  public String getPage()
  {
    return (page);
  }

  public void setPage(String page)
  {
    this.page = page;
  }

  public String getUrl()
  {
    return ("http://" + getHost() + ":" + getPort() + "/" + getPage());
  }

 
  public static String executePost(String targetURL, String urlParameters) {
    HttpURLConnection connection = null;

    try {
      //Create connection
      URL url = new URL(targetURL);
      connection = (HttpURLConnection) url.openConnection();
      connection.setRequestMethod("POST");
      connection.setRequestProperty("Content-Type", 
          "application/x-www-form-urlencoded");

      connection.setRequestProperty("Content-Length", 
          Integer.toString(urlParameters.getBytes().length));
      connection.setRequestProperty("Content-Language", "en-US");  

      connection.setUseCaches(false);
      connection.setDoOutput(true);

      //Send request
      DataOutputStream wr = new DataOutputStream (
          connection.getOutputStream());
      wr.writeBytes(urlParameters);
      wr.close();

      //Get Response  
      InputStream is = connection.getInputStream();
      BufferedReader rd = new BufferedReader(new InputStreamReader(is));
      StringBuilder response = new StringBuilder(); // or StringBuffer if Java version 5+
      String line;
      while ((line = rd.readLine()) != null) {
        response.append(line);
        response.append('\r');
      }
      rd.close();
      return response.toString();
    } catch (Exception e) {
      e.printStackTrace();
      return null;
    } finally {
      if (connection != null) {
        connection.disconnect();
      }
    }
  }

  protected String page;
}

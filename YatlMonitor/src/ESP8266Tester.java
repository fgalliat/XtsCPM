import java.io.*;
import java.util.ArrayList;
import java.util.Comparator;
import java.util.HashMap;
import java.util.List;
import java.util.Map;

public class ESP8266Tester {

    public static void main(String[] args) throws Exception {
        // if ( args == null || args.length != 1 ) {
        //     System.out.println("Usage : ");
        //     System.out.println("");
        //     System.out.println("java ESP8266");
        //     return;
        //   }
          process(  );
    }

    public static String readLine() throws Exception {
      // return Terminal.getInstance().readSerLine(500, true);
      // ESP can be slow to respond !!!
      return Terminal.getInstance().readSerLine(6000, true);
    }

    public static void write(String str) throws Exception {
      Terminal.getInstance().serPrint(str);
    }

    public static void writeCMD(String str) throws Exception {
      // waits for CR+LF
      Terminal.getInstance().serPrint(str+"\r\n");
    }

    public static void write(char ch) throws Exception {
      Terminal.getInstance().serWrite(ch);
    }

    // ====================
    static boolean testModule() throws Exception {
      writeCMD("AT");
      String line;
      while( (line = readLine()) != null ) {
        // dbug(">>> "+line);
        if ( line.equals("OK") ) { return true; }
        if ( line.equals("ERROR") ) { return false; }
      }
      return false;
    }

    static boolean resetModule() throws Exception {
      writeCMD("AT+RST");
      String line;
      while( (line = readLine()) != null ) {
        // if ( line.equals("OK") ) { return true; }
        // ESP-!2
        if ( line.contains("ready") ) { return true; }
        if ( line.equals("ERROR") ) { return false; }
      }
      return false;
    }

    // STA : wifi AP client
    static final int WIFI_MODE_STA = 1;
    // AP : wifi soft AP server
    static final int WIFI_MODE_AP = 2;

    static final String[] wifiModes = {
      "none", "STA", "AP", "STA+AP"
    };

    static int getWifiMode() throws Exception {
      writeCMD("AT+CWMODE?");
      int result = -1;
      String line;
      while( (line = readLine()) != null ) {
        // System.out.println("? "+line);
        if ( line.startsWith("+CWMODE:") ) {
          result = Integer.parseInt( line.substring("+CWMODE:".length()) );
        }
        if ( line.equals("OK") ) { break; }
        if ( line.equals("ERROR") ) { break; }
      }
      if ( line == null ) { dbug("NULL LINE !!!!!!"); }
      return result;
    }

    static boolean setWifiMode(boolean sta, boolean ap) throws Exception {
      int mode = 0;
      if ( sta ) { mode += WIFI_MODE_STA; }
      if ( ap ) { mode += WIFI_MODE_AP; } // can be BOTH (3)
      writeCMD("AT+CWMODE="+mode);
      String line;
      while( (line = readLine()) != null ) {
        // System.out.println("...? "+line);
        if ( line.equals("OK") ) { return true; }
        if ( line.equals("ERROR") ) { return false; }
      }
      if ( line == null ) { dbug("NULL LINE !!!!!!"); }
      return false;
    }

    // returns SSID
    static String STA_getSSID() throws Exception {
      writeCMD("AT+CWJAP?");
      String result = null;
      String line;
      while( (line = readLine()) != null ) {
        // System.out.println("...? "+line);
        if ( line.length() == 0 ) { continue; }
        if ( line.equals("OK") ) { break; }
        if ( line.equals("ERROR") ) { break; }
        if ( line.startsWith("+CWJAP") ) {
          line = line.substring(line.indexOf('"')+1);
          line = line.substring(0, line.indexOf('"'));
          result = line;
        }
      }
      return result;
    }

    // infos on AP mode
    static String AP_infos() throws Exception {
      writeCMD("AT+CWSAP?");
      String line, result = null;
      while( (line = readLine()) != null ) {
        if ( line.length() == 0 ) { continue; }
        if ( line.equals("OK") ) { break; }
        if ( line.equals("ERROR") ) { return null; }
        if ( line.startsWith("+CWSAP") ) {
          // contains BOTH ssid + psk
          System.out.println("? "+line);
          line = line.substring(line.indexOf('"')+1);
          line = line.substring(0, line.indexOf('"'));
          result = line;
        }
      }
      return result;
    }

    static String getSSID() throws Exception {
      return (isStaMode() ? STA_getSSID() : AP_infos());
    }

    static boolean arrayContains(List<String> list, String key) {
      // Ugly impl. just to be close to C++ impl.
      for(int i=0; i < list.size(); i++) {
        if ( list.get(i).equals(key) ) {
          return true;
        }
      }
      return false;
    }

    // return list of available APs
    // for now requires setWifiMode(true, true) ... see later
    // for setWifiMode(false, true)
    // but seems must be called explicitly
    static String[] AP_allSSID() throws Exception {
      writeCMD("AT+CWLAP");
      List<String> result = new ArrayList<String>();
      String line;
      while( (line = readLine()) != null ) {
        // System.out.println("...? "+line);
        if ( line.startsWith("+CWLAP:") ) {
          line = line.substring(line.indexOf('"')+1);
          line = line.substring(0, line.indexOf('"'));
          if ( !arrayContains(result, line) ) {
            // beware there are really APs
            // w/ same names & # MACs !!!!
            result.add(line);
          }
        }
        if ( line.equals("OK") ) { break; }
        if ( line.equals("ERROR") ) { break; }
      }
      if ( line == null ) { dbug("NULL LINE"); }

      // result.sort( new Comparator<String>() {
      //   @Override
      //   public int compare(String o1, String o2) {
      //     return o1.compareTo(o2);
      //   }
      // } );

      return result.toArray( new String[ result.size() ] );
    }

    // connect to an AP
    static boolean AP_connect(String ssid, String pass) throws Exception {
      writeCMD("AT+CWJAP=\""+ssid+"\",\""+pass+"\"");
      String line;
      while( (line = readLine()) != null ) {
        System.out.println("? "+line);
        if ( line.equals("OK") ) { return true; }
        if ( line.equals("ERROR") ) { return false; }
      }
      return false;
    }

    // disconnect from current AP
    static boolean AP_disconnect() throws Exception {
      writeCMD("AT+CWQAP");
      String line;
      while( (line = readLine()) != null ) {
        // System.out.println("? "+line);
        if ( line.equals("OK") ) { return true; }
        if ( line.equals("ERROR") ) { return false; }
      }
      return false;
    }

    

    // set SoftAP mode
    static boolean AP_set(String ssid, String psk) throws Exception {
      writeCMD("AT+CWSAP=\""+ssid+"\",\""+psk+"\",5,3");
      String line;
      while( (line = readLine()) != null ) {
        System.out.println("? "+line);
        if ( line.equals("OK") ) { return true; }
        if ( line.equals("ERROR") ) { return false; }
      }
      return false;
    }

    // return list of connected client
    static String[] AP_clients() throws Exception {
      writeCMD("AT+CWLIF");
      List<String> result = new ArrayList<String>();
      String line;
      while( (line = readLine()) != null ) {
        // System.out.println("? "+line);
        result.add(line);
        if ( line.equals("OK") ) { break; }
        if ( line.equals("ERROR") ) { break; }
      }
      return result.toArray( new String[ result.size() ] );
    }

    private static String XX_getIP(boolean STA) throws Exception {
      if ( STA ) { writeCMD("AT+CIPSTA?"); }
      else { writeCMD("AT+CIPAP?"); }
      
      String result = null;
      String line;
      while( (line = readLine()) != null ) {
        // System.out.println("? "+line);
        if ( line.length() == 0 ) { continue; }
        if ( line.equals("OK") ) { break; }
        if ( line.equals("ERROR") ) { return null; }
        if ( line.startsWith("+CIP") ) {
          // ip: / gateway: / netmask:
          if ( line.contains("ip:") ) {
            line = line.substring(line.indexOf('"')+1);
            line = line.substring(0, line.indexOf('"'));
            result = line;
          }
        }
      }
      return result;
    }

    private static boolean XX_setIP(boolean STA, String ip) throws Exception {
      if ( STA ) { writeCMD("AT+CIPSTA=\""+ip+"\""); }
      else { writeCMD("AT+CIPAP=\""+ip+"\""); }
      
      String result = null;
      String line;
      while( (line = readLine()) != null ) {
        // System.out.println("? "+line);
        result = line;
        if ( line.equals("OK") ) { return true; }
        if ( line.equals("ERROR") ) { return false; }
      }
      return false;
    }

    static boolean isStaMode()  throws Exception {
      // TODO : better cf can be BOTH
      int wmode = getWifiMode();
      return wmode == WIFI_MODE_STA ||
      false;
      // wmode == WIFI_MODE_STA + WIFI_MODE_STA;
    }

    static String IP_get()  throws Exception {
      return XX_getIP(isStaMode());
    }

    static boolean IP_set(String ip)  throws Exception {
      return XX_setIP(isStaMode(), ip);
    }
    
    // ---------------

    static boolean closeSocket() throws Exception {
      writeCMD("AT+CIPCLOSE()");
      String line;
      while( (line = readLine()) != null ) {
        // System.out.println("? "+line);
        if ( line.equals("OK") ) { break; }
        if ( line.equals("ERROR") ) { return false; }
      }
      return true;
    }

    // "192.168.1.135" 8080 "/login.jsp?user=toto&pass=titi"
    static void wget(String host, int port, String query)  throws Exception {

      if ( host.equals("$home") ) {
        host = getHomeServer();
      }

      // start
      writeCMD("AT+CIPSTART=\"TCP\",\""+host+"\","+port);
      String line;
      while( (line = readLine()) != null ) {
        System.out.println("? "+line);
        if ( line.equals("OK") ) { break; }
        if ( line.equals("ERROR") ) { 
          closeSocket();
          return; 
        }
      }

      String fullQ = "GET "+query+"\r\n";
      writeCMD("AT+CIPSEND="+fullQ.length() );
      writeCMD( fullQ );
      writeCMD("+++"); // EOT

      while( (line = readLine()) != null ) {
        if ( line.equals("ERROR") ) { closeSocket(); return; }
        if ( line.equals("SEND OK") ) { break; }
      }
      
      // listen for +IPD(s) ....
      String buffer = "";
      while( (line = readLine()) != null ) {
        // System.out.println("? "+line);
        // +IPD,$length:$content
        if ( line.startsWith("+IPD,") ) {
          int len = -1;
          try { len = Integer.parseInt(line.substring(5, line.indexOf(":") )); }
          catch(Exception ex) {}
          String keptLine = line.substring(line.indexOf(":")+1);
          if ( keptLine.length() < len) {
            buffer += keptLine + "\n"; //"\r\n";
          } else if ( keptLine.length() == len) {
            buffer += keptLine;
          } else {
            // < len
            keptLine = keptLine.substring(0, len);
            buffer += keptLine; // w/o LF
            line = line.substring(5+len);
            if ( line.endsWith("CLOSED") ) { break; }
          }
        } else if ( line.startsWith("CLOSED") ) { break; }
        else {
          buffer += line + "\n";
          if ( line.endsWith("CLOSED") ) { 
            buffer = buffer.substring( 0, buffer.length() - 7 ); 
            break; 
          }
        }
      }

      dbug(buffer);

      // close
      closeSocket();
    }

    // ======================

    static void dbug(Object o) {
      System.out.println("(dbg) "+o);
    }

    static String HOME_SSID = null;
    static String HOME_LOCAL_SRV = null;
    static String HOME_REMOTE_SRV = null;

    static Map<String,String> ssidPsks = new HashMap<String,String>();

    static String getPskForSSID(String ssid) {
      return ssidPsks.get(ssid);
    }

    static String getHomeServer() throws Exception {
      if ( HOME_SSID == null ) {
        return null;
      }

      String curSSID = getSSID();
      if ( curSSID == null ) {
        return null;
      }

      if ( curSSID.equals(HOME_SSID) ) {
        return HOME_LOCAL_SRV;
      } else {
        return HOME_REMOTE_SRV;
      }
    }

    static void parseWIFIconfig(String filename) throws Exception {
      if ( ! new File( filename ).exists() ) {
        dbug("Warning NO WiFi config found !");
        return;
      }

      BufferedReader reader = new BufferedReader(new FileReader(filename));
      String line;
      line = reader.readLine();
      if ( line == null || line.trim().isEmpty() ) {
        dbug("Warning EMPTY WiFi config found !");
        reader.close();
        return;
      }

      // $HOME_SSID$:$HOME_HOST$:$REMOTE_HOST$\n
      String[] tks = line.split(":");
      // TODO : handle errors
      HOME_SSID = tks[0];
      HOME_LOCAL_SRV = tks[1];
      HOME_REMOTE_SRV = tks[2];

      // $SSID$:$PSK$\n (one line per SSID)
      ssidPsks.clear();
      while( (line = reader.readLine() ) != null ) {
        tks = line.split(":");
        // TODO : handle errors
        ssidPsks.put( tks[0], tks[1] );
      }

      reader.close();
      dbug("WiFi config successfully loaded");
    }

    static void process() throws Exception {

        parseWIFIconfig("./wifi.psk");

        // let's ignore .properties file ....
        String ESPCommPort = "COM4";

        if ( true ) { ESPCommPort = "COM8"; }

        Terminal.getInstance().setCommPort( ESPCommPort );
        Terminal.getInstance().setSilent(true);

        boolean ok = Terminal.getInstance().reconnect(false);
        if ( !ok ) {
            throw new Exception("Could not connect to ESP8266 !!!!");
        }

        BufferedReader keyb = new BufferedReader( new InputStreamReader( System.in ) );

        // String line = null;
        // while( (line = readLine()) != null ) {
        //   System.out.println("? "+line);
        // }
        Terminal.getInstance().flushRX();

        testModule();


        while(true) {
          int _mode = getWifiMode();
          dbug( "Current mode is : "+wifiModes[_mode] );
          dbug( "Home Server is : "+getHomeServer() );
          dbug("");

        dbug("a. test module");
        dbug("b. reset module");

        dbug("c. setSTAmode + connectAP + getIP + getSSID");
        dbug("d. scan APs SSIDs");
        dbug("e. open APmode");
        dbug("f. setSTAmode");
        dbug("g. getIP + getSSID");

        dbug("h. wgetTest");

        dbug("x. eXit");

        String choice = keyb.readLine();

        if (choice == null) { return; }
        choice = (""+choice.charAt(0)).toLowerCase();
        char ch = choice.charAt(0);

        if ( ch == 'x' ) { break; }
        else if ( ch == 'a' ) { boolean _ok = testModule(); dbug("Module is "+(_ok ? "OK":"NOK")); }
        else if ( ch == 'b' ) { resetModule(); }
        else if ( ch == 'c' ) { 
          // STA mode - AP client
          dbug("Connect to SSID ? ");
          String ssid = keyb.readLine();
          String psk;
          psk = getPskForSSID(ssid);
          if ( psk == null ) {
            dbug("Connect with PSK ? ");
            psk = keyb.readLine();
          } else {
            dbug("Found a PSK ! ");
          }

          setWifiMode(true, false);
          AP_connect(ssid, psk);
          String ip = IP_get();
          dbug("IP : "+ip);
          ssid = getSSID();
          dbug("SSID : "+ssid);
        }
        else if ( ch == 'd' ) { 
          setWifiMode(true, false);
          String[] ssids = AP_allSSID();
          for(String ssid : ssids) {
            dbug("SSID:"+ssid);
          }
        } else if ( ch == 'e' ) { 
          dbug("Open SSID ? ");
          String ssid = keyb.readLine();
          dbug(" with PSK ? ");
          String psk = keyb.readLine();
          setWifiMode(false, true);
          boolean _ok = AP_set(ssid, psk);
          dbug("Soft AP opened = "+_ok);
          String ip = IP_get();
          dbug("IP : "+ip);
          AP_infos();
        } else if ( ch == 'f' ) { 
          setWifiMode(true, false);
        } else if ( ch == 'g' ) { 
          String ip = IP_get();
          dbug("IP : "+ip);
          String ssid = getSSID();
          dbug("SSID : "+ssid);
        }

        else if ( ch == 'h' ) {
          // wget("www.google.fr", 80, "/search?q=esp8266");
          wget("$home", 8089, "/");
        }

      }

    }
  
    // // static PrintStream serialOut;
  
    // static void printLineToCPM(String line) throws Exception {
    // //   serialOut.print(line+"\r\n");
    //     Terminal.getInstance().serPrintln(line+"\r");
    // }
  
    // static void printLineToCPMSlowly(String line) throws Exception {
    //   // Zzz(50);
    // //   serialOut.print(line+"\r\n");
    //     Terminal.getInstance().serPrintln(line+"\r");
    //   // Zzz(50);
    // }
  
    static void Zzz(long time) {
        try {
          Thread.sleep(time);
        } catch(Exception ex) {}
    }


}
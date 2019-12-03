import java.io.*;
import java.util.ArrayList;
import java.util.List;

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
      return Terminal.getInstance().readSerLine(500, true);
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
        if ( line.equals("OK") ) { return true; }
      }
      return false;
    }

    static boolean resetModule() throws Exception {
      writeCMD("AT+RST");
      String line;
      while( (line = readLine()) != null ) {
        if ( line.equals("OK") ) { return true; }
      }
      return false;
    }

    static final int WIFI_MODE_STA = 1;
    static final int WIFI_MODE_AP = 2;

    static int getWifiMode() throws Exception {
      writeCMD("AT+CWMODE?");
      int result = -1;
      String line;
      while( (line = readLine()) != null ) {
        // System.out.println("? "+line);
        if ( line.startsWith("+CWMODE:") ) {
          return Integer.parseInt( line.substring("+CWMODE:".length()) );
        }
        if ( line.equals("OK") ) { break; }
      }
      return result;
    }

    static boolean setWifiMode(boolean sta, boolean ap) throws Exception {
      int mode = 0;
      if ( sta ) { mode += WIFI_MODE_STA; }
      if ( ap ) { mode += WIFI_MODE_AP; } // can be BOTH (3)
      writeCMD("AT+CWMODE="+mode);
      String line;
      while( (line = readLine()) != null ) {
        // System.out.println("? "+line);
        if ( line.equals("OK") ) { return true; }
      }
      return false;
    }

    // returns SSID
    static String AP_getSSID() throws Exception {
      writeCMD("AT+CWJAP?");
      String result = null;
      String line;
      while( (line = readLine()) != null ) {
        // System.out.println("? "+line);
        result = line;
        if ( line.equals("OK") ) { break; }
      }
      return result;
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
        // System.out.println("? "+line);
        result.add(line);
        if ( line.equals("OK") ) { break; }
      }
      return result.toArray( new String[ result.size() ] );
    }

    // connect to an AP
    static boolean AP_connect(String ssid, String pass) throws Exception {
      writeCMD("AT+CWJAP=\""+ssid+"\",\""+pass+"\"");
      String line;
      while( (line = readLine()) != null ) {
        // System.out.println("? "+line);
        if ( line.equals("OK") ) { return true; }
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
      }
      return false;
    }

    // infos on STA mode
    static boolean STA_infos() throws Exception {
      writeCMD("AT+CWSAP?");
      String line;
      while( (line = readLine()) != null ) {
        System.out.println("? "+line);
        if ( line.equals("OK") ) { return true; }
      }
      return false;
    }

    // set STA mode
    static boolean STA_set(String ssid, String psk) throws Exception {
      writeCMD("AT+CWSAP=\""+ssid+"\",\""+psk+"\",5,3");
      String line;
      while( (line = readLine()) != null ) {
        // System.out.println("? "+line);
        if ( line.equals("OK") ) { return true; }
      }
      return false;
    }

    // return list of connected client
    static String[] STA_clients() throws Exception {
      writeCMD("AT+CWLIF");
      List<String> result = new ArrayList<String>();
      String line;
      while( (line = readLine()) != null ) {
        // System.out.println("? "+line);
        result.add(line);
        if ( line.equals("OK") ) { break; }
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
        result = line;
        if ( line.equals("OK") ) { break; }
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
      }
      return false;
    }

    static boolean isStaMode()  throws Exception {
      return getWifiMode() == WIFI_MODE_STA;
    }

    static String IP_get()  throws Exception {
      return XX_getIP(isStaMode());
    }

    static boolean IP_set(String ip)  throws Exception {
      return XX_setIP(isStaMode(), ip);
    }
    
    // ---------------

    // "192.168.1.135" 8080 "/login.jsp?user=toto&pass=titi"
    static void wget(String host, int port, String query)  throws Exception {
      // start
      writeCMD("AT+CIPSTART=\"TCP\",\""+host+"\", port");

      String fullQ = "GET "+query;
      writeCMD("AT+CIPSEND="+fullQ.length() );
      writeCMD( fullQ );
      
      // listen for +IPD(s) ....
      String line;
      while( (line = readLine()) != null ) {
        System.out.println("? "+line);
      }

      // close
      writeCMD("AT+CIPCLOSE()");
    }

    // ======================

    static void dbug(Object o) {
      System.out.println("(dbg) "+o);
    }

    static void process() throws Exception {
        // lets use .properties file ....
        // String ESPCommPort = "COM13";
        // Terminal.getInstance().setCommPort( ESPCommPort );
        boolean ok = Terminal.getInstance().reconnect(false);
        if ( !ok ) {
            throw new Exception("Could not connect to ESP8266 !!!!");
        }

        BufferedReader keyb = new BufferedReader( new InputStreamReader( System.in ) );

        String line = null;
        while( (line = readLine()) != null ) {
          System.out.println("? "+line);
        }

        testModule();

        while(true) {
        dbug("a. test module");
        dbug("b. reset module");

        dbug("c. setAPmode + connectAP + getIP + getSSID");
        dbug("d. scan APs SSIDs");
        dbug("e. setSTAmode + getIP + getSSID");

        dbug("f. wgetTest");

        dbug("x. eXit");

        String choice = keyb.readLine();

        if (choice == null) { return; }
        choice = (""+choice.charAt(0)).toLowerCase();
        char ch = choice.charAt(0);

        if ( ch == 'x' ) { break; }
        else if ( ch == 'a' ) { testModule(); }
        else if ( ch == 'b' ) { resetModule(); }
        else if ( ch == 'c' ) { 
          dbug("Connect to SSID ? ");
          String ssid = keyb.readLine();
          dbug("Connect with PSK ? ");
          String psk = keyb.readLine();

          setWifiMode(false, true);
          AP_connect(ssid, psk);
          String ip = IP_get();
          dbug("IP : "+ip);
          ssid = AP_getSSID();
          dbug("SSID : "+ssid);
        }
        else if ( ch == 'd' ) { 
          setWifiMode(false, true);
          String[] ssids = AP_allSSID();
          // deals  w/ that array ..
        } else if ( ch == 'e' ) { 
          dbug("Open SSID ? ");
          String ssid = keyb.readLine();
          dbug(" with PSK ? ");
          String psk = keyb.readLine();
          setWifiMode(true, false);
          STA_set(ssid, psk);
          String ip = IP_get();
          dbug("IP : "+ip);
          STA_infos();
        } else if ( ch == 'f' ) {
          wget("http://www.google.fr", 80, "/search?q=esp8266");
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
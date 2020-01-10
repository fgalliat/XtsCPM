import java.io.*;

public class BdosSerialSender {

    public static void main(String[] args) throws Exception {
        if ( args == null || args.length < 1 ) {
            System.out.println("Usage : ");
            System.out.println("");
            System.out.println("java BdosSerialSender READ.ME [c] [y to compile]");
            return;
          }
          process( args[0], args.length > 1 ? args[1] : "c", args.length > 2 );
    }

    static String okBuff = ""; 
    static boolean waitOk() throws Exception {
      String resp;
      do {
        resp = Terminal.getInstance().readString(3);
        System.out.println(">> "+resp);
        okBuff+=resp.trim().replaceAll("\n", "");
        if ( okBuff.endsWith("+OK") ) { break; }
        if ( okBuff.endsWith("-OK") ) { return false; }
      } while( true );

      return true;
    }

    static void process(String filename, String driveName, boolean launchCmdPas) throws Exception {
        File f = new File(filename);
        String destName = f.getName().toUpperCase();
        if ( destName.length() > 12 ) {
          throw new IllegalArgumentException("not a 8.3 filename !");
        }
        if ( !f.exists() ) {
          throw new IllegalArgumentException("not found filename !");
        }
        if (driveName.length() != 1) {
          throw new IllegalArgumentException("drive letter not found !");
        }
  
        boolean ok = Terminal.getInstance().reconnect(false);
        if ( !ok ) {
            throw new Exception("Could not connect to YATL !!!!");
        }

        // DOWN.COM compiled from DOWN.PAS
        printLineToCPM("c:DOWN");
        printLineNoCPM("Go !"); // sends some bytes
        ok = waitOk();
        if ( !ok ) { System.out.println("Error 0x01"); return; }
        printLineNoCPM("/"+driveName+"/0/"+destName);
        ok = waitOk();
        if ( !ok ) { System.out.println("Error 0x02"); return; }
        printLineNoCPM(""+ (f.length()) );
        ok = waitOk();
        if ( !ok ) { System.out.println("Error 0x03"); return; }

        FileInputStream fin = new FileInputStream(f);
        int packetLen = 64;
        byte[] buff = new byte[packetLen];
        while( fin.available() > 0 ) {
          int len = fin.read(buff, 0, packetLen);
          Terminal.getInstance().serWrite( buff, len );
          Zzz(5);
        }
        fin.close();
        System.out.println("-EOF-");
        // serialOut.close();

        if (launchCmdPas) {
          Zzz(1500);
          printLineToCPM("c:CLS");
          Zzz(1500);
          printLineToCPM("b:TURBO");
          Zzz(1500);
          // Show messages (assumes that C:TURBO.MSG exists)
          Terminal.getInstance().serPrint("Y");
          Zzz(500);
          // Run
          Terminal.getInstance().serPrint("r");
          Zzz(500);
          Terminal.getInstance().serPrint(destName.substring(0, destName.indexOf('.'))+"\r");
          
          // System.out.println("Wait Enter ...");
          // new BufferedReader(new InputStreamReader(System.in)).readLine();
          // // if !wait -> when JVM ends it break compilation ...
        }
    }
  
  
    static void printLineToCPM(String line) throws Exception {
      Terminal.getInstance().serPrintln(line+"\r");
    }
    static void printLineNoCPM(String line) throws Exception {
      Terminal.getInstance().serPrintln(line);
    }
  
    static void printLineToCPMSlowly(String line) throws Exception {
      // Zzz(50);
    //   serialOut.print(line+"\r\n");
        Terminal.getInstance().serPrintln(line+"\r");
      // Zzz(50);
    }
  
    static void Zzz(long time) {
        try {
          Thread.sleep(time);
        } catch(Exception ex) {}
    }


}
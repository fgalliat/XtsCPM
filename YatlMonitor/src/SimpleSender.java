import java.io.*;

public class SimpleSender {

    public static void main(String[] args) throws Exception {
        if ( args == null || args.length != 1 ) {
            System.out.println("Usage : ");
            System.out.println("");
            System.out.println("java Sender READ.ME");
            return;
          }
          process( args[0] );
    }

    static void process(String filename) throws Exception {
        File f = new File(filename);
        String destName = f.getName().toUpperCase();
        if ( destName.length() > 12 ) {
          throw new IllegalArgumentException("not a 8.3 filename !");
        }
        if ( !f.exists() ) {
          throw new IllegalArgumentException("not found filename !");
        }
  
        boolean ok = Terminal.getInstance().reconnect(false);
        if ( !ok ) {
            throw new Exception("Could not connect to YATL !!!!");
        }

        // serialOut = new PrintStream( new FileOutputStream(port) );
  
        printLineToCPM("a:mBASIC c:RTEXT.BAS");
        Zzz(2000);
        printLineToCPM(destName);
        Zzz(1000);
  
        BufferedReader reader = new BufferedReader(new FileReader(f));
        String line;
        while( ( line = reader.readLine() ) != null ) {
          printLineToCPMSlowly(line);
          Zzz(300);
        }
        reader.close();
        printLineToCPM("-EOF-");
        Zzz(1000);
        printLineToCPM("SYSTEM");
        Zzz(1000);
  
        // serialOut.close();

    }
  
    // static PrintStream serialOut;
  
    static void printLineToCPM(String line) throws Exception {
    //   serialOut.print(line+"\r\n");
        Terminal.getInstance().serPrintln(line+"\r");
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
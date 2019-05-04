import java.io.*;

public class SubMCUTester {

    public static void main(String[] args) throws Exception {
        // if ( args == null || args.length != 1 ) {
        //     System.out.println("Usage : ");
        //     System.out.println("");
        //     System.out.println("java SubMCUTester");
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

    static void process() throws Exception {
        String subMCUCommPort = "COM13";
        Terminal.getInstance().setCommPort( subMCUCommPort );
        boolean ok = Terminal.getInstance().reconnect(false);
        if ( !ok ) {
            throw new Exception("Could not connect to YATL LowerLayout MCU !!!!");
        }

        String line = null;
        while( (line = readLine()) != null ) {
          System.out.println("? "+line);
        }

        // write("m"); // launch an MP3

        write("i"); // read System infos
        while( (line = readLine()) != null ) {
          System.out.println("? "+line);
        }

        write("j"); // read Joypad state
        while( (line = readLine()) != null ) {
          System.out.println("? "+line);
        }
        write("k"); // read keyboard buffer
        while( (line = readLine()) != null ) {
          System.out.println("? "+line);
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
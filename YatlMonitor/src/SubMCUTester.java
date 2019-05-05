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

    public static void write(char ch) throws Exception {
      Terminal.getInstance().serWrite(ch);
    }

    // ====================
    static void playMp3(int num) throws Exception {
      char d0 = (char)(num >> 8);
      char d1 = (char)(num % 256);
      write( 'p' ); write( 'p' ); write( d0 );write( d1 );
    }

    static void nextMp3() throws Exception {
      write( 'p' ); write( 'n' );
    }

    static void prevMp3() throws Exception {
      write( 'p' ); write( 'v' );
    }

    static void pauseMp3() throws Exception {
      write( 'p' ); write( 'P' );
    }

    static void stopMp3() throws Exception {
      write( 'p' ); write( 's' );
    }

    static void volumeMp3(int howMany) throws Exception {
      write( 'p' ); write( 'V' ); write( (char)howMany );
    }
    // ======================


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
        // playMp3( 32 );
        // nextMp3();
        pauseMp3();

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
import java.io.BufferedReader;
import java.io.File;
import java.io.FileOutputStream;
import java.io.FileReader;
import java.io.PrintStream;

public class Sender {

  static String port = "/dev/ttyACM0";
  // static String port = "/dev/ttyS15"; // COM16 w/ Cygwin -> but translated as "\\dev\\ttyS15"

  public static void main(String[] args) throws Exception{
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

      serialOut = new PrintStream( new FileOutputStream(port) );

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

      serialOut.close();
  }

  static PrintStream serialOut;

  static void printLineToCPM(String line) {
    serialOut.print(line+"\r\n");
    // System.out.println(">"+line);
  }

  static void printLineToCPMSlowly(String line) {
    // Zzz(50);
    serialOut.print(line+"\r\n");
    // System.out.println("~"+line);
    // Zzz(50);
  }

  static void Zzz(long time) {
      try {
        Thread.sleep(time);
      } catch(Exception ex) {}
  }

}
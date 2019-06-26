import java.io.*;
import java.util.*;
import java.net.*;

public class TelnetSender {

    protected static Socket sk;
    protected static OutputStream sout;

    static void send(String str) throws Exception {
        sout.write( str.getBytes() ); 
        sout.flush(); 
    }

    static void send(byte[] str) throws Exception {
        sout.write( str ); 
        sout.flush(); 
    }

    static void sendln(String str) throws Exception {
        send( str+"\n" );
    }

    static String readUntiEmptyLine(BufferedReader inRead, boolean haltOnStatus) throws Exception {
        String resp;
        while( (resp = inRead.readLine()) != null && !resp.equals("") ) {
            System.out.println(">"+resp);
            if ( haltOnStatus && ( resp.trim().startsWith("+OK") || resp.trim().startsWith("-OK") ) ) {
                return resp;
            }
        }
        return resp;
    }

    static boolean readUntiStatusLine(BufferedReader inRead) throws Exception {
        String resp;
        while( (resp = inRead.readLine()) != null && !resp.startsWith("+OK") ) {
            System.out.println(">"+resp);
            if ( resp.startsWith("-OK") ) {
                return false;
            }
        }
        System.out.println(">"+resp);
        return resp != null;
    }


    public static void main(String[] args) throws Exception {
        Runtime.getRuntime().addShutdownHook( new Thread() {
            public void run() {
                try { sk.getInputStream().close(); } catch(Exception ex) {}
                try { sk.getOutputStream().close(); } catch(Exception ex) {}
                try { sk.close(); } catch(Exception ex) {}
            }
        } );

        // "192.168.1.32";
        String ip = "192.168.4.1";
        String drive = "c";
        String entry = "test.txt";
        byte[] content = ("Hello world ! "+new Date().toString()).getBytes();

        if ( args != null ) {
            if ( args.length > 0 ) { 
                File f = new File(args[0]);
                entry = f.getName(); 
                content = new byte[(int)f.length()];
                FileInputStream fin = new FileInputStream(f);
                fin.read( content, 0, content.length);
                fin.close();
            }
            if ( args.length > 1 ) { drive = args[1]; }
            if ( args.length > 2 ) { ip = args[2]; }
        }

        
        process(ip, drive, entry, content);
    }
    public static void process(String ip, String drive, String entryname, byte[] fileContent) throws Exception {
        sk = new Socket(ip, 23);
        BufferedReader inRead = new BufferedReader(new InputStreamReader(sk.getInputStream()));
        sout = sk.getOutputStream();
        String resp;
        
        System.out.println("Waiting for userPrompt"); 
        while( (resp = inRead.readLine()) != null && !resp.equals("") ) {
            System.out.println(">"+resp);
        }
        char[] tmp = new char[32];
        inRead.read(tmp, 0, 32); // reads UserName : 
        System.out.println("Sending username"); 
        sendln( "root"); Zzz(50);
        inRead.read(tmp, 0, 32); // reads Password : 
        System.out.println("Sending passw"); 
        sendln( "yatl"); Zzz(50);

        System.out.println("Waiting for User Menu"); 
        readUntiEmptyLine(inRead, true);
        Zzz(300);

        System.out.println("Sending control seq."); 
        // sk.getOutputStream().write( ((char)27+"r").getBytes()); sk.getOutputStream().flush();
        send("3");
        Zzz(500);
        
        System.out.println("Waiting Upload prompt.");
        String tmpL;
        boolean ok = false;
        while( (tmpL = readUntiEmptyLine(inRead, true) ) != null ) {
            Zzz(10);
            if ( tmpL.trim().startsWith("+OK") ) {
                ok = true;
                System.out.println("### FOUND OK");
                break;
            }
        }


        // Zzz(500);
        
        ok = ok || readUntiStatusLine(inRead);
        if ( !ok ) { System.out.println("Error waiting upload prompt"); }
        Zzz(100);

        String filepath = "/"+ (drive.toUpperCase()) +"/0/"+(entryname.toUpperCase());
        System.out.println("Sending file path => "+filepath);
        sendln(""+filepath);
        ok = readUntiStatusLine(inRead);
        if ( !ok ) { System.out.println("Error sending file name"); }
        Zzz(100);

        long fileSize = fileContent.length;
        System.out.println("Sending file size => "+fileSize);
        sendln(""+fileSize);
        ok = readUntiStatusLine(inRead);
        if ( !ok ) { System.out.println("Error sending file size"); }
        Zzz(200);

        System.out.println("Sending file content"); 
        send( fileContent );
        ok = readUntiStatusLine(inRead);
        if ( !ok ) { System.out.println("Error sending file content"); }

        // System.out.println("Press Enter...");
        // new BufferedReader(new InputStreamReader(System.in)).readLine();
        Zzz(400);

        System.out.println("Waiting for userPrompt"); 
        while( (resp = inRead.readLine()) != null && !resp.equals("") ) {
            System.out.println(">"+resp);
        }

        // System.out.println("Sending closing seq."); 
        // send( ((char)27+"m"));
        System.out.println("closing conn."); 
        send( "y");

        System.out.println("closing Socket"); 
        try { sk.getOutputStream().close(); } catch(Exception ex) {}
        try { sk.getInputStream().close(); } catch(Exception ex) {}

        sk.close();
    }

    static void Zzz(long time) {
        try { Thread.sleep(time); }
        catch(Exception ex) {}
    }

}

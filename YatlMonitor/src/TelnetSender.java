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

    static void sendln(String str) throws Exception {
        send( str+"\n" );
    }


    public static void main(String[] args) throws Exception {
        Runtime.getRuntime().addShutdownHook( new Thread() {
            public void run() {
                try { sk.getInputStream().close(); } catch(Exception ex) {}
                try { sk.getOutputStream().close(); } catch(Exception ex) {}
                try { sk.close(); } catch(Exception ex) {}
            }
        } );

        // String ip = "192.168.1.32";
        String ip = "192.168.4.1";
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
        sendln( "root");
        inRead.read(tmp, 0, 32); // reads Password : 
        System.out.println("Sending passw"); 
        sendln( "yatl");

        System.out.println("Waiting for User Menu"); 
        while( (resp = inRead.readLine()) != null && !resp.equals("") ) {
            System.out.println(">"+resp);
        }
        Zzz(300);

        System.out.println("Sending control seq."); 
        // sk.getOutputStream().write( ((char)27+"r").getBytes()); sk.getOutputStream().flush();
        send("3");
        Zzz(500);
        
        System.out.println("Waiting Upload prompt.");
        while( (resp = inRead.readLine()) != null && !resp.equals("") ) {
            System.out.println(">"+resp);
        }

        // Zzz(500);
        
        while( (resp = inRead.readLine()) != null && !resp.startsWith("+OK") ) {
        }
        Zzz(100);

        System.out.println("Sending file path"); 
        String filepath = "/C/0/TEST.TXT";
        sendln(""+filepath);
        while( (resp = inRead.readLine()) != null && !resp.startsWith("+OK") ) {
        }
        Zzz(100);

        System.out.println("Sending file size"); 
        long fileSize = 4;
        sendln(""+fileSize);
        while( (resp = inRead.readLine()) != null && !resp.startsWith("+OK") ) {
        }
        Zzz(200);

        System.out.println("Sending file content"); 
        send( "AbCd" );
        while( (resp = inRead.readLine()) != null && !resp.startsWith("+OK") ) {
        }

        // System.out.println("Press Enter...");
        // new BufferedReader(new InputStreamReader(System.in)).readLine();
        Zzz(400);

        System.out.println("Sending closing seq."); 
        send( ((char)27+"m"));
        System.out.println("closing conn."); 
        send( "y");

        try { sk.getOutputStream().close(); } catch(Exception ex) {}
        try { sk.getInputStream().close(); } catch(Exception ex) {}

        sk.close();
    }

    static void Zzz(long time) {
        try { Thread.sleep(time); }
        catch(Exception ex) {}
    }

}

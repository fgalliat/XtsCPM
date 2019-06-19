import java.io.*;
import java.util.*;
import java.net.*;

public class TelnetSender {

    public static void main(String[] args) throws Exception {
        // String ip = "192.168.1.32";
        String ip = "192.168.4.1";
        Socket sk = new Socket(ip, 23);
        BufferedReader inRead = new BufferedReader(new InputStreamReader(sk.getInputStream()));
        String resp;
        
        System.out.println("Waiting for userPrompt"); 
        while( (resp = inRead.readLine()) != null && !resp.equals("") ) {
            System.out.println(">"+resp);
        }
        char[] tmp = new char[32];
        inRead.read(tmp, 0, 32); // reads UserName : 
        System.out.println("Sending username"); 
        sk.getOutputStream().write( "root\n".getBytes()); sk.getOutputStream().flush();
        // while( (resp = inRead.readLine()) != null ) {
        //     System.out.println(resp);
        // }
        inRead.read(tmp, 0, 32); // reads Password : 
        System.out.println("Sending passw"); 
        sk.getOutputStream().write( "yatl\n".getBytes()); sk.getOutputStream().flush();
        // while( (resp = inRead.readLine()) != null ) {
        //     System.out.println(resp);
        // }

        System.out.println("Waiting for User Menu"); 
        while( (resp = inRead.readLine()) != null && !resp.equals("") ) {
            System.out.println(">"+resp);
        }

        // telnetd waits for a READY char
        sk.getOutputStream().write( ("!").getBytes()); sk.getOutputStream().flush();

        System.out.println("Sending control seq."); 
        // sk.getOutputStream().write( ((char)27+"r").getBytes()); sk.getOutputStream().flush();
        sk.getOutputStream().write( ("3").getBytes()); sk.getOutputStream().flush();
        
        // while( inRead.available() > 0) {inRead.read();} // reads ??? 
        Zzz(500);

        while( (resp = inRead.readLine()) != null && !resp.equals("") ) {
            System.out.println(">"+resp);
        }

        // Zzz(500);
        
        System.out.println("Sending file path"); 
        String filepath = "/C/0/TEST.TXT\n";
        sk.getOutputStream().write( (filepath).getBytes()); sk.getOutputStream().flush();
        while( (resp = inRead.readLine()) != null && !resp.startsWith("+OK") ) {
        }
        System.out.println("Sending file size"); 
        long fileSize = 4;
        sk.getOutputStream().write( (""+fileSize+"\n").getBytes()); sk.getOutputStream().flush();
        while( (resp = inRead.readLine()) != null && !resp.startsWith("+OK") ) {
        }

        System.out.println("Sending file content"); 
        sk.getOutputStream().write( ("AbCd").getBytes()); sk.getOutputStream().flush();
        while( (resp = inRead.readLine()) != null && !resp.startsWith("+OK") ) {
        }

        System.out.println("Press Enter...");
        new BufferedReader(new InputStreamReader(System.in)).readLine();

        sk.close();
    }

    static void Zzz(long time) {
        try { Thread.sleep(time); }
        catch(Exception ex) {}
    }

}

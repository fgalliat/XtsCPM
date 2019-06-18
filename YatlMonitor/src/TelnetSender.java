import java.io.*;
import java.util.*;
import java.net.*;

public class TelnetSender {

    public static void main(String[] args) throws Exception {
        // String ip = "192.168.1.32";
        String ip = "192.168.4.1";
        Socket sk = new Socket(ip, 23);

        sk.getOutputStream().write( ((char)27+"r").getBytes());
        BufferedReader inRead = new BufferedReader(new InputStreamReader(sk.getInputStream()));
        String resp;
        while( (resp = inRead.readLine()) != null && !resp.startsWith("+OK") ) {
            // ....
        }
        
        String filepath = "/C/0/TEST.TXT";
        sk.getOutputStream().write( (filepath).getBytes()); sk.getOutputStream().fush();
        while( (resp = inRead.readLine()) != null && !resp.startsWith("+OK") ) {
        }
        long fileSize = 4;
        sk.getOutputStream().write( (""+fileSize).getBytes()); sk.getOutputStream().fush();
        while( (resp = inRead.readLine()) != null && !resp.startsWith("+OK") ) {
        }

        sk.getOutputStream().write( ("AbCd").getBytes()); sk.getOutputStream().fush();

        System.out.println("Press Enter...");
        new BufferedReader(new InputStreamReader(System.in)).readLine();

        sk.close();
    }


}

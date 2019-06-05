import java.io.*;
import java.util.*;
import java.net.*;

public class TelnetTest {

    public static void main(String[] args) throws Exception {
        String ip = "192.168.1.32";
        Socket sk = new Socket(ip, 23);

        sk.getOutputStream().write("abc123".getBytes());

        System.out.println("Press Enter...");
        new BufferedReader(new InputStreamReader(System.in)).readLine();

        sk.close();
    }


}

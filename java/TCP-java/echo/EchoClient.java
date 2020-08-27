import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.net.Socket;
import java.net.UnknownHostException;
import java.util.Scanner;

public class EchoClient {
    static Socket clientSock = null;
	static Scanner scan = new Scanner(System.in);

	static final int BUFFER_SIZE = 512;
	static byte[] sendData = new byte[BUFFER_SIZE];
	static byte[] receiveData = new byte[BUFFER_SIZE];

	public static void init(String ip, int port) {
        try {
        	clientSock = new Socket(ip, port);
        } catch (UnknownHostException e) {
            System.err.println("Don't know about host: " + ip);
            System.exit(1);
        } catch (IOException e) {
            System.err.println("Couldn't get I/O for " + "the connection to: " + ip);
            System.exit(1);
        }
	}
	   
    public static void send(String s) {
        if (clientSock != null) {
	          Utility.paddingBytesArray(sendData, BUFFER_SIZE, s.getBytes(), s.length());
	          try {
	        	  OutputStream os = clientSock.getOutputStream();
	              os.write(sendData, 0, BUFFER_SIZE);
	          } catch (IOException e) {
	              System.out.println ("Connection Terminated");
	          }	
        }
    }

    public static String receive() {
    	String data = null;
        if (clientSock != null) {
        	int nread = 0;
        	int nbytes = BUFFER_SIZE;
            try {
                InputStream is = clientSock.getInputStream();
                int readBytes = 0;
                while (nbytes > 0) {
                    readBytes = is.read(receiveData, nread, nbytes);
                    nread += readBytes;
                    nbytes -= readBytes;
                }
            }
            catch (IOException e) {
            	System.out.println("connection terminated.");
            }          
    		data = Utility.getReceivedString(receiveData);
	    }
        return data;
    }
    
    public static void main(String[] args) throws IOException {

        String destIP = "127.0.0.1";
        int destPort = 9000;
    	if (args.length >= 1) destIP = args[0];
    	if (args.length >= 2) destPort = Integer.parseInt(args[1]);
        System.out.println ("Attemping to connect to host " + destIP + " on port " + destPort);

        init(destIP, destPort);
	    String text = "";
	    while (text.compareTo("EXIT") != 0) {
		    System.out.print("\t\tType your text: ");
		    text = scan.nextLine();
		    send(text);
		    String s = receive();
		    System.out.println("\t\tCLIENT received: " + s);
	    }
	    
		clientSock.close();
		
	    /*        
    	PrintWriter out = null;
        BufferedReader in = null;
        try {
			out = new PrintWriter(clientSock.getOutputStream(), true);
			in = new BufferedReader(new InputStreamReader(clientSock.getInputStream()));
		} catch (IOException e1) {
			e1.printStackTrace();
		}

        BufferedReader stdIn = new BufferedReader(new InputStreamReader(System.in));
		String input = "";
	
	    System.out.print ("input: ");
		try {
			while ((input = stdIn.readLine()) != null) {
			    if (input.equals("EXIT")) break; 
			    out.println(input);
			    System.out.println("received: " + in.readLine());
			    System.out.print ("input: ");
			}
			in.close();
			out.close();
			stdIn.close();
			clientSock.close();
		} catch (IOException e) {
			e.printStackTrace();
		}
*/
    }
}

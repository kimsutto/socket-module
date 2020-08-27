import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.net.ServerSocket;
import java.net.Socket;
import java.util.Scanner;

public class EchoServer {
	static final int BUFFER_SIZE = 512;
	static byte[] sendData = new byte[BUFFER_SIZE];
	static byte[] receiveData = new byte[BUFFER_SIZE];

	static Scanner scan = new Scanner(System.in);
	
	static ServerSocket serverSock = null;
	static Socket clientSock = null;
	
	public static void init(int port) {
		try { 
			serverSock = new ServerSocket(port); // create, bind, listen
			System.out.println ("Waiting for connection on port: " + port);
	    } catch (IOException e) { 
	    	System.err.println("Could not listen on port: " + port); 
			System.exit(1);;
	    }
	}

	public static Socket checkForNewConnections() {
		Socket sock = null;
		try {
			if (serverSock != null) {
				sock = serverSock.accept();
				if (sock != null) {
					System.out.println ("Connection successful");
					return sock;
				}
				else return null;
			}
		} catch (IOException e) {
	         System.err.println("Accept failed."); 
	         return null; 
		}
		return sock; 
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
            	return null;
            }          
    		data = Utility.getReceivedString(receiveData);
	    }
        return data;
    }
    
	public static void main(String[] args) throws IOException { 
		int port = 9000;
     	if (args.length >= 1) port = Integer.parseInt(args[0]);

     	init(port);
		clientSock = checkForNewConnections();
   	    while (true) {
   	    	String s = receive();
   	    	if (s == null) break;
			System.out.println("SERVER: " + s);
		    send("Received: " + s);
		    try {
				Thread.sleep(1000);
			} catch (InterruptedException e) {
				e.printStackTrace();
			}
        }
		clientSock.close(); 
	    serverSock.close();
    	System.out.println("done..");
	} 
}

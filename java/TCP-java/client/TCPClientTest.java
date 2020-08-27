import java.util.Scanner;

public class TCPClientTest {
    static Scanner scan = new Scanner(System.in);

    static final int BUFFER_SIZE = 512;
    byte[] sendData = new byte[BUFFER_SIZE];
    byte[] receiveData = new byte[BUFFER_SIZE];    
    TCPClient client = null;
    
    public TCPClientTest(String ip, int port) {
        client = new TCPClient(ip, port);
    }

    public void send(String s) {
    	if (client != null) {
    		Utility.paddingBytesArray(sendData, BUFFER_SIZE, s.getBytes(), s.length());
    		client.write(sendData, BUFFER_SIZE, true);	
        }
    }

    public String receive() {
    	String data = null;
        if (client != null) {
        	int nread = client.read(receiveData, BUFFER_SIZE, true);
	    	if (nread > 0) {
	    		data = Utility.getReceivedString(receiveData);
	    	} 
	    	else if (nread == TCPClient.CONNECTION_TERMINATED) {
	    		return null;
	    	}
	    }
	    else {
		    System.out.println("TCPClient is null");
	    }
        return data;
    }

    public static void main(String[] args) {
    	String ip = "127.0.0.1";
    	int port = 9000;
    	if (args.length >= 1) ip = args[0];
    	if (args.length >= 2) port = Integer.parseInt(args[1]);

        TCPClientTest t = new TCPClientTest(ip, port);
        
        String text = "";
        String s = "";
	    while (s != null && text.compareTo("bye") != 0) {
		    System.out.print("\t\tType your text: ");
		    text = scan.nextLine(); // IO blocking
		    t.send(text); // TCP write
		    s = t.receive(); // TCP read
		    System.out.println("\t\tTCP CLIENT RECEIVED: " + s);
	    }
   	    System.out.println("TCP CLIENT: DONE");
    }
}

import java.util.Scanner;
 
public class UDPClientTest {
    static Scanner scan = new Scanner(System.in);

    static final int BUFFER_SIZE = 512;
	byte[] sendData = new byte[BUFFER_SIZE];
    byte[] receiveData = new byte[BUFFER_SIZE];
	
	UDPSocket client = null;

	UDPClientTest(String dest, int port) {
        client = new UDPSocket(dest, port);
	}

    public void send(String s) {
        if (client != null) {
	        Utility.paddingBytesArray(sendData, BUFFER_SIZE, s.getBytes(), s.length());
            client.send(sendData, BUFFER_SIZE);
	    }
    }
 
    public String receive() {
    	String data = null;
        if (client != null) {
        	receiveData = client.receive(BUFFER_SIZE);
        	if (receiveData != null) {
            	data = Utility.getReceivedString(receiveData);
        	}
        	else {
    		    System.out.println("receiveData is null");
	    	}
        } 
        else {
        	System.out.println("UDPSocket Client is null");   	
        }
        return data;
    }

    public static void main(String[] args) {
    	String ip = "127.0.0.1";
    	int port = 9002;
    	if (args.length >= 1) ip = args[0];
    	if (args.length >= 2) port = Integer.parseInt(args[1]);
    	
        UDPClientTest t = new UDPClientTest(ip, port);		
	    String text = "";
	    while (text.compareTo("bye") != 0) {
		    System.out.print("\t\tType your text: ");
		    text = scan.nextLine();
		    t.send(text);
            String s = t.receive();
		    System.out.println("\t\tUDP CLIENT RECEIVED: " + s);
	    }
   	    System.out.println("UDP CLIENT: DONE");
    }
}
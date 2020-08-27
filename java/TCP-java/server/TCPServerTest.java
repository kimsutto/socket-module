public class TCPServerTest {
    public static final int BUFFER_SIZE = 512;
    byte[] sendData = new byte[BUFFER_SIZE];
    byte[] receiveData = new byte[BUFFER_SIZE];
    TCPServer server = null;
    TCPClient client = null;
  
    public TCPServerTest(int port) {
        server = new TCPServer(port);
		System.out.println("Server port " + port);
    }
  
    public void listen() {
	    System.out.println("Server listen ..");
	    client = server.checkForNewConnections();
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
		    System.out.println("Client is null");
	    }
	    return data;
    }

    public static void main(String[] args) {
       	int port = 9000;
     	if (args.length >= 1) port = Integer.parseInt(args[0]);

	    TCPServerTest t = new TCPServerTest(port);
	    t.listen();
   	    while (true) {
   	    	String s = t.receive(); // TCP read
   	    	if (s == null) break;
			System.out.println("TCP SERVER: " + s);
		    t.send("Server Received: " + s); // TCP write
        }
   	    System.out.println("TCP SERVER: DONE");
    }
}

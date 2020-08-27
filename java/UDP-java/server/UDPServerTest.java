 
public class UDPServerTest {
    static final int BUFFER_SIZE = 512;
    byte[] sendData = new byte[BUFFER_SIZE];
    
    UDPSocket server = null;
    
    public UDPServerTest(int port) {
        server = new UDPSocket(port);
    }
    
    public void send(String s) {
        if (server != null) {
	        Utility.paddingBytesArray(sendData, BUFFER_SIZE, s.getBytes(), s.length());
            server.send(sendData, BUFFER_SIZE);
	    }
    }
    
    public String receive() {
        if (server != null) {
        	byte[] receiveData = server.receive(BUFFER_SIZE);
        	String data = Utility.getReceivedString(receiveData);
        	return data;
        }
        return null;
    }
 
    public void copyReceiveAddressToSendAddress() {
        server.copyReceiveAddressToSendAddress();
    }
    
    public static void main(String[] args){
    	int port = 9002;
    	if (args.length >= 1) port = Integer.parseInt(args[0]);

        UDPServerTest t = new UDPServerTest(port);      
        while (true) {
            String s = t.receive();
   	    	if (s == null) break;
            System.out.println("UDP SERVER: " + s);
            t.copyReceiveAddressToSendAddress();
            t.send("Received: " + s);
        }
   	    System.out.println("UDP SERVER: DONE");
    }
} 

public class ThreadedTCPClient implements Runnable {
    public static final int BUFFER_SIZE = 512;
    TCPClient client = null;
    byte[] sendData = new byte[BUFFER_SIZE];
    byte[] receiveData = new byte[BUFFER_SIZE];
    
	public ThreadedTCPClient(TCPClient c) {
		client = c;
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
        }
	    else {
		    System.out.println("TCPClient is null");
	    }
	    return data;
    }

	@Override
    public void run() {
        System.out.println("ThreadedTCPClient.run()");
   	    while (true) {
			String s = receive();
			System.out.println(s);
			send("Received: " + s); // 1-to-1 send
        }  
    }
}

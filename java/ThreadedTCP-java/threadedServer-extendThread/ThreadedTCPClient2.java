import java.net.Socket;
import java.net.SocketException;
import java.net.UnknownHostException;
import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;

public class ThreadedTCPClient2 extends Thread 
{
    public static final int BUFFER_SIZE = 512;
    TCPClient client = null;
    byte[] sendData = new byte[BUFFER_SIZE];
    byte[] receiveData = new byte[BUFFER_SIZE];
  
    ThreadedTCPClient2(TCPClient c) {
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

    public void run() {
        System.out.println("ThreadedTCPClient2.run()");
   	    while (true) {
			String s = receive();
			System.out.println(s);
		    send("Received: " + s);
        }       
    }
}

import java.io.IOException;
import java.net.ServerSocket;
import java.net.Socket;

public class TCPServer {	
	final static int OK = 1;
	final static int FAILED = 0;
	protected ServerSocket serverSocket = null;

	public TCPServer(int port) {
		init(port);
	}

	public int init(int port) {
		try {
			serverSocket = new ServerSocket(port);
			if (serverSocket != null) return OK;
			else return FAILED;
		}
		catch (IOException e) {
			return FAILED;
		}
	}
	
	public TCPClient checkForNewConnections() {
		try {
			if (serverSocket != null) {
				Socket s = serverSocket.accept();
				if (s != null) 
                    return new TCPClient(s); 
                return null;
			}
		}
		catch (IOException e) {
			return null;
		}
		
		return null;
	}
}

public class ThreadedTCPServerTest {
	TCPServer server = null; // server
	static int port = 9000;
	public static final int MAX_NUM_CLIENTS = 5; // clients
    int count = 0;
    
    public ThreadedTCPServerTest(int port) {
        server = new TCPServer(port);
    }
    
    // return ThreadedTCPClient (Runnable)
    public ThreadedTCPClient listen() {
	    System.out.println("Server listen at port# " + port);
	    TCPClient c = server.checkForNewConnections();
	    count++;
	    return new ThreadedTCPClient(c);
    }
    
    public void run() {
        while (true) {
            if (count < MAX_NUM_CLIENTS) {
            	ThreadedTCPClient c = listen();
                new Thread(c).start();
            }
        }
    }
    
    public static void main(String[] args) {
    	if (args.length >= 1) port = Integer.parseInt(args[0]);

    	ThreadedTCPServerTest s = new ThreadedTCPServerTest(port);
    	s.run();
    }
}
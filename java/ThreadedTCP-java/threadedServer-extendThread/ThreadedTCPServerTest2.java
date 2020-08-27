public class ThreadedTCPServerTest2 {
	TCPServer server = null; // server
	static int port = 9000;
	public static final int MAX_NUM_CLIENTS = 5; // clients
    int count = 0;
    
    ThreadedTCPServerTest2(int port) {
        server = new TCPServer(port);
    }
  
    public ThreadedTCPClient2 listen() {
	    System.out.println("Server listen at port# " + port);
	    TCPClient c = server.checkForNewConnections();
		count++;
        return new ThreadedTCPClient2(c);
    }
  
    public void run() {
        while (true) {
            if (count < MAX_NUM_CLIENTS) {
				ThreadedTCPClient2 t = listen();
            	t.start();
                count++;
            }
        }
    }
    
    public static void main(String[] args) {
		if (args.length >= 1) port = Integer.parseInt(args[0]);
    	ThreadedTCPServerTest2 s = new ThreadedTCPServerTest2(port);
    	System.out.println("new threaded tcp server test 2");
        s.run();          
    }
}
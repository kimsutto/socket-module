import java.util.concurrent.ConcurrentHashMap;

public class ThreadedTCPClient implements Runnable {
    public String find(ConcurrentHashMap<String,Woeid>hashmap, String city) {
        Woeid ww = hashmap.get(city);
        String ss = ww.toString();
        return ss;
    }
    public static final int BUFFER_SIZE = 5000;
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
            String word = s.split(" ")[0];
            
            if(word.equals("Insert")) {
            	word = s.replace("Insert ","");
            	Woeid ww = Woeid.getData(word); 
            	ThreadedTCPServerTest.hashmap.put(ww.getCity(), ww);
            	
            } else if(word.equals("Query")) {
            	String city = s.split(" ")[1];
            	s =find(ThreadedTCPServerTest.hashmap, city);
            	
            } else if(word.equals("Print")) {
            	s = ThreadedTCPServerTest.hashmap.toString();
            	if (s == "") {
            		s = "nothing";
            	}
                

            }
            System.out.println(s);
            send("Received: " + s); // 1-to-1 send
            } 
        }  
    }

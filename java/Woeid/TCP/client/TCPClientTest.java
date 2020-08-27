import java.util.List;
import java.util.Scanner;

public class TCPClientTest {
    static Scanner scan = new Scanner(System.in);

    static final int BUFFER_SIZE = 5000;
    byte[] sendData = new byte[BUFFER_SIZE];
    byte[] receiveData = new byte[BUFFER_SIZE];    
    TCPClient client = null;
    static List<Woeid> list = null;
    
    TCPClientTest(String ip, int port, String filename) {
        client = new TCPClient(ip, port); 
        list = WoeidImporter.loadCSV(filename);
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
        return data;
    }

    public static void main(String[] args) {
    	String ip = "127.0.0.1";
    	int port = 9000;
        String filename = args[0];

        TCPClientTest t = new TCPClientTest(ip, port,filename);

        int i = 0; 
        String text = "";
        while(i < list.size()) {
	       text = "Insert " + list.get(i).getID()+ "," + list.get(i).getCity()+","+list.get(i).getCountry() 
           + ","+ list.get(i).getLatitude() + "," + list.get(i).getLongitude() ; 
           t.send(text);
           String s = t.receive();
           i++;
           System.out.println("\t\tTCP CLIENT RECEIVED: " + s); 
        }
	    while (text.compareTo("bye") != 0) { 
	    	System.out.print("\t\tMessage: ");
		    text = scan.nextLine(); 
		    t.send(text); 
		    String s = t.receive();
		    System.out.println("\t\tTCP CLIENT RECEIVED: " + s); 
	    }
   	    System.out.println("TCP CLIENT: DONE");
    }
}

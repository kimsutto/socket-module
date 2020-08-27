import java.util.Scanner;

public class TCPClientTest {
    static Scanner scan = new Scanner(System.in);

    static final int BUFFER_SIZE = 512;
    byte[] sendData = new byte[BUFFER_SIZE];
    byte[] receiveData = new byte[BUFFER_SIZE];    
    TCPClient client = null;
    
    TCPClientTest(String ip, int port) { //ip와 port번호를 매개변수로 받는 생성자 
        client = new TCPClient(ip, port); //TCPClient타입 client 소켓 생성
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
        if (args.length != 1)
            //예외처리 
        String filename = args[0];

        TCPClientTest t = new TCPClientTest(ip, port);
	    String text = ""; //text변수 선언 및 초기화 
	    while (text.compareTo("bye") != 0) { //text가 bye일때까지 계속 
		    System.out.print("\t\tMessage: ");
		    text = scan.nextLine(); //입력을 받음 
		    t.send(text); // 입력을 서버로 send
		    String s = t.receive(); //서버에서 받은 값을 String s에 넣음
		    System.out.println("\t\tTCP CLIENT RECEIVED: " + s); // 받은 값 출력 
	    }
   	    System.out.println("TCP CLIENT: DONE");
    }
}

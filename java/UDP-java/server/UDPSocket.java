import java.io.IOException;
import java.net.DatagramPacket;
import java.net.DatagramSocket;
import java.net.InetAddress;
import java.net.InetSocketAddress;
import java.net.SocketException;
import java.net.UnknownHostException;

public class UDPSocket {
    final static int OK = 1;
    final static int FAILED = 2;

    DatagramSocket socket = null;
    InetAddress destAddr = null;
    int destPort = 0;
    InetAddress receivedAddr = null;
    int receivedPort = 0;

    public UDPSocket() {
        try {
        	socket = new DatagramSocket();
        }
        catch (SocketException e) {
            System.out.println("UDPSocket: Creating a udp socket failed.\n"); 
        }
    }
    
    public UDPSocket(int port) {
        try {
        	socket = new DatagramSocket(port);
        }
        catch (SocketException e) {
            System.out.println("UDPSocket: Creating a udp socket failed.\n"); 
        }
    }
          
    public UDPSocket(InetAddress dest, int port) {        
        try {
        	socket = new DatagramSocket();
            setSendAddress(dest, port);
        }
        catch (SocketException e) {
            System.out.println("UDPSocket: Creating a udp socket failed.\n"); 
        }
    }
    
    public UDPSocket(String ip, int port) {
        try {
        	socket = new DatagramSocket();
            setSendAddress(ip, port);
        }
        catch (SocketException e) {
            System.out.println("UDPSocket: Creating a udp socket failed.\n"); 
        }
    }
    
    public void close() {
        if (socket != null) {
            socket.close();
            socket = null;
        }
    }
    
    public void setSendAddress(String ip, int port) {
        try {
            destAddr = InetAddress.getByName(ip);
        }
        catch (UnknownHostException e) {
        }
        destPort = port;        
    }

    public void setSendAddress(InetAddress dest, int port) {
        destAddr = dest;
        destPort = port;        
    }

    public void copyReceiveAddressToSendAddress() {
        setSendAddress(receivedAddr, receivedPort);
    }
       
    public int send(byte[] buffer, int size) {
        if (socket != null) {
            DatagramPacket dp = new DatagramPacket(buffer, size, destAddr, destPort);
        
            if (dp != null) {
                try {
                    socket.send(dp);
                }
                catch (IOException e) {
                    System.out.println("UDPSocket.send: Error occurred while sending a datagram packet.\n");                    
                    return 0;
                }
            }
        }
        return size;
    }
      				
    public byte[] receive(int size) {
        if (size > 0) {
            byte[] buffer = new byte[size];
            
            if (buffer != null) {
                DatagramPacket dp = new DatagramPacket(buffer, size);
                
                try {
                   receivedAddr = null;
                   receivedPort = 0;
                   socket.receive(dp);
                   receivedAddr = dp.getAddress();
                   receivedPort = dp.getPort();
                }
                catch (IOException e) {
                    System.out.println("UDPSocket.receive: Error occurred while receiving a datagram packet.\n");
                    return null;
                }
                return buffer;
            }
        }
        return null;
    }
}
    
    
                

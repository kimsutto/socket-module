import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.net.Socket;
import java.net.SocketException;
import java.net.UnknownHostException;

public class TCPClient {
    public static final int NO_MORE_DATA = -1;
    public static final int TIME_OUT = -2;
    public static final int CONNECTION_TERMINATED = -3;
    public static final int SOCKET_EXCEPTION = -4;
    public static final int OK = 1;
    public static final int FAILED = 2;

    protected int timeoutPeriod;
    protected Socket socket = null;

    public TCPClient() {
    }
    
    public TCPClient(Socket s) {
        socket = s;
    }

    public TCPClient(String ip, int port) {
    	connectToServer(ip, port);
    }

    public void close() {
        try {
            if (socket != null) {
                socket.close();
                socket = null;
            }
        }
        catch (IOException e) {
            System.out.println("TCPSocket.close: couldn't close the socket.");
        }
    }
    
    public int connectToServer(String ip, int port) {
        try {
            socket = new Socket(ip, port);
            return OK;
        }
        catch (UnknownHostException e) {
            System.out.println("Cannot resolve the destination host name: " + ip + ".");
            socket = null;
        }
        catch (IOException e) {
            System.out.println("I/O error occurred while creating a socket connect.");
            socket = null;
        }
        catch (SecurityException e) {
            System.out.println("Security manager doesn't allow making connection.");
            socket = null;
        }
        return FAILED;
    }

    public void setTimeoutPeriod(int timeout) {
        timeoutPeriod = timeout;
    }
    
    public int read(byte[] buffer, int nbytes, boolean blocking) {
        if (socket != null) {
            int nread = 0;
            int prevTimeout;
            try {
                prevTimeout = socket.getSoTimeout();
                if (blocking) socket.setSoTimeout(timeoutPeriod);
                else socket.setSoTimeout(0);
            }
            catch (SocketException e) {
            	//System.out.println("setting timeout failed.");
                return SOCKET_EXCEPTION;
            }

            try {
                InputStream is = socket.getInputStream(); // read
                int readBytes = 0;     
                while (nbytes > 0) {
                    readBytes = is.read(buffer, nread, nbytes); // read
                    nread += readBytes;
                    nbytes -= readBytes;
                	//System.out.println("readBytes=" + readBytes + " nread=" + nread + " nbytes=" + nbytes);
                }
            }
            catch (IOException e) {
            	System.out.println("connection terminated.");
                return CONNECTION_TERMINATED;
            }
    /*		catch (InterruptedIOException e) {
                return TIME_OUT;
            }*/
            finally {
                try {
                    socket.setSoTimeout(prevTimeout);
                }
                catch (SocketException e) {
                    return SOCKET_EXCEPTION;
                }
            }
            return nread;
        }
        return CONNECTION_TERMINATED;
    }

    public int write(byte[] buffer, int nbytes, boolean blocking) {
        if (socket != null) {
            int prevTimeout;
            try {
                prevTimeout = socket.getSoTimeout();
                if (blocking) socket.setSoTimeout(timeoutPeriod);
                else socket.setSoTimeout(0);
            }
            catch (SocketException e) {
                return SOCKET_EXCEPTION;
            }

            try {
                OutputStream os = socket.getOutputStream(); // write
                os.write(buffer, 0, nbytes); // write
            }
            catch (IOException e) {
                return CONNECTION_TERMINATED;
            }
    /*		catch (InterruptedIOException e) {
                return TIME_OUT;
            }*/
            finally {
                try {
                    socket.setSoTimeout(prevTimeout);
                }
                catch (SocketException e) {
                    return SOCKET_EXCEPTION;
                }
            }

            return nbytes;
        }
        return CONNECTION_TERMINATED;
    }
}

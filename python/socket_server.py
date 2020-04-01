import socket 
from _thread import *

HOST = '0.0.0.0'
PORT = 8800


def threaded(client_socket, addr): 

    print('Connected by :', addr[0]) 
    
    while True: 
        try:
            data = client_socket.recv(1024)

            if not data: 
                print('Disconnected by ' + addr[0])
                break
            print('Received from ' + addr[0] , data.decode('utf-8'))
            break

        except ConnectionResetError as e:
            print('Disconnected by ' + addr[0],':',addr[1])
            break
             
    client_socket.close() 




server_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM) 
server_socket.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
server_socket.bind((HOST, PORT)) 
server_socket.listen() 

print('server start')


while True: 

    print('wait')


    client_socket, addr = server_socket.accept() 
    start_new_thread(threaded, (client_socket, addr)) 

server_socket.close() 

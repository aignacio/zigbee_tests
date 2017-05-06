from socket import *
 
if __name__ == '__main__':
 
    host = 'localhost'
    port = 55567
    buf = 1024
 
    addr = (host, port)
 
    clientsocket = socket(AF_INET, SOCK_STREAM)
 
    clientsocket.connect(addr)
 
    while 1:
        data = raw_input(">> ")
        if not data:
            break
        else:
            clientsocket.send(data)
            data = clientsocket.recv(buf)
            if not data:
                break
            else:
                print data
     
    #clientsocket.close()
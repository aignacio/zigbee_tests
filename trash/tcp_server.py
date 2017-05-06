#!/usr/bin/python
user = 'aignacio'

import sys
sys.path.insert(0, '/home/'+user+'/homestark/MySQL')
import db_control as db
import asyncore
import socket
import threading as th
from time import gmtime, strftime
import time

server_address = "localhost"
server_port = 8080
server_inf = (server_address,server_port)

i = 0
buffer_rc = 8192 #13 bytes
limit = 500 #Limit of devices in the table
value_old = [0 for i in xrange(limit)]
sensors_ip = [0 for i in xrange(limit)]
socket_dev = [0 for i in xrange(limit)]

class Callback_Handler(asyncore.dispatcher_with_send):
    def handle_read(self):
        #print self
        device_ip = self.getpeername()
        data = self.recv(buffer_rc)
        sensor = data.split(':')
        if sensors_ip[int(sensor[2])] == 0:
            print "Recebendo de sensor novo:"
            sensors_ip[int(sensor[2])] = device_ip
            socket_dev[int(sensor[2])] = self
        else:
            print "\nRecebendo de sensor velho:\n"

        if socket_dev[int(sensor[2])] == self:
            print "RECEBI DO SOCKET CORRETO!"
        else:
            print "NAO RECEBI DO SOCKET CORRETO!"
    
        print "\n\rData:"+data
        if data:
            self.send(data)
    
    def handle_error (self):

        exception, value, traceback = sys.exc_info() # get exception info
        if self.intolerant or exception is KeyboardInterrupt: # server is in intolerant mode or manually killed
            #raise # crash!
            socket_dev[2] = 0
            sensors_ip[2] = 0
        elif self.debug: # server is in debug mode
            self.log.exception('caught an exception, closing socket') # log full traceback
        else: # server is in normal operating mode
            while traceback.tb_next: # find position where the exception occurred
                traceback = traceback.tb_next
            self.log.error('caught an exception in %s: %s', traceback.tb_frame.f_code.co_name, value) # log single line traceback

        #self.close() # close socket and remove from asyncore map



    # def handle_expt(self):
    #     print "EXCPET"
    #     self.close() # connection failed, shutdown

    # def shutdown(self):
    #     self.close()
    #     for conn in self.connections:
    #         conn.write("\r\nShutting down...\r\n")
    #         conn.shutdown()

class Create_Server(asyncore.dispatcher):

    def __init__(self, host, port):
        asyncore.dispatcher.__init__(self)
        self.create_socket(socket.AF_INET, socket.SOCK_STREAM)
        self.set_reuse_addr()
        self.bind((host, port))
        self.listen(5)

    def handle_accept(self): 
        pair = self.accept()
        if pair is not None:
            sock, addr = pair
            print 'Sensor incoming from ip:%s' % repr(addr)    
            handler = Callback_Handler(sock)

    def handle_close(self):
        print "Device has exit from network!"
        self.close()
        return
        
############## Print Logo Homestark ################
f = open('images_logos/logo','r')
for line in f:
    sys.stdout.write(line) #Print without new line
f.close()
print "\n\t[TCP Socket Server:%s:%d]\n" % (server_inf) 
####################################################

servers = Create_Server(server_inf[0],server_inf[1])
asyncore.loop()
##############################################################
#										 
#	Project: Home Control - Socket tcp server for sensors  
#	Author: Anderson Ignacio da Silva					 
#	Date: 29/12/14								 
#	Target: Cubieboard_2-A20						 
#	Inf.: http://www.esplatforms.blogspot.com.br 			 
#										 
##############################################################
#!/usr/bin/python
import socket
import sys
import time
from time import gmtime, strftime

# Create a TCP/IP socket
sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
# Bind the socket to the port
server_address = ('localhost', 10000)
print >>sys.stderr, 'starting up on %s port %s' % server_address
sock.bind(server_address)
# Listen for incoming connections
sock.listen(1)

while True:
   	# Wait for a connection
   	print >>sys.stderr, 'waiting for a connection'
    	connection, client_address = sock.accept()
	
	try:
	        print >>sys.stderr, 'connection from', client_address

        	# Receive the data in small chunks and retransmit it
        	while True:
			#data = connection.recv(8192)
			connection.sendall('\n\rHello World:Anderson-'+strftime("%Y-%m-%d %H:%M:%S", gmtime()))
			time.sleep(2) 
           		data = connection.recv(16)
            		print >>sys.stderr, 'received "%s"' % data
            		if data:
               			print >>sys.stderr, 'sending data back to the client'
                		connection.sendall(data)
            		else:
                		print >>sys.stderr, 'no more data from', client_address
                	break  
	except:
		print "ERROR!"
    	#finally:
       		# Clean up the connection
        	#connection.close()

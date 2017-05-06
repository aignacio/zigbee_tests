##############################################################
#															 
#	Project: Home Control - uC Serial to MySQL socket		 
#	Author: Anderson Ignacio da Silva						 
#   Date: 09/10/14											 
#	Target: Cubieboard_2-A20								 
#	Inf.: http://www.esplatforms.blogspot.com.br 			 
#															 
##############################################################
#!/usr/bin/python
 
import serial
import os
import sys
import time 

port_default = '/dev/ttyACM1'
baudrate_default = 115200
name_fout = 'serial_out'

#check_port() - List all avaiable serial ports
def list_ports():
	active_ports = []
	for port  in range(0,10):
		try:
			obj = serial.Serial(port)
	            	active_ports.append((port,obj.portstr)) 
           		obj.close()
		except (OSError, serial.SerialException):
	          	pass
	return active_ports

#wr_port(port,bd,value) - Used to write some value in serial port
def wr_port(port,bd,value):
	try:
   		#value = (raw_input("\nWrite value:"))
   		obj = serial.Serial(port,bd)
   		obj.write(value)
   		obj.close()
   	except serial.SerialException:
       		print"\n@ERROR: Check your connections!"
   	return value

#wr_port_default(value) - Used to write some value in serial port
def wr_str_default(value):
	try:
		#print value
		send = list(value)
		max_l = len(send)
		obj = serial.Serial(port_default,baudrate_default)
		#print "Max it:%d" % max_l
   		for i in range(0,max_l):
   			#print "Sending:%c" % send[i]
   			obj.write(send[i])
			time.sleep(0.001) #Time for the uC proccess the data
   		obj.close()
   	except serial.SerialException:
       		print"\n@ERROR: Check your connections!"
   	return value

#rd_port(port,bd) - Used to read some value in serial port
def rd_port(port,bd):
	try:
		obj = serial.Serial(port,bd)
		value = obj.read()
   	    	obj.close()
   	except serial.SerialException:
       		print"\n@ERROR: Check your connections!"
	return value

#poll_serial() - Used to poll serial and send to a file
def poll_serial():
	try:
		value = 'A'
		f = open(name_fout,'w')
		obj = serial.Serial(port_default,baudrate_default)
		while '@' not in value:
			value = obj.read()
		value = obj.read()
		while '#' not in value:
			f.write(value)
			value =  obj.read()
			#sys.stdout.write(value) #Print out data
		f.close()	
	except:
		print"\n@ERROR: Check your connections!"
	return 0

#poll_serial_cc2530() - Used to poll serial and capture a value from cc2530
def poll_serial_cc2530():
	try:
		value = 'A'
		#f = open("reading_from_cc2530.txt",'w')
		obj = serial.Serial(port_default,baudrate_default)
		while 'V' not in value:
			value = obj.read()
		while ':' not in value:
			value = obj.read()
		value = obj.read()
		#f.write(value)
		#sys.stdout.write(value) #Print out data
		#f.close()	
	except:
		print"\n@ERROR: Check your connections!"
	return value

if __name__ == '__main__':
	poll_serial()
	wr_str_default(value)
	poll_serial_cc2530()
		

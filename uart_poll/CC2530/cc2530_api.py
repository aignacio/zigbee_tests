##############################################################
#															 
#	Project: Home Control - API for communicate with 
#				SoC-CC2530		 
#	Author: Anderson Ignacio da Silva						 
#	Date: 01/12/14											 
#	Target: CC2530								 
#	Inf.: http://www.esplatforms.blogspot.com.br 			 
#	
# 	Commands:
#	Write command:
#	  #   0   0   0   W   0   0   0   0   #
#	[00][01][02][03][04][05][06][07][08][09]
#	Read command:
#	  #   0   0   0   R   0   0   0   X   #
#	[00][01][02][03][04][05][06][07][08][09]
#	Toggle command:
#	  #   0   0   0   T   0   0   0   X   #
#	[00][01][02][03][04][05][06][07][08][09]
# 	Custom command:
#	  #   0   0   0   C   0   0   0   0   #
#	[00][01][02][03][04][05][06][07][08][09]	
#													 
##############################################################
#!/usr/bin/python
import serial_com as com
import serial
import sys
import os

router = '001'
address = '001'
value = '1'
frame = 'ABCD'

def toggle_address(router,address):
	print 'Sending toogle to router ' + router + ' in the address ' + address
	buffer = '#' + router + 'T' + address + 'X#'
	com.wr_str_default('E')
	com.wr_str_default(buffer)
	com.wr_str_default('S')

def read_address(router,address):
	print 'Reading address ' + address 
	buffer = '#' + router + 'R' + address + 'X#'
	com.wr_str_default('E')
	com.wr_str_default(buffer)
	com.wr_str_default('S')
	value = com.poll_serial_cc2530();
	print 'Value read: ' + value

def write_address(router,address,value):
	print 'Sending ' + value + ' to address ' + address + ' in ' + router
	buffer = '#' + router + 'W' + address + value + '#'
	com.wr_str_default('E')
	com.wr_str_default(buffer)
	com.wr_str_default('S')

def send(router,frame):
	print 'Sending ' + frame + ' to router ' + router
	buffer = '#' + router + 'C' + frame + '#'
	com.wr_str_default('E')
	com.wr_str_default(buffer)
	com.wr_str_default('S')

if __name__ == '__main__':
	toggle_address(router,address)
	write_address(router,address,value)
	send(router,frame)
	read_address(router,address)

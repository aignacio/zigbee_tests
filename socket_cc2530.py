##############################################################
#															 
#	Project: Home Control - Main script of polling sensors   
#	Author: Anderson Ignacio da Silva						 
#   Date: 09/10/14											 
#	Target: Cubieboard_2-A20								 
#	Inf.: http://www.esplatforms.blogspot.com.br 			 
#															 
##############################################################

#!/usr/bin/python
user = "cubie"
address_output_default = "001" #this is the output in device, in this casa P0.4 of CC2530
import sys
import os
sys.path.insert(0, '/home/'+user+'/homestark/uart_poll')
import serial_com
sys.path.insert(0, '/home/'+user+'/homestark/MySQL')
import db_control
sys.path.insert(0, '/home/'+user+'/homestark/uart_poll/CC2530')
import cc2530_api as cc
import time

time.sleep(10) #Dummy way to wait mysql start

limit = 100 #Limit of devices in the table

value_old = [0 for i in xrange(limit)]

db_name = "homestark"			#Name of database for the system
tb_name = "sensors"				#Name for the table entry

##############Print Logo Homestark################
f = open('images_logos/logo','r')
for line in f:
    sys.stdout.write(line) #Print without new line
f.close()
print "\n"
##################################################

char = 'A'
while 'z' not in char:
	time.sleep(0.1) #Useful to get cold cubieboard

	# #Getting data from the microcontroller 
	# serial_com.poll_serial()   
	# with open('/home/aignacio/homestark/serial_out','r') as f:
	#     for line in f:
	#     	sensor = line.split(' ')
	#     	if len(sensor) == 5:
	# 	    	if sensor[0] == "Sensor" and sensor[0].isdigit() == False:
	# 		    	if sensor[3].isdigit() == True and sensor[1].isdigit() == True:
	# 			    		vrf = db_control.get_row(db_name,tb_name,int(sensor[1])) #Return a tupple
	# 			    		if vrf != None:
	# 			    			if int(sensor[1]) <= addr_reading:
	# 			    				db_control.update_table(db_name,tb_name,int(sensor[1]),"value",int(sensor[3]))
	# f.close()

	#Sending data to CC2530 
	for i in range(1,limit):
		row = db_control.get_row(db_name,tb_name,i) #Return a tupple and verify if the sensor exist
		if row != None: #if id exist a sensor, send to the coordinator
			if value_old[i] != row[4]:
				value_old[i] = row[4]
				#Prepare address to send
				if row[5] < 10:
					dev_address = '00'+str(row[5])
				elif row[5] < 100:
					dev_address = '0'+str(row[5])
				else:
					dev_address = str(row[5])
				#print "\nDEVICE:"+str(row[7])+"\n"
				if row[2] == 'switch':
					#print "\n\tSwitch:"
					#print "\n\tTag:" + row[1]
					#print "\n\tDevice:" + dev_address
					#print "\n\tValue:" + row[4]
					cc.write_address(dev_address,address_output_default,row[4])	
				elif row[2] == 'light':
					#print "\n\tLight:"
					#print "\n\tTag:" + row[1]
					#print "\n\tDevice:" + dev_address
					#print "\n\tValue:" + row[4]
					if dev_address == "002":
						cc.write_address(dev_address,address_output_default,row[4])
					else:
						cc.write_address(dev_address,'002',row[4])
							
				elif row[2] == 'light_rgb':	
					#print "\n\tLight RGB:"
					#print "\n\tTag:" + row[1]
					#print "\n\tDevice:" + dev_address
					#print "\n\tValue:" + row[4]
					cc.write_address(dev_address,address_output_default,row[4])	
	# dir=" "
	# file = "serial_out"
	# cmd_rm= "rm "+ dir + file
	# os.system(cmd_rm)

	#char = 'z'
	if char == 'z': 
		print 'Exiting.....'	
	

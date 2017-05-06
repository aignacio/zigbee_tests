##############################################################
#                                                            
#   Project: Home Control - TCP Server script to poll sensors    
#   Author: Anderson Ignacio da Silva                        
#   Date: 02/01/15                                           
#   Target: Cubieboard_2-A20                                 
#   Inf.: http://www.esplatforms.blogspot.com.br             
#                                                            
##############################################################
# This script creates a TCP Server and make possible the 
# sensors ESP8266 connect to them to receive and write 
# values from MySQL in database Homestark.
#
# The commands accepted in this Server from devices are:
#
# sensor:device_address:type_device - Add a sensor in 
# socket table
#
# write:column_in_mysql:value_to_write - Write a value
# in a column of MySQL - Homestark.Sensors
#
# Debug:
# To debug how this sockets works, just run as:
#     python tcp_server.py 
# And in other terminal connect with a telnet:
#     telnet server_address server_port
# *Remember to see this address in code below

#!/usr/bin/python
user = "aignacio"

import sys
import os
sys.path.insert(0, '/home/'+user+'/homestark/MySQL')
import db_control as db
from socket import *
import thread
import struct
import socket

limit = 255 #Limit of devices in the table [255-Netmask of ip devices]
server_address = "localhost"
server_port = 8080
server_inf = (server_address,server_port)
buffer_rc = 8192 #buffer for receive frames from sockets (13 bytes)

#List of types of devices allowed to connect
type_device = {
    'light'    :   1,
    'light_rgb':   1,
    'switch'   :   1,
}

value_old = [0 for i in xrange(limit)] #List to control mysql poll of sensors
SensorsSockets = [0 for i in xrange(limit)] #List of sockets from each device

db_name = "homestark"           #Name of database for the system
tb_name = "sensors"             #Name for the table entry

#Add device in network
def addDevice(data,ClientSocket):
    device = int(data[1])
    if not SensorsSockets[device]:
        SensorsSockets[device] = ClientSocket
        print 'Device:%d added in socket table' % device
    else:
        print 'Already has a device with this id:%d' % device
        return 0
    msg = '@OK\n'
    SensorsSockets[device].sendall(msg)
    return device

#Write in MySQL database value from sensors
def writeMySQL(device,data,ClientSocket):
    if not SensorsSockets[device]:
        print 'There is no device registered in this Client Socket!'
    else:
        db.update_table(db_name,tb_name, str(device), data[1], data[2])
        print '[MySQL]Update in %s=%s where device_address = %d' % (data[1],data[2],device)
    msg = '@OK'
    SensorsSockets[device].sendall(msg)

#Used to handle with receive frames from devices
def handle_recv(socket, ClientIP):
    while True:
        try:
            data = socket.recv(buffer_rc)
            if data:
                data = data.split(':')
                if len(data) == 3: #Verify if the syntax of the frame is correct
                    data[2] = data[2].replace('\r\n','')
                    if data[0] == 'sensor' and data[1].isdigit() == True and type_device[data[2]]: #If we want to register new device
                        device = addDevice(data,socket)
                    elif 'device' in locals() and device != 0 and data[0] == 'write': #If we want to write a value of a sensor
                        writeMySQL(device,data,socket)
                    else:
                        print 'Refused command from (%s,%s)' % (ClientIP[0],ClientIP[1])
            else: #Empty data is the result of disconnected device
                if 'device' in locals() and device:
                    print 'Device:%d in address:(%s,%s) disconnect!' % (device,ClientIP[0],ClientIP[1])
                    SensorsSockets[device] = 0
                else:
                    print 'Device:Not Registered in address:(%s,%s) disconnect!' % (ClientIP[0],ClientIP[1])
                thread.exit()
        except error: #Handle errors and disconnect the client
            if 'device' in locals() and device:
                print 'Device:%d in address:(%s,%s) disconnect!' % (device,ClientIP[0],ClientIP[1])
                SensorsSockets[device] = 0
            else:
                print 'Device:Not Registered in address:(%s,%s) disconnect!' % (ClientIP[0],ClientIP[1])
            thread.exit() #Finish the thread to leave processor work

#Poll MySQL to write value in sensors
def MySQLPoll():
    while True:    
        try:
            for i in range(1,limit):
                row = db.get_row(db_name,tb_name,i) #Return a tupple and verify if the sensor exist
                #Tupple row format (id,tag,type,request,value,device_address,location,active,register)
                if row != None:
                    value = row[4]
                    if value_old[i] != value:
                        value_old[i] = value
                        device_address = row[5]
                        type_dev = row[2]
                        if SensorsSockets[device_address] != 0:
                            if type_dev == 'light_rgb':
                                value = struct.unpack('BBB',value.decode('hex')) #Convert from #AABBCC to (R,G,B)
                                #msg = 'value:('+str(value[0])+','+str(value[1])+','+str(value[2])+')\n'
                                if value[0] < 10:
                                    R = '00'+str(value[0])
                                elif value[0] < 100:
                                    R = '0'+str(value[0])
                                else:
                                    R = str(value[0])

                                if value[1] < 10:
                                    G = '00'+str(value[1])
                                elif value[1] < 100:
                                    G = '0'+str(value[1])
                                else:
                                    G = str(value[1])

                                if value[2] < 10:
                                    B = '00'+str(value[2])
                                elif value[2] < 100:
                                    B = '0'+str(value[2])
                                else:
                                    B = str(value[2])

                                msg = 'C'+R+G+B+'F'
                                SensorsSockets[device_address].sendall(msg)
                                print '[MySQL]Send value=%s to device_address=%d' % (value,device_address)
                            else:
                                msg = 'value:'+value+'\n'
                                SensorsSockets[device_address].sendall(msg)
                                print '[MySQL]Send value=%s to device_address=%d' % (value,device_address)
        except error:
            print "[MySQL] ERROR DATABASE"

if __name__ == "__main__":
 
    addr = (server_address, server_port)

    serversocket = socket.socket(AF_INET, SOCK_STREAM)
    serversocket.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
    #serversocket.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
    serversocket.bind(addr)
    serversocket.listen(5)
    
    ##############Print Logo Homestark################
    f = open('images_logos/logo','r')
    for line in f:
        sys.stdout.write(line) #Print without new line
    f.close()
    print '\n\t[TCP Socket Server:%s:%d]\n' % (server_inf)
    ##################################################

    #Create the MySQL poll sensors function
    thread.start_new_thread(MySQLPoll, ())

    while True:
        print 'Server is listening for connections...'
        socket, ClientIP = serversocket.accept()
        print '\tAdded a new client in: ', ClientIP
        thread.start_new_thread(handle_recv, (socket, ClientIP)) #Makes a new thread for handle received frames from each device
    serversocket.close()

##############################################################
#                                                            
#   Project: Home Control - Script to configure bt-hc-06    
#   Author: Anderson Ignacio da Silva                        
#   Date: 23/10/14                                           
#   Target: HC-06                                            
#   Inf.: http://www.esplatforms.blogspot.com.br             
#                                                            
##############################################################
#!/usr/bin/python
# Remember to configure the rfcomm.conf in /etc/bluetooth/ with 
# rfcomm0 {
#     # Automatically bind the device at startup
#     bind yes;

#     # Bluetooth address of the device
#     device (Bluetooth Address); > To get this use hcitool scan

#     # RFCOMM channel for the connection
#     channel 1;

#     # Description of the connection
#     comment "Description";
# }
#And pair the computer with the module
#After to connect use:
#sudo rfcomm connect rfcomm0
#and then open the terminal:
#sudo screen /dev/ttyUSB0 115200
#!/usr/bin/python
import serial
import time
 
PORT    = "/dev/ttyUSB0"                 #   If windows, change it to COMX
PINCODE = "1234"                         #   Change it to your PIN
DEVNAME = "HomeStark2"                   #   Change it to your device name
BAUDRATE= 115200                         #   Change it to your baudrate
BAUDRATE_MODULE_TO_CONFIGURE = 9600      #   Change it to your baudrate
  
 
baudlist = {
    1200    :   "AT+BAUD1",
    2400    :   "AT+BAUD2",
    4800    :   "AT+BAUD3",
    9600    :   "AT+BAUD4",
    19200   :   "AT+BAUD5",
    38400   :   "AT+BAUD6",
    57600   :   "AT+BAUD7",
    115200  :   "AT+BAUD8"
}
 
print "Opening Serial Port"
ser = serial.Serial(PORT, BAUDRATE_MODULE_TO_CONFIGURE, timeout=1)
 
print "Opened. Sending AT"
ser.write("AT")
data = ser.read(20)
print "Received: %s" %data
time.sleep(1)
 
print "Sending PINCODE Change: %s" %("AT+PIN%s"%PINCODE)
ser.write("AT+PIN%s"%PINCODE)
data = ser.read(20)
print "Received: %s" %data
time.sleep(1)
 
print "Sending DEVNAME Change: %s" %("AT+NAME%s"%DEVNAME)
ser.write("AT+NAME%s"%DEVNAME)
data = ser.read(20)
print "Received: %s" %data
time.sleep(1)
 
if baudlist.has_key(BAUDRATE):
    print "Sending BAUDRATE Change: %s" %(baudlist[BAUDRATE])
    ser.write(baudlist[BAUDRATE])
    data = ser.read(20)
    print "Received: %s" %data
    time.sleep(1)
else:
    print "Invalid baudrate %s. Skipping." %BAUDRATE
 
print "Finished!"

#!/bin/python

import cc2530_api as cc
import sys
import time

while True:
	cc.toggle_address('001','001')
	time.sleep(.5)
	cc.toggle_address('002','001')
	time.sleep(.5)
	#cc.send('002','0000')
	#time.sleep(.5)
	#cc.send('001','0001')
	#time.sleep(.5)
		

	# for x in range(0,10):
	# 	print "\nSending PWM=%d to router=%s..." % (x,'001')
	# 	cc.toggle_address('001','001')
	# 	if x <= 10:
	# 		PWM = '000'+str(x)
	# 	elif x<100:
	# 		PWM = '00'+str(x)
	# 	elif x<1000:
	# 		PWM = '0'+str(x)
	# 	cc.send('001',PWM)
	# 	time.sleep(.0050)

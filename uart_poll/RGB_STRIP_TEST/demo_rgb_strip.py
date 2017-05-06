#!/usr/bin/python
import serial_com as cc
import time

G = '127'
B = '127'
R = '127'
speed = 2
while True:
	R='255'
	G='255'
	B='255'
	cc.wr_str_default('A')
	cc.wr_str_default('C'+R+G+B+'F')
	#print "Testing RED"	
	time.sleep(speed)

	#R='000'
	#G='255'
	#B='000'
	#cc.wr_str_default('A')
	#cc.wr_str_default('C'+R+G+B+'F')
	#print "Testing GREEN"	
	#time.sleep(speed)

	#R='000'
	#G='000'
	#B='255'
	#cc.wr_str_default('A')
	#cc.wr_str_default('C'+R+G+B+'F')
	#print "Testing BLUE"
	#time.sleep(speed)

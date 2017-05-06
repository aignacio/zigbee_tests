#!/bin/python

import time
import gpio_cubie as gpio

gpio.act_pin(19)
gpio.act_pin(17)
gpio.act_pin(63)

gpio.out_cfg(17)
gpio.out_cfg(19)
gpio.in_cfg(63)

while(True):
	#gpio.out_wr(17,0)
	#gpio.out_wr(19,1)
	#time.sleep(0.1)
	#gpio.out_wr(17,1)
	#gpio.out_wr(19,0)
	#time.sleep(0.1)
	valor = gpio.in_rd(63)
	print "Valor lido:%s\n" % valor
	if int(valor) == 1:
		gpio.out_wr(17,1)
	        gpio.out_wr(19,0)
	else:
		gpio.out_wr(19,1)
		gpio.out_wr(17,0)




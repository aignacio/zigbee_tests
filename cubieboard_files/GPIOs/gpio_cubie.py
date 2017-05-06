##############################################################
#															 
#	Project: Home Control - GPIO socket	 				 
#	Author: Anderson Ignacio da Silva						 
#   Date: 05/11/14											 
#	Target: Cubieboard_2-A20								 
#	Inf.: http://www.esplatforms.blogspot.com.br 			 
#															 
##############################################################
#!/usr/bin/python
import os
import time

gpio_list = {
    1    :   'gpio1_pg3'  ,
    2    :   'gpio2_pb19' ,
    3    :   'gpio3_pb18' ,
    4    :   'gpio4_pg6'  ,
    5    :   'gpio5_pg5'  ,
    6    :   'gpio6_pg4'  ,
    7    :   'gpio7_pg1'  ,
    8    :   'gpio8_pg2'  ,
    9    :   'gpio9_pg0'  ,
    10   :   'gpio10_pg0' ,
    11   :   'gpio11_ph15',
    12   :   'gpio12_pi6' ,
    13   :   'gpio13_pi5' ,
    14   :   'gpio14_pi4' ,
    15   :   'gpio15_pg11',
    16   :   'gpio16_pg10',
    17   :   'gpio17_pg9' ,
    18   :   'gpio18_pg8' ,
    19   :   'gpio19_pg7' ,
    20   :   'gpio20_pg8' ,
    21   :   'gpio21_pg7' ,
    22   :   'gpio22_pe6' ,
    23   :   'gpio23_pe5' ,
    24   :   'gpio24_pe4' ,
    25   :   'gpio25_pi9' ,
    26   :   'gpio26_pi8' ,
    27   :   'gpio27_pi7' ,
    28   :   'gpio28_pd4' ,
    29   :   'gpio29_pd3' ,
    30   :   'gpio30_pd2' ,
    31   :   'gpio31_pd1' ,
    32   :   'gpio32_pd0' ,
    33   :   'gpio33_pe11',
    34   :   'gpio34_pe10',
    35   :   'gpio35_pe9' ,
    36   :   'gpio36_pd12',
    37   :   'gpio37_pd11',
    38   :   'gpio38_pd10',
    39   :   'gpio39_pd9' ,
    40   :   'gpio40_pd8' ,
    41   :   'gpio41_pd7' ,
    42   :   'gpio42_pd6' ,
    43   :   'gpio43_pd5' ,
    44   :   'gpio44_pd20',
    45   :   'gpio45_pd19',
    46   :   'gpio46_pd18',
    47   :   'gpio47_pd17',
    48   :   'gpio48_pd16',
    49   :   'gpio49_pd15',
    50   :   'gpio50_pd14',
    51   :   'gpio51_pd13',
    52   :   'gpio52_pb2' ,
    53   :   'gpio53_pd25',
    54   :   'gpio54_pd24',
    55   :   'gpio55_pd26',
    56   :   'gpio56_pd27',
    57   :   'gpio57_pd23',
    58   :   'gpio58_pd22',
    59   :   'gpio59_pd21',
    60   :   'gpio60_pi11',
    61   :   'gpio61_pi13',
    62   :   'gpio62_pi10',
    63   :   'gpio63_pi12',
    64   :   'gpio64_pb13',
    65   :   'gpio65_pb11',
    66   :   'gpio66_pb10',
    67   :   'gpio67_ph7' 
}

def in_cfg(pin):
	cmd = 'echo in > /sys/class/gpio/' + gpio_list[pin] + '/direction'
	os.system(cmd)

def in_rd(pin):
	cmd = 'cat /sys/class/gpio/' + gpio_list[pin] + '/value'
	value = os.popen(cmd).readline()
	return value

def out_cfg(pin):
	cmd = 'echo out > /sys/class/gpio/' + gpio_list[pin] + '/direction'
	os.system(cmd)

def out_wr(pin,value):
	cmd = 'echo ' + str(value) + ' > /sys/class/gpio/' + gpio_list[pin] + '/value'
	os.system(cmd)

def act_pin(pin):
	cmd = 'echo ' + str(pin) + ' > /sys/class/gpio/export'
	os.system(cmd)

if __name__ == '__main__':
	in_cfg(pin)
	value = in_rd(pin)
	out_cfg(pin)
	out_wr(pin,value)
	act_pin(pin)

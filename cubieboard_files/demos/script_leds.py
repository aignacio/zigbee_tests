#!/usr/bin/python
# Python example to blink those little LED's on the Cubieboard2
# Written by skr00b74 (skr00b74@gmail.com)
# I probably could have written this in a LOT less code.
# It is intended for learning purposes. That's it!

#We want to run some system commands so we need to import os
import os

#We need to use sleep in our loop so we need to import time
import time



#setup some strings to manage the blue led

#Turn the blue led on
blueled_on = 'echo 1 > /sys/class/leds/blue:ph21:led2/brightness'

#Turn the blue led off
blueled_off = 'echo 0 > /sys/class/leds/blue:ph21:led2/brightness'

#setup some strings to manage the green led

#Turn the green led on
greenled_on = 'echo 1 > /sys/class/leds/green:ph20:led1/brightness'

#Turn the green led off
greenled_off = 'echo 0 > /sys/class/leds/green:ph20:led1/brightness'

#Before we get started with the loop let's make sure we don't have triggers in use

#Set the blue trigger to none
os.system(blueled_trigger)

#Set the green trigger to none
os.system(greenled_trigger)

#Just in case lets make sure that the leds start in the off position

#Turn off the blue led
os.system(blueled_off)

#Turn off the green led
os.system(greenled_off)

#variable to keep track of the loops

loop_count = 0;

#Start the loop

while(True):
    if loop_count <= 10:
        #Turn on the blue led
        os.system(blueled_on)

        #Sleep for half a second
        time.sleep(0.5)

        #Turn off the blue led
        os.system(blueled_off)

        #Turn on the green led
        os.system(greenled_on)

        #Sleep for a half a second 
        time.sleep(0.5)

        #Turn off the green led
        os.system(greenled_off)

        #Let's add 1 to each loop count
        loop_count = loop_count + 1

    else:
        #We got here due to the fact loop count exceeded our if statement

        #Turn off the blue led
        os.system(blueled_off)

        #Turn off the green led   
        os.system(greenled_off)

        #We're done so lets break out of the loop

        break

#The end of the code

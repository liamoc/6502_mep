from serial import Serial
from time import *
ser = Serial("/dev/tty.usbserial-140",baudrate=115200) #or whatever 
with open('main.hex','rb') as openfileobject:
	for line in openfileobject:
		print(line.strip());
		ser.write(line.strip())
		ser.write(b'\r')
		sleep(0.01)
ser.write(b'0300R\r')

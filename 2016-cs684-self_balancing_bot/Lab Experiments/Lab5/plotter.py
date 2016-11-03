import numpy as np
import matplotlib.pyplot as plt
import time
import serial
import re

# configure the serial connections (the parameters differs on the device you are connecting to)
ser = serial.Serial(
    port='/dev/ttyACM1',
    baudrate=115200,
    parity=serial.PARITY_NONE,
    stopbits=serial.STOPBITS_ONE,
    bytesize=serial.EIGHTBITS
)

ser.isOpen()

print 'Enter your commands below.\r\nInsert "exit" to leave the application.'

plt.axis([-4096, 4096, -4096, 4096])
plt.ion()
first=True


input=1
while 1 :
    # get keyboard input
    #input = raw_input(">> ")
        # Python 3 users
        # input = input(">> ")
   # if input == 'exit':
     #   ser.close()
     #   exit()
   # else:
        # send the character to the device
        # (note that I happend a \r\n carriage return and line feed to the characters - this is requested by my device)
       # ser.write(input + '\r\n')
		out = ''
        # let's wait one second before reading output (let's give device time to answer)
       # time.sleep(1)
		x_axis = 1
		y_axis = 1
		while ser.inWaiting() > 0:
			out += ser.readline()

		if first:
			first=False
			continue
		if out != '':
			arr = re.split("[\r\n]+", out)
			if len(arr) > 0 :
				for coord in arr:
					co = re.split("[: ]+", coord)
					if len(co) == 2 :
						if co[0] == 'X' :
							x_axis = co[1]
						elif co[0] == 'Y' :
							y_axis = co[1]
						print x_axis, y_axis
			#print arr
			#print len(arr)
			#plt.clf()
			plt.scatter(int(x_axis), 0)
			plt.scatter(0, int(y_axis))
			plt.draw()
			plt.pause(0.05)



while True:
	plt.pause(0.05)
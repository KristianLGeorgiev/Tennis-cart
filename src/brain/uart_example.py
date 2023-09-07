#!/usr/bin/python3
import time
import serial

serial_port = serial.Serial(
    port="/dev/ttyACM3",
    baudrate=115200
)
# Wait a second to let the port initialize
time.sleep(1)
global width
width = 5
try:
    # Send a simple header
    while True:
        if serial_port.inWaiting() > 0:


	    

            data = str(width)
            
            serial_port.write(data.encode())
            # if we get a carriage return, add a line feed too
            # \r is a carriage return; \n is a line feed
            # This is to help the tty program on the other end 
            # Windows is \r\n for carriage return, line feed


except KeyboardInterrupt:
    print("Exiting Program")


finally:
    serial_port.close()
    pass

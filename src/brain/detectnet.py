#!/usr/bin/env python3

import time
import sys
import argparse
import serial

from jetson_inference import detectNet
from jetson_utils import videoSource, videoOutput, Log

serial_port = serial.Serial(
    port="/dev/ttyACM0",
    baudrate=115200
)

# parse the command line
parser = argparse.ArgumentParser(description="Locate objects in a live camera stream using an object detection DNN.", 
                                 formatter_class=argparse.RawTextHelpFormatter, 
                                 epilog=detectNet.Usage() + videoSource.Usage() + videoOutput.Usage() + Log.Usage())

parser.add_argument("input", type=str, default="", nargs='?', help="URI of the input stream")
parser.add_argument("output", type=str, default="", nargs='?', help="URI of the output stream")
parser.add_argument("--network", type=str, default="ssd-mobilenet", help="pre-trained model to load (see below for options)")
parser.add_argument("--overlay", type=str, default="box,labels,conf", help="detection overlay flags (e.g. --overlay=box,labels,conf)\nvalid combinations are:  'box', 'labels', 'conf', 'none'")
parser.add_argument("--threshold", type=float, default=0.5, help="minimum detection threshold to use")

# Wait a second to let the port initialize
time.sleep(1)

try:
	args = parser.parse_known_args()[0]
except:
	print("")
	parser.print_help()
	sys.exit(0)

# create video sources and outputs
input = videoSource(args.input, argv=sys.argv)
output = videoOutput(args.output, argv=sys.argv)
	
# load the object detection network
net = detectNet(args.network, sys.argv, args.threshold)

global width
width = 5
try:
   while True:
	if serial_port.inWaiting() > 0:

	    # capture the next image
	    img = input.Capture()

	    # detect objects in the image (with overlay)
	    detections = net.Detect(img, overlay=args.overlay)

	    # print the detections
	    #print("detected {:d} objects in image".format(len(detections)))
	    numberDet = (len(detections))

	    char1 = str(width)
	    serial_port.write(char1.encode())

	    output.Render(img)

	    # update the title bar
	    output.SetStatus("{:s} | Network {:.0f} FPS".format(args.network, net.GetNetworkFPS()))

	    # print out performance info
	    net.PrintProfilerTimes()

	    # exit on input/output EOS
	    if not input.IsStreaming() or not output.IsStreaming():
		break

except KeyboardInterrupt:
    print("Exiting Program")


finally:
    serial_port.close()
    pass	    

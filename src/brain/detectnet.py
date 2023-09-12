#!/usr/bin/env python3

import time
import sys
import argparse
import serial

import jetson_inference 
import jetson_utils 

MOVEMENT_STOP 		= 0
MOVEMENT_DIR_FWD 	= 1 
MOVEMENT_DIR_BKWD	= 2
MOVEMENT_DIR_LT		= 3
MOVEMENT_DIR_RT		= 4
MOVEMENT_DIR_LS		= 5
MOVEMENT_DIR_RS		= 6




# parse the command line
#parser = argparse.ArgumentParser(description="Locate objects in a live camera stream using an object detection DNN.", 
                                 #formatter_class=argparse.RawTextHelpFormatter, 
                                 #epilog=detectNet.Usage() + videoSource.Usage() + videoOutput.Usage())

#parser.add_argument("input", type=str, default="", nargs='?', help="URI of the input stream")
#parser.add_argument("output", type=str, default="", nargs='?', help="URI of the output stream")
#parser.add_argument("--network", type=str, default="ssd-mobilenet", help="pre-trained model to load (see below for options)")
#parser.add_argument("--overlay", type=str, default="box,labels,conf", help="detection overlay flags (e.g. --overlay=box,labels,conf)\nvalid combinations are:  'box', 'labels', 'conf', 'none'")
#parser.add_argument("--threshold", type=float, default=0.5, help="minimum detection threshold to use")

# Wait a second to let the port initialize
time.sleep(1)

#try:
#	args = parser.parse_known_args()[0]
#except:
#	print("")
#	parser.print_help()
#	sys.exit(0)

# create video sources and outputs
#input = videoSource(args.input, argv=sys.argv)
#output = videoOutput(args.output, argv=sys.argv)
net = jetson_inference.detectNet(argv=['--model=models/Ballz/ssd-mobilenet.onnx', '--labels=models/Ballz/labels.txt', '--input-blob=input_0', '--output-cvg=scores', '--output-bbox=boxes'], threshold=0.5)
camera = jetson_utils.videoSource("csi://0")
display = jetson_utils.videoOutput("display://0")
	
# load the object detection network
#net = detectNet(args.network, sys.argv, args.threshold)

global center
center = 0

def movement(detections) -> int:
	"""
	Movement logic:
	1. biggest bounding box is the ball that is the nearest to the camera
	2. We wanto center the ball relative to the camera
		2.1. Get Center()coordinates of biggest bounding box
		2.2. We aim to have the center of the boundng box (the X-coordinate) in the
		"middle" of the frame
			2.2.1 - the "middle" of the frame we defined as having X coordantes:
				>= 480 && <= 800
			
			Camera frame:

			<------------------------1280-------------------------->
			^
			|
			| 720
			|
			v 
			________________________________________________________
			|								                       |
			|								                       |
			|								                       |
			|								                       |
			|								                       |
			________________________________________________________
			<-------------------------1280------------------------->
			<-----------640------------><-----------640------------>
			<----320-----><----320-----><----320-----><----320----->
			<-160-><-160-><-160-><-160-><-160-><-160-><-160-><-160->
								|<<<  MID  >>>|
									width 320
									coords: 480..800
		
		2.3. if ball position ... (left, right, center), then robot ... (turn left, turn right, fwd)
			...... TO
			

	"""
	for detection in detections:
		center = detection.Center
		if center[0] > 480 and center[0] < 800: #fwd
			return MOVEMENT_DIR_FWD
		elif center[0] < 480: #left
			return MOVEMENT_DIR_LT
		elif center[0] > 800: #right
			return MOVEMENT_DIR_RT

	return

def locate(detections):
	arduino.write(str(MOVEMENT_DIR_FWD).encode())
	time.sleep(2)
	arduino.write(str(MOVEMENT_DIR_LS).encode())
	time.sleep(2)
	return

try:
	with serial.Serial(port="/dev/ttyACM0", baudrate=115200, timeout=None) as arduino:

		while display.IsStreaming():
			
			img = camera.Capture()
			detections = net.Detect(img)
			display.Render(img)
			numberDet = (len(detections))

			if numberDet > 0:
				dir = movement(detections)
				arduino.write(str(dir).encode())

			else:
				dir = locate(detections)
				arduino.write(str(dir).encode())
				# capture the next image
				
				# detect objects in the image (with overlay)
				

				# print the detections
				#print("detected {:d} objects in image".format(len(detections)))
				

				
				#movementDir = movement(detections)

				#arduino.write(str(movementDir).encode())
				#print(f'Movement direction: {movementDir}')

				

				# update the title bar
			display.SetStatus("Object | Network {:.0f} FPS".format (net.GetNetworkFPS()))

				# print out performance info
				#net.PrintProfilerTimes()

					# exit on input/output EOS
			if not camera.IsStreaming() or not display.IsStreaming():
				break

except KeyboardInterrupt:
	print("Exiting Program")
	exit()


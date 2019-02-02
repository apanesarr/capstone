from collections import deque
from threading import Thread

from imutils.video import VideoStream
import numpy as np
import argparse
import cv2
import imutils
import time
import math

ap = argparse.ArgumentParser()
ap.add_argument("-v", "--video",
	help="path to the (optional) video file")
ap.add_argument("-b", "--buffer", type=int, default=64,
	help="max buffer size")
args = vars(ap.parse_args())

# Parameters - Object Tracking #

MIN_RADIUS = 10
MAX_RADIUS = 50
COLOR_LOWER = (29, 86, 6)
COLOR_UPPER = (64, 255, 255)

# Parameters - Geometry #

CAMERA_RES_W = 500
CAMERA_RES_H = 500

COVERAGE_AREA_X_MIN = 50
COVERAGE_AREA_Y_MIN = 50
COVERAGE_AREA_X_MAX = 450
COVERAGE_AREA_Y_MAX = 450

NUM_REGIONS_X = 10
NUM_REGIONS_Y = 10

# End Parameters #

regionSizeX = (COVERAGE_AREA_X_MAX - COVERAGE_AREA_X_MIN) / NUM_REGIONS_X
regionSizeY = (COVERAGE_AREA_Y_MAX - COVERAGE_AREA_Y_MIN) / NUM_REGIONS_Y

class Location(Thread):
	def __init__(self, val):
		self.regions = np.full([NUM_REGIONS_X, NUM_REGIONS_Y], {})

		for i in range(0, self.regions.shape[0]):
			for j in range(0, self.regions.shape[1]):
				region = {'visited': False }

				x = (i / NUM_REGIONS_X) * (1.5) * (regionSizeX)
				y = (j / NUM_REGIONS_Y) * (1/5) * (regionSizeY)

				region['center'] = (x, y)

				self.regions[i][j] = region

		self.rawLocation = (-1, -1)

	def run(self):
		pts = deque(maxlen=args["buffer"])

		if not args.get("video", False):
			vs = VideoStream(src=0).start()
		else:
			vs = cv2.VideoCapture(args["video"])

		# allow the camera or video file to warm up
		time.sleep(2.0)

		while True:
			frame = vs.read()

			frame = frame[1] if args.get("video", False) else frame

			if frame is None:
				break

			frame = imutils.resize(frame, width=CAMERA_RES_W, height=CAMERA_RES_H)
			blurred = cv2.GaussianBlur(frame, (11, 11), 0)
			hsv = cv2.cvtColor(blurred, cv2.COLOR_BGR2HSV)

			mask = cv2.inRange(hsv, COLOR_LOWER, COLOR_UPPER)
			mask = cv2.erode(mask, None, iterations=2)
			mask = cv2.dilate(mask, None, iterations=2)

			cnts = cv2.findContours(mask.copy(), cv2.RETR_EXTERNAL,
									cv2.CHAIN_APPROX_SIMPLE)
			cnts = imutils.grab_contours(cnts)
			center = None

			if len(cnts) > 0:
				# find the largest contour in the mask, then use
				# it to compute the minimum enclosing circle and
				# centroid
				c = max(cnts, key=cv2.contourArea)
				((x, y), radius) = cv2.minEnclosingCircle(c)
				M = cv2.moments(c)
				center = (int(M["m10"] / M["m00"]), int(M["m01"] / M["m00"]))

				# only proceed if the radius meets a minimum size
				if radius > MIN_RADIUS and radius < MAX_RADIUS:
					# draw the circle and centroid on the frame,
					# then update the list of tracked points
					cv2.circle(frame, (int(x), int(y)), int(radius),
							   (0, 255, 255), 2)
					cv2.circle(frame, center, 5, (0, 0, 255), -1)

					rawLocation = center

			# update the points queue
			pts.appendleft(center)

			# loop over the set of tracked points
			for i in range(1, 1):  # for i in range(1, len(pts)):
				# if either of the tracked points are None, ignore
				# them
				if pts[i - 1] is None or pts[i] is None:
					continue

				# otherwise, compute the thickness of the line and
				# draw the connecting lines
				thickness = int(np.sqrt(args["buffer"] / float(i + 1)) * 2.5)
				cv2.line(frame, pts[i - 1], pts[i], (0, 0, 255), thickness)

			# show the frame to our screen
			cv2.imshow("Frame", frame)
			key = cv2.waitKey(1) & 0xFF

			# if the 'q' key is pressed, stop the loop
			if key == ord("q"):
				break

		# if we are not using a video file, stop the camera video stream
		if not args.get("video", False):
			vs.stop()

		# otherwise, release the camera
		else:
			vs.release()

		# close all windows
		cv2.destroyAllWindows()

	def exactLocation(self):
		if not self.rawLocation: return (-1, -1)

		x = (self.rawLocation[0] - COVERAGE_AREA_X_MIN) / (COVERAGE_AREA_X_MAX - COVERAGE_AREA_X_MIN)
		y = (self.rawLocation[1] - COVERAGE_AREA_Y_MIN) / (COVERAGE_AREA_Y_MAX - COVERAGE_AREA_Y_MIN)

		return (x, y)

	def distance(self, p1, p2):
		deltaX = abs(p1[0], p2[1])
		deltaY = abs(p2[0], p2[1])

		return math.sqrt(deltaX * deltaX + deltaY * deltaY)

	def nextLocation(self):
		currentLoc = self.exactLocation()

		if (currentLoc == (-1, -1)): return (-1, -1)

		min = self.regions[0]

		for r in self.regions:
			cent = r.center

			if not r.visited and self.distance(cent, currentLoc) < min:
				min = r

		return r.center

	def region(self):
		loc = self.exactLocation()

		if (loc == (-1, -1)): return loc

		return (loc[0] * NUM_REGIONS_X, loc[1] * NUM_REGIONS_Y)
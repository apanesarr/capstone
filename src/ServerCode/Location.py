from collections import deque
from threading import Thread

from imutils.video import VideoStream
import numpy as np
import argparse
import cv2
import imutils
import time
import math

import Parameters

regionSizeX = 1
regionSizeY = 1

class Location:
	def createRegions(self):
		regions = np.full([Parameters.NUM_REGIONS_X, Parameters.NUM_REGIONS_Y], {})

		for i in range(0, regions.shape[0]):
			for j in range(0, regions.shape[1]):
				region = {"visited": False }

				x = (i / Parameters.NUM_REGIONS_X) * (1.5) * (regionSizeX)
				y = (j / Parameters.NUM_REGIONS_Y) * (1/5) * (regionSizeY)

				region["center"] = (x, y)

				regions[i][j] = region

		return regions

	def __init__(self):
		self.regions 		= self.createRegions()
		self.rawLocation 	= (-1, -1)
		self.hasTarget		= False

	def updateLocation(self, location):
		self.rawLocation = location

	def exactLocation(self):
		if not self.rawLocation: return (-1, -1)

		x = (self.rawLocation[0] - Parameters.COVERAGE_AREA_X_MIN) / (Parameters.COVERAGE_AREA_X_MAX - Parameters.COVERAGE_AREA_X_MIN)
		y = (self.rawLocation[1] - Parameters.COVERAGE_AREA_Y_MIN) / (Parameters.COVERAGE_AREA_Y_MAX - Parameters.COVERAGE_AREA_Y_MIN)

		return (x, y)

	def distance(p1, p2):
		deltaX = abs(p1[0], p2[1])
		deltaY = abs(p2[0], p2[1])

		return math.sqrt(deltaX * deltaX + deltaY * deltaY)

	def nextLocation(self):
		# If we"re out of bounds... screw it, it"s game over
		if (self.rawLocation == (-1, -1)): return (-1, -1)

		min = self.regions[0]

		for r in self.regions:
			cent = r.center

			if not r.visited and self.distance(cent, currentLoc) < min:
				min = r

		return r.center
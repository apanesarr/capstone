from collections import deque
from threading import Thread

import numpy as np
import time
import math

import Parameters

regionSizeX = (Parameters.COVERAGE_AREA_X_MAX - Parameters.COVERAGE_AREA_X_MIN) / Parameters.NUM_REGIONS_X
regionSizeY = (Parameters.COVERAGE_AREA_Y_MAX - Parameters.COVERAGE_AREA_Y_MIN) / Parameters.NUM_REGIONS_Y

class Location:
	def createRegions(self):
		regions = np.full([Parameters.NUM_REGIONS_X, Parameters.NUM_REGIONS_Y], {})

		for i in range(0, regions.shape[0]):
			for j in range(0, regions.shape[1]):
				region = {"visited": False }

				# Calculate center of region
				x = (i + 0.5) * regionSizeX
				y = (i + 0.5) * regionSizeY

				region["center"] = (x, y)

				regions[i][j] = region

		return regions

	def __init__(self):
		self.regions 		= self.createRegions()

	def exactLocation(self):
		if not self.rawLocation: return (-1, -1)

		x = (self.rawLocation[0] - Parameters.COVERAGE_AREA_X_MIN) / (Parameters.COVERAGE_AREA_X_MAX - Parameters.COVERAGE_AREA_X_MIN)
		y = (self.rawLocation[1] - Parameters.COVERAGE_AREA_Y_MIN) / (Parameters.COVERAGE_AREA_Y_MAX - Parameters.COVERAGE_AREA_Y_MIN)

		return (x, y)

	def nextLocation(self, insect):
		currentLoc = insect.currentLocation()

		# If we're out of bounds... screw it, it's game over for this insect
		if (currentLoc == (-1, -1)):
			return (-1, -1)

		min = self.regions[0][0]
		minD = 99999

		for r in self.regions:
			for j in r:
				cent = j["center"]

				if not j["visited"] and self.distance(cent, currentLoc) < minD:
					min = j
					minD = self.distance(cent, currentLoc)

		return min["center"]

	def roundToRegion(self, x, y):
		# TODO
		pass

	# A measurement has been made so mark it as visited
	def measurementMade(self, x, y):
		(x, y) = roundToRegion(x, y)

		self.regions[x][y]["visited"] = True

	def distance(self, p1, p2):
		deltaX = abs(p1[0] - p2[1])
		deltaY = abs(p2[0] - p2[1])

		return math.sqrt(deltaX * deltaX + deltaY * deltaY)

	def angle(self, location_current, location_new):
		delta = (location_new[0] - location_current[0], location_new[1] - location_current[1])

		return math.degrees(np.arctan2(delta[0], delta[1]))
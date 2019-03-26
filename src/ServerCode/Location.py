from collections import deque
from threading import Thread

import numpy as np
import time
import math

import Parameters

regionSizeX = Parameters.REGION_SIZE_X
regionSizeY = Parameters.REGION_SIZE_Y

VISITED_STATUS_NEVER 		= 0
VISITED_STATUS_DONE  		= 1
VISITED_STATUS_IN_PROGRESS 	= 2

class Location:
	def createRegions(self):
		regions = np.zeros([Parameters.NUM_REGIONS_X, Parameters.NUM_REGIONS_Y], {})

		for i in range(0, regions.shape[0]):
			for j in range(0, regions.shape[1]):
				region = {"visited": False }

				# Calculate center of region
				x = (i + 0.5) * regionSizeX
				y = (i + 0.5) * regionSizeY

				region["center"] = (x, y)

				regions[i][j] = region

		return regions

	def __init__(self, measurements):
		self.regions 		= self.createRegions()
		self.measurements	= measurements

	def regionComplete(self, region):
		center = region["center"]

		filtered = [item for item in self.measurements if lambda x : center == round(x["location"])]

		return len(filtered) > 0

	def surveyComplete(self):
		for i in range(0, self.regions.shape[0]):
			for j in range(0, self.regions.shape[1]):
				r = self.regions[i][j]

				if not regionComplete(r):
					return False

		return True

	def getInsect(self, insects, insectId):
		return insects[0]

	def nextState(self, insect):
		currentLoc	= insect.currentLocation
		nextLoc		= self.nextLocation(insect)

		if nextLoc == (-1, -1):
			return { 'State': 'STOP' }

		# TODO just testing
		self.hasTarget = True

		return { 'State': 'FORWARD', 'Distance': '20' }

	def nextLocation(self, insect):
		currentLoc = insect.currentLocation

		# If we're out of bounds... screw it, it's game over for this insect
		if (currentLoc == (-1, -1)):
			return (-1, -1)

		min = self.regions[0][0]
		minD = 9999999

		for r in self.regions:
			for j in r:
				cent = j['center']

				if j['visited'] == VISITED_STATUS_NEVER and self.distance(cent, currentLoc) < minD:
					min = j
					minD = self.distance(cent, currentLoc)

		min['visited'] = VISITED_STATUS_IN_PROGRESS
		
		return min['center']

	def roundToRegion(self, x, y):
		return (round(x), round(y))			

	# A measurement has been made so mark it as visited
	def measurementMade(self, x, y):
		(x, y) = roundToRegion(x, y)

		self.regions[x][y]["visited"] = VISITED_STATUS_DONE

	def distance(self, p1, p2):
		deltaX = abs(p1[0] - p2[1])
		deltaY = abs(p2[0] - p2[1])

		return math.sqrt(deltaX * deltaX + deltaY * deltaY)

	def angle(self, location_current, location_new):
		delta = (location_new[0] - location_current[0], location_new[1] - location_current[1])

		return math.degrees(np.arctan2(delta[0], delta[1]))
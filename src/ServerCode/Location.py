import numpy as np
import math

import Parameters

regionSizeX = Parameters.REGION_SIZE_X
regionSizeY = Parameters.REGION_SIZE_Y

VISITED_STATUS_NEVER 		= 0
VISITED_STATUS_DONE  		= 1
VISITED_STATUS_IN_PROGRESS 	= 2

NUM_REGIONS_X = Parameters.NUM_REGIONS_X
NUM_REGIONS_Y = Parameters.NUM_REGIONS_Y

VERBOSE = False

moves = [
	{'State': 'FORWARD', 'Distance': 400},
	{'State': 'FORWARD', 'Distance': 400},
	{'State': 'FORWARD', 'Distance': 400},
	{'State': 'ROTATE', 'Angle': 90},
	{'State': 'FORWARD', 'Distance': 400},
	{'State': 'ROTATE', 'Angle': 180},
	{'State': 'FORWARD', 'Distance': 400},
]

class Location:
	def createRegions(self):
		regions = []

		# Create empty matrix
		for i in range(0, NUM_REGIONS_X):
			x = []

			for j in range(0, NUM_REGIONS_Y):
				x.append(0)

			regions.append(x)

		for i in range(0, NUM_REGIONS_X):
			for j in range(0, NUM_REGIONS_Y):
				region = { "visited": VISITED_STATUS_NEVER }

				# Calculate center of region
				x = (i + 0.5) * regionSizeX
				y = (j + 0.5) * regionSizeY

				region["center"] = (x, y)

				regions[i][j] = region

		return regions

	def __init__(self, measurements):
		self.regions 		= self.createRegions()
		self.measurements	= measurements
		self.hasTarget      = False
		self.move_index     = 0

	def regionComplete(self, region):
		center = region["center"]

		filtered = [item for item in self.measurements if lambda x : center == round(x["location"])]

		return len(filtered) > 0

	def surveyComplete(self):
		for i in range(0, self.regions.shape[0]):
			for j in range(0, self.regions.shape[1]):
				r = self.regions[i][j]

				if not self.regionComplete(r):
					return False

		return True

	def getInsect(self, insects, insectId):
		for insect in insects:
			if insect.insectId == insectId:
				return insect

		return None

		print("Insect out of bounds")

	def nextState(self, insect):
		m = moves[self.move_index]
		self.move_index += 1

		return m

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

		insect.target = min['center']
		
		return min['center']

	def roundToRegion(self, x, y):
		return (round(x), round(y))			

	# A measurement has been made so mark it as visited
	def measurementMade(self, x, y):
		(x, y) = self.roundToRegion(x, y)

		self.regions[x][y]["visited"] = VISITED_STATUS_DONE

	def distance(self, p1, p2):
		deltaX = abs(p1[0] - p2[0])
		deltaY = abs(p1[1] - p2[1])

		return math.sqrt(deltaX * deltaX + deltaY * deltaY)

	def angle(self, angle, location_current, location_new):
		delta = (location_new[0] - location_current[0], location_new[1] - location_current[1])

		x = math.degrees(np.arctan2(delta[1], delta[0]))

		return x
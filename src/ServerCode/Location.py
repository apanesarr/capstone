import numpy as np
import math

import Parameters

VISITED_STATUS_NEVER 		= 0
VISITED_STATUS_DONE  		= 1
VISITED_STATUS_IN_PROGRESS 	= 2
VISITED_STATUS_DEAD         = 3

class Location:
	def createRegions(self):
		regions = []

		for i in range(0, Parameters.NUM_REGIONS_X):
			regions.append([])

			for j in range(0, Parameters.NUM_REGIONS_Y):
				# Calculate center of region
				x = (i + 0.5) * Parameters.REGION_SIZE_X
				y = (j + 0.5) * Parameters.REGION_SIZE_Y

				region = {
					"visited": VISITED_STATUS_NEVER,
					"center" : (x, y)
				}

				regions[i].append(region)

    # TODO - make this better
		regions[0][0]["visited"] = VISITED_STATUS_IN_PROGRESS
		regions[0][1]["visited"] = VISITED_STATUS_IN_PROGRESS

		return regions

	def __init__(self, measurements):
		self.measurements	= measurements
		self.regions 		= self.createRegions()

	def surveyComplete(self):
		for r in self.regions:
			for s in r:
				if not (s["visited"] == VISITED_STATUS_DONE):
					return False

		return True

	# TODO - move this somewhere else
	def getInsect(self, insects, insectId):
		for insect in insects:
			if insect.insectId == insectId:
				return insect

		return None

		print("Insect out of bounds")


	def nextState(self, insect):
		currentLoc	= insect.currentLocation

		if insect.hasTarget:
			nextLoc = insect.target
		else:
			nextLoc		= self.nextLocation(insect)

		if Parameters.VERBOSE:
			print('Moving to location: %s - %s' % (nextLoc[0], nextLoc[1]))

		if nextLoc == (-1, -1):
			return { 'State': 'STOP' }

		insect.hasTarget = True
		insect.target    = nextLoc

		dist = self.distance(currentLoc, nextLoc)
		angl = self.angle(insect.angle, currentLoc, nextLoc)

		(x, y) = (dist * math.cos(math.radians(angl)), dist * math.sin(math.radians(angl)))

		if abs(x) > 10:
			new_angl = - insect.angle

			if abs(new_angl) > 10:
				return { 'State': 'ROTATE', 'Angle': 0 }

			if x > 0:
				return { 'State': 'FORWARD', 'Distance': abs(x) }
			else:
				return { 'State': 'REVERSE', 'Distance': abs(x) }

		if abs(insect.angle - 90) > 10:
			return { 'State': 'ROTATE', 'Angle': 90 }

		if y > 0:
			return { 'State': 'FORWARD', 'Distance': abs(y) }
		else:
			return { 'State': 'REVERSE', 'Distance': abs(y) }

	def nextLocation(self, insect):
		currentLoc = insect.currentLocation

		# If we're out of bounds... screw it, it's game over for this insect
		if (currentLoc == (-1, -1)):
			return (-1, -1)

		min  = self.getRegionAt(insect.defaultLocation[0], insect.defaultLocation[1])
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

	def getRegionAt(self, x, y):
		for i in self.regions:
			for j in i:
				center = j["center"]

				if (abs(x - center[0]) <= (Parameters.REGION_SIZE_X / 2)) and (abs(y - center[1]) <= (Parameters.REGION_SIZE_Y / 2)):
					return j

		return None

	# A measurement has been made so mark it as visited
	def measurementMade(self, x, y):
		region = self.getRegionAt(x, y)

		region["visited"] = VISITED_STATUS_DONE

	def distance(self, p1, p2):
		deltaX = abs(p1[0] - p2[0])
		deltaY = abs(p1[1] - p2[1])

		return math.sqrt(deltaX * deltaX + deltaY * deltaY)

	def angle(self, angle, location_current, location_new):
		delta = (location_new[0] - location_current[0], location_new[1] - location_current[1])

		x = math.degrees(np.arctan2(delta[1], delta[0]))

		return x
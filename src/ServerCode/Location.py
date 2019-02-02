from collections import deque
from threading import Thread

from imutils.video import VideoStream
import numpy as np
import argparse
import cv2
import imutils
import time
import math

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
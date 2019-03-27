import time
import queue
import threading

import Reader
import Writer
import Parameters
class Insect:
    def __init__(self, insectId):
        self.insectId        = insectId
        self.currentLocation = (0, 0)
        self.hasTarget       = False
        self.target          = (-1, -1)
        self.angle           = 0

    def arrived(self):
        x1 = self.currentLocation[0]
        y1 = self.currentLocation[1]

        x2 = self.target[0]
        y2 = self.target[1]

        return abs(x1 - x2) < Parameters.REGION_SIZE_X and abs(y1 - y2) < Parameters.REGION_SIZE_Y
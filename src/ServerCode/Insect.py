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
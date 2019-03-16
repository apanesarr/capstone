from collections import deque
import threading
import time
import queue

from imutils.video import VideoStream
import numpy as np
import cv2
import imutils
import argparse

import Communication
import Parameters
import Location

import Reader
import Writer

def handleComEvent(queue):
    if queue.empty():
        return

    item = queue.get()

    # if it's not meant for us, put it back and return
    if not (item["recipient"] == "MAIN"):
        queue.put(item)
        return

    elif (item["command" == "Pair"):
        insectId = item["insectId"]

        ## TODO - add insect

    elif (item["command"] == "RequestMeasurement"):
        measurement = item["measurement"]
        insectId    = item["insectId"]

        measurements.append({
            "insectId"      : insectId
            "location"      : location.exactLocation(),
            "measurement"   : measurement
        })

        getInsect(insectId).hasTarget = False

    elif (item["command"] == "GetLocation"):
        data = item["location"]

        # TODO - are we doing this?
        

if __name__ == "__main__":
    # Set up Communication threads

    queue_writer = queue.Queue()
    event_writer = threading.Event()
    event_writer.queue = queue_writer

    queue_reader = queue.Queue()
    event_reader = threading.Event()
    event_reader.queue = queue_reader 

    measurements    = [] # List of recieved measurement data
    recieved        = [] # List of recieved messages
    insects         = pairInsects() # List of insects

    reader = Reader.Reader(Parameters.SERIAL_PORT_IN, Parameters.SERIAL_BODE, queue_reader, recieved)
    writer = Writer.Writer(Parameters.SERIAL_PORT_OUT, Parameters.SERIAL_BODE, queue_writer, recieved)

    reader.start()
    writer.start()

    # TODO replace
    rawLocation = (-1, -1) 
    nextLocation = (-1, -1)

    location = Location.Location() # TODO

    print("Initialization complete. Surveying area...")

    endTime = time.time() + Parameters.SURVEY_TIME

    # Main loop
    while time.time() < Parameters.SURVEY_TIME: # TODO - check for when the process is complete
        handleComEvent(queue_reader)

"""             queue_writer.put({
                "recipient"     : "COM",
                "command"       : "RequestMeasurement",
                "insectId"      : 1
            }) """
            location.hasTarget = True

        if not location.hasTarget:
            loc = location.nextLocation()

            queue_writer.put({
                "recipient"     : "COM",
                "command"       : "SetState"
            }) # TODO - we still need this funcitonality that determines the correct state to set

            location.target = loc
            location.hasTarget = True

        # if the "q" key is pressed, stop the loop
        if key == ord("q"):
            break

        time.sleep(0.2)
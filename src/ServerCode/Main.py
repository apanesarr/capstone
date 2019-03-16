from collections import deque
import threading
import time
import queue

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

    if (item["command" == "Arrived"):
        # TODO
        pass

    elif (item["command"] == "RequestMeasurement"):
        measurement = item["measurement"]
        insectId    = item["insectId"]

        measurements.append({
            "insectId"      : insectId
            "location"      : location.exactLocation(),
            "measurement"   : measurement
        })

        getInsect(insects, insectId).hasTarget = False

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

    location = Location.Location(measurements) # TODO

    print("Initialization complete. Surveying area...")

    endTime = time.time() + Parameters.SURVEY_TIME

    # Main loop
    while time.time() < Parameters.SURVEY_TIME: # TODO - check for when the process is complete
        handleComEvent(queue_reader)

        for insect in insects:
            if not insect.hasTarget:
                queue_writer.put({
                    "recipient"     : "COM",
                    "command"       : "SetState",
                    "state"         : location.nextLocation(insect)
                })

        # if the "q" key is pressed, stop the loop
        if key == ord("q"):
            break

        time.sleep(0.2)
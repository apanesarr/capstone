from collections import deque
import threading
import time
import queue

import Communication
import Parameters
import Location

import Reader
import Writer

class Main(Threading.thread):
    def handleComEvent(self):
        if self.queue.empty():
            return

        item = queue.get()

        # if it's not meant for us, put it back and return
        if not (item["recipient"] == "MAIN"):
            self.queue.put(item)
            return

        if (item["command" == "Arrived"):
            insectId    = item["insectId"]

            getInsect(insects, insectId).hasTarget = False

        elif (item["command"] == "RequestMeasurement"):
            measurement = item["measurement"]
            insectId    = item["insectId"]
            insect      = getInsect(insects, insectId)
            loc         = insect.exactloc()

            measurements.append({
                "insectId"      : insectId
                "location"      : loc,
                "measurement"   : measurement
            })

            print("Measurement made:")
            print("InsectId - %s - loc - (%s, %s) - Temperature - %s - Humidity - %s" \
                % (insectId, loc["X"], loc["Y"], measurement["Temperature"], measurement["Humidity"]))

            insect.hasTarget = False

    def __init__(self):
        # Set up Communication threads
        self.queue_writer = queue.Queue()
        self.event_writer = threading.Event()
        self.event_writer.queue = queue_writer

        self.queue_reader = queue.Queue()
        self.event_reader = threading.Event()
        self.event_reader.queue = queue_reader 

        self.measurements    = [] # List of recieved measurement data
        self.recieved        = [] # List of recieved messages
        self.insects         = pairInsects() # List of insects

        self.reader = Reader.Reader(Parameters.SERIAL_PORT_IN, Parameters.SERIAL_BODE, queue_reader, recieved)
        self.writer = Writer.Writer(Parameters.SERIAL_PORT_OUT, Parameters.SERIAL_BODE, queue_writer, recieved)

        self.reader.start()
        self.writer.start()

        self.location = Location.Location(measurements) # TODO

        print("Initialization complete")

    def run(self):
        endTime = time.time() + Parameters.SURVEY_TIME

        print("Starting surveying")

        # Main loop
        while (not self.location.surveyComplete()) and time.time() < Parameters.SURVEY_TIME:
            self.handleComEvent(queue_reader)

            # Set new targets for insects that aren't doing anything
            for insect in self.insects:
                if not insect.hasTarget:
                    self.queue_writer.put({
                        "recipient"     : "COM",
                        "command"       : "SetState",
                        "state"         : self.location.nextLocation(insect)
                    })

            # if the "q" key is pressed, stop the loop
            if key == ord("q"):
                break

            time.sleep(0.2)

if __name__ == "__main__":
    main = Main()
    main.start()
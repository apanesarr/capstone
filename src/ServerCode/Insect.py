import time
import queue
import threading

import Reader
import Writer
import Parameters

def getInsect(insects, insectId):
    for insect in insects:
        if insect["insectId"] == insectId:
            return insect

    return null

# Waits for insects to pair and initializes them
def pairInsects():
    print("Beginning pairing")

    endTime = time.time() + Parameters.PAIR_TIME

    # Initialize serial communication
    queue_writer = queue.Queue()
    event_writer = threading.Event()
    event_writer.queue = queue_writer

    queue_reader = queue.Queue()
    event_reader = threading.Event()
    event_reader.queue = queue_reader

    insects  = []
    recieved = []

    try:
        reader = Reader.Reader(Parameters.SERIAL_PORT_IN, Parameters.SERIAL_BODE, queue_reader, recieved)
        writer = Writer.Writer(Parameters.SERIAL_PORT_IN, Parameters.SERIAL_BODE, queue_writer, recieved)
    except Exception as e:
        print('pairInsects() - SerialException - %s' % e)
        return []

    reader.start()
    writer.start()

    while (time.time() < endTime and len(insects) < Parameters.NUM_INSECTS):
        if not queue_reader.empty():
            item = queue_reader.get()

            if (item["recipient"] == "PAIR" and item["command"] == "PAIR"):
                insectId   = item["insectId"]

                ins = Insect(insectId)

                insects.append(ins)

            # If it's not for us, put it back
            else:
                queue_reader.put(item)

        time.sleep(2)
        print("...")

    print("%i out of %i insects paired" % (len(insects), Parameters.NUM_INSECTS))
    return insects

class Insect:
    def __init__(self, insectId, colorRange):
        self.insectId        = insectId
        self.currentLocation = (-1, -1)
        self.hasTarget       = False
        self.target          = (-1, -1)
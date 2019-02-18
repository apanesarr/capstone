import time

# Waits for insects to pair and initializes them
def pairInsects():
    endTime = time.time() + 60 * Parameters.PAIR_TIME

    # Initialize serial communication
    queue_writer = queue.Queue()
    event_writer = threading.Event()
    event_writer.queue = queue_writer

    queue_reader = queue.Queue()
    event_reader = threading.Event()
    event_reader.queue = queue_reader

    reader = Reader.Reader(Parameters.SERIAL_PORT_IN, Parameters.SERIAL_BODE, queue_reader, recieved)
    writer = Writer.Writer(Parameters.SERIAL_PORT_IN, Parameters.SERIAL_BODE, queue_writer, recieved)

    reader.start()
    writer.start()

    insects = []

    while (time.time() < endTime):
        if not queue_reader.empty():
            item = queue_reader.get()

            if (item["recipient"] == "PAIR"):
                insectId   = item["id"]
                colorRange = item["colorRange"]

                ins = Insect(insectId, colorRange)

                insects.append(ins)

class Insect:
    def __init__(self, insectId, colorRange):
        self.insectId        = insectId
        self.currentLocation = (-1, -1)
        self.hasTarget       = False
        self.target          = (-1, -1)
        self.colorRange      = colorRange

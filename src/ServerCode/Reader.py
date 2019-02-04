import serial
import threading
from time import sleep

import Parameters

class Reader(threading.Thread):
    def __init__(self, port, bps, queue, recieved, sleep=0.01):
        self.port = port
        self.bps = bps
        self.queue = queue
        self.sleep = sleep
        self.recieved = recieved
        self.ser = serial.Serial(self.port, self.bps)

        super().__init__()

    def run(self):
        while True:
            if (self.ser.inWaiting() > 0):
                print('hi2')
                data = self.ser.readline()

                try:
                    data = data.decode("utf-8").replace("\r\n", "").split(",")

                    # Proper responses should have 3 comma seperated values
                    if len(data) == 3:
                        messageId   = data[0]
                        messageType = data[1]
                        message     = data[2]

                        if messageType == Parameters.OK:
                            self.recieved.append(messageId)

                        elif messageType == Parameters.TEMP:
                            self.queue.append({
                                "recipient": "MAIN",
                                "command": "RECORD_MEASUREMENT",
                                "measurement": message
                            })
                except:
                    pass
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
                data = self.ser.readline()

                try:
                    data = data.decode()
                    data = json.dumps(data)

                except JSONDecodeError as e:
                    print("Reader.run() - error decoding JSON - %s" % e)
                    break

                except UnicodeError as e:
                    print("Reader.run() - error decoding binary - %s" % e)
                    break

                except:
                    print("Reader.run() - unknown error")
                    break

                
                messageType = data["MessageType"]
                messageId   = data["MessageId"]

                # Acknowledge that the message has been recieved to prevent retries
                if messageId not in self.recieved:
                    self.recieved.append(messageId)

                # We only handle certain responses (RequestMeasurement, GetLocation)

                if messageType == "Pair":
                    self.queue.append({
                        "recipient"     : "PAIR",
                        "command"       : "Pair",
                        "insectId"      : data["data"]["insectId"] # TODO
                    })

                if messageType == "RequestMeasurement":
                    self.queue.append({
                        "recipient"     : "MAIN",
                        "command"       : "RequestMeasurement",
                        "measurement"   : data["data"]

                if messageType == "GetLocation":
                    self.queue.append({
                        "recipient"     : "MAIN",
                        "command"       : "GetLocation",
                        "location"      : data["data"]
                    )}
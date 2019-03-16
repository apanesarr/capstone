import serial
import threading
from time import sleep
import queue

import Parameters

newMessageId = 1

class Writer(threading.Thread):
    def rec(self, messageId):
        for r in self.recieved:
            if r == messageId:
                return True

        return False

    def send(self, queue, messageType, recipientId, message):
        global newMessageId

        messageId = newMessageId
        newMessageId = newMessageId + 1

        message = {
            "MessageId"     : messageId,
            "RecipientId"   : recipientId,
            "MessageType"   : messageType,
            "Data"          : message  
        }

        # Periodically check for a response and re-send if necessary
        # The outer layer is meant for the Server - the inner layer (the `message` object
        # is the layer that is meant for the insect)
        queue.put({
            "recipient"     : "COM",
            "command"       : "CHECK_RESPONSE",
            "time"          : time.time() + Parameters.RESEND_DELAY
            "messageId"     : messageId,
            "message"       : str(message).encode()
        })

        self.ser.write(message.encode())

    def resend(self, queue, item):
        message = item["message"]

        queue.put({
            "recipient"     : "COM",
            "command"       : "CHECK_RESPONSE",
            "time"          : time.time() + Parameters.RESEND_DELAY
            "messageId"     : item["messageId"],
            "message"       : message
        })

        self.ser.write(message.encode())

    def __init__(self, port, bps, queue, recieved, leep=0.01):
        self.port = port
        self.bps = bps
        self.queue = queue
        self.sleep = sleep
        self.recieved = recieved
        self.ser = serial.Serial(self.port, self.bps)

        super().__init__()

    def run(self):
        while True:
            if not self.queue.empty():
                item = self.queue.get()

                # If it"s not meant for us, put it back and continue
                if not (item["recipient"] == "COM"):
                    self.queue.put(item)

                elif (item["command"] == "CheckResponse"):
                    if not self.rec(item["messageId"]):
                        self.resend(self.queue, item)

                elif (item["command"] == "RequestMeasurement"):

                elif (item["command"] == "SetState"):

                elif (item["command"] == "GetLocation"):

                else:
                    print("ERR: Communication.py - Invalid message")
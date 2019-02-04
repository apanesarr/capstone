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

    def send(self, queue, message):
        global newMessageId

        messageId = newMessageId
        newMessageId = newMessageId + 1

        message = str(messageId) + "," + message + "\0"

        queue.put({
            "recipient": "COM",
            "command": "CHECK_RESPONSE",
            "messageId": messageId,
            "message": message
        })

        self.ser.write(message.encode())

    def resend(self, queue, item):
        message = item["message"] + "\0"

        queue.put({
            "recipient": "COM",
            "command": "CHECK_RESPONSE",
            "messageId": item["messageId"],
            "message": message
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

                elif (item["command"] == "EXIT"):
                    break

                elif (item["command"] == "CHECK_RESPONSE"):
                    if not self.rec(item["messageId"]):
                        self.resend(self.queue, item)

                elif (item["command"] == "SEND_NEW_LOCATION"):
                    message = "%i,%s,%i" % (1, Parameters.CMD_SET_GYRO, item["angle"])

                    self.send(self.queue, message)

                elif (item["command"] == "MOTION_STOP"):
                    message = "%i,%s" % (1, Parameters.CMD_STOP_MOTOR)

                    self.send(self.queue, message)

                elif (item["command"] == "MOTION_START"):
                    message = "%i,%s" % (1, Parameters.CMD_START_MOTOR)

                    self.send(self.queue, message)

                elif (item["command"] == "GET_TEMP"):
                    message = "%i,%s" % (1, Parameters.CMD_GET_TEMP)

                    self.send(self.queue, message)

                else:
                    print("ERR: Communication.py - Invalid message")
import time
import serial
import json

import Parameters
import Location

awaitingResponse    = []
recievedMessages    = []
newMessageId        = 1

class Communication:
    # Reset the buffer to switch between reading and writing
    def rb(self):
        self.serial_in.close()
        self.serial_in.open()

        self.serial_out.close()
        self.serial_out.open()

    def send(self, queue, messageType, message):
        global newMessageId

        messageId = newMessageId
        newMessageId = newMessageId + 1

        message = {
            "MessageId"     : messageId,
            "RecipientId"   : 1,
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
            "message"       : message
        })

        self.serial_out.write(message.encode())

    def resend(self, queue, item):
        message = item["message"] + "\r\n"

        queue.put({
            "recipient"     : "COM",
            "command"       : "CHECK_RESPONSE",
            "messageId"     : item["messageId"],
            "message"       : message
        })

        self.serial_out.write(message.encode())

    def recieved(self, messageId):
        for r in recievedMessages:
            if r == messageId:
                return True

        return False

    def runCommunication(self, event):
        queue = event.queue

        self.serial_in = serial.Serial(
            Parameters.SERIAL_PORT_IN,
            Parameters.SERIAL_BODE
        )

        self.serial_out = serial.Serial(
            Parameters.SERIAL_PORT_OUT,
            Parameters.SERIAL_BODE
        )

        while True:
            # Check if there are any response messages waiting for us
            if self.serial_in.inWaiting() > 0:
                data = self.serial_in.readline()

                try:
                    data = data.decode()
                    data = json.dumps(data)

                except JSONDecodeError as e:
                    print("runCommunication() - error decoding JSON - %s" % e)
                    break

                except UnicodeError as e:
                    print("runCommunication() - error decoding binary - %s" % e)
                    break

                except:
                    print("runCommunication() - unknown error")
                    break

                # Proper responses should have 3 comma seperated values
"""                 if len(data) == 3:
                    messageId   = data[0]
                    messageType = data[1]
                    message     = data[2]

                    if messageType == Parameters.OK:
                        recievedMessages.append(messageId)

                    elif messageType == Parameters.TEMP:
                        queue.append({
                            "recipient": "MAIN",
                            "command": "RECORD_MEASUREMENT",
                            "measurement": message
                        }) """

            if not queue.empty():
                item = queue.get()

                # If it"s not meant for us, put it back and continue
                if not (item["recipient"] == "COM"):
                    queue.put(item)

                elif (item["command"] == "EXIT"):
                    break

                elif (item["command"] == "CHECK_RESPONSE"):
                    if not self.recieved(item["messageId"]):
                        self.resend(queue, item)

                elif (item["command"] == "SEND_NEW_LOCATION"):
                    message = "%i,%s,%i" % (1, Parameters.CMD_SET_GYRO, item["angle"])

                    self.send(queue, message)

                elif (item["command"] == "MOTION_STOP"):
                    message = "%i,%s" % (1, Parameters.CMD_STOP_MOTOR)

                    self.send(queue, message)

                elif (item["command"] == "MOTION_START"):
                    message = "%i,%s" % (1, Parameters.CMD_START_MOTOR)

                    self.send(queue, message)

                elif (item["command"] == "GET_TEMP"):
                    message = "%i,%s" % (1, Parameters.CMD_GET_TEMP)

                    self.send(queue, message)

                else:
                    print("ERR: Communication.py - Invalid message")

            # Limit
            time.sleep(0.1)
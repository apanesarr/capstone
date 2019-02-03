import time
import serial

import Parameters

recievedMessages = []
newMessageId = 1

class Communication:
    def send(self, queue, message):
        global newMessageId

        messageId = newMessageId
        newMessageId = newMessageId + 1

        message = str(messageId) + "," + message + "\r\n"

        queue.put({
            "recipient": "COM",
            "command": "CHECK_RESPONSE",
            "messageId": messageId,
            "message": message
        })

        print(message.encode())

        self.serial.write(message.encode())
        print(self.serial.readline())

    def resend(self, queue, item):
        message = item["message"] + "\r\n"

        queue.put({
            "recipient": "COM",
            "command": "CHECK_RESPONSE",
            "messageId": item["messageId"],
            "message": message
        })

        self.serial.write(message.encode())

        print(self.serial.readline())

    def recieved(self, messageId):
        for r in recievedMessages:
            if r == messageId:
                return True

        return False

    def runCommunication(self, event):
        queue = event.queue

        self.serial = serial.Serial(
            Parameters.SERIAL_PORT_IN,
            Parameters.SERIAL_BODE,
        )

        self.serial.reset_input_buffer()
        self.serial.reset_output_buffer()

        self.serial.write('testeroo'.encode())

        while True:
            # Check if there are any response messages waiting for us
            print(str(self.serial.inWaiting()))
            if self.serial.inWaiting() > 0:
                data = self.serial.readline()

                print(data)

                # data = data.decode("utf-8").replace("\r\n", "")

                # We got an "OK" response, so add it to
                # the list of responses we"ve recieved
                # message_id  = int(data)
                message_id = -1
                if message_id > 0:
                    recievedMessage.append(message_id)

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
                    message = str(1) + "," + "S" + "," + str(item["location"][0]) + str(item["location"][1])
                    self.send(queue, message)

                elif (item["command"] == "MOTION_STOP"):
                    print()
                    # Stop moving the insect

                else:
                    print("ERR: Communication.py - Invalid message")

            # Limit
            time.sleep(0.1)
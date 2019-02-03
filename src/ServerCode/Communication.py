import time
import serial

import Parameters

recievedMessages = []
newMessageId = 1

def send(serial, queue, message):
    messageId = newMessageId
    newMessageId = newMessageId + 1

    message = str(messageId) + "," + message

    queue.put({
        "recipient": "COM",
        "command": "CHECK_RESPONSE",
        "messageId": messageId
    })

    serial.write(message.encode())

def recieved(messageId):
    for r in recievedMessages:
        if r == messageId:
            return True

    return False

def runCommunication(event):
    queue = event.queue

    ser = serial.Serial(
        Parameters.SERIAL_PORT,
        Parameters.SERIAL_BODE,
        timeout=Parameters.SERIAL_TIMEOUT
    )

    while True:
        # Check if there are any response messages waiting for us
        if ser.inWaiting() > 0:
            data = ser.readline()

            print(data)

            data = data.decode("utf-8").replace("\r\n", "")

            # We got an "OK" response, so add it to
            # the list of responses we"ve recieved
            message_id  = int(data)

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
                if not recieved(item["messageId"]):
                    send(ser, queue, item["message"])

            elif (item["command"] == "SEND_NEW_LOCATION"):
                message = str(1) + "," + "S" + "," + str(item["location"][0]) + str(item["location"][1])
                send(ser, queue, message)

            elif (item["command"] == "MOTION_STOP"):
                print()
                # Stop moving the insect

            else:
                print("ERR: Communication.py - Invalid message")

        # Limit
        time.sleep(0.1)
import time
import serial

import Parameters

recievedMessages = []
newMessageId = 1

def send(ser, queue, message,):
    global newMessageId

    messageId = newMessageId
    newMessageId = newMessageId + 1

    message = str(messageId) + "," + message

    queue.put({
        "recipient": "COM",
        "command": "CHECK_RESPONSE",
        "messageId": messageId,
        "message": message
    })

    ser.write(message.encode())

    print(('Wrote: ' + message).encode())

def resend(ser, queue, item):
    message = item["message"]

    queue.put({
        "recipient": "COM",
        "command": "CHECK_RESPONSE",
        "messageId": item["messageId"],
        "message": message
    })

    ser.write(message.encode())

def recieved(messageId):
    for r in recievedMessages:
        if r == messageId:
            return True

    return False

def runCommunication(event):
    queue = event.queue

    ser_out = serial.Serial(
        Parameters.SERIAL_PORT_OUT,
        Parameters.SERIAL_BODE,
        timeout=Parameters.SERIAL_TIMEOUT
    )

    ser_in = serial.Serial(
        Parameters.SERIAL_PORT_IN,
        Parameters.SERIAL_BODE,
        timeout=Parameters.SERIAL_TIMEOUT
    )

    ser_out.close()
    ser_out.open()

    ser_in.close()
    ser_in.open()

    while True:
        # Check if there are any response messages waiting for us
        print(str(ser_in.in_waiting))
        if ser_in.in_waiting > 0:
            print('hi')
            data = ser.readline()
            print('hi2')
            print('READING: ' + data)

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
                    resend(ser_out, queue, item)

            elif (item["command"] == "SEND_NEW_LOCATION"):
                message = str(1) + "," + "S" + "," + str(item["location"][0]) + str(item["location"][1])
                send(ser_out, queue, message)

            elif (item["command"] == "MOTION_STOP"):
                print()
                # Stop moving the insect

            else:
                print("ERR: Communication.py - Invalid message")

        # Limit
        time.sleep(0.1)
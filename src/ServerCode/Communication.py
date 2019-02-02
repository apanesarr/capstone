import time
import serial

recievedMessage = []

def runCommunication(event):
    running = True

    ser = serial.Serial('/dev/tty.usbmodem14201', 115200, timeout=100)

    queue = event.queue
    while running:
        if ser.inWaiting() > 0:
            data = ser.readline()

            data = data
                .decode('utf-8')
                .replace('\r', '')
                .replace('\n', '')

            # We got an 'OK' response, so add it to
            # the list of responses we've recieved
            message_id  = int(data)

            if message_id > 0:
                recievedMessage.append(message_id)

        if not queue.empty():
            item = queue.get()

            # If it's not meant for us, put it back and continue
            if not (item.recipient == 'COM'):
                queue.put(item)

            elif (item.command == 'SEND_NEW_LOCATION'):
                print()
                # Send the insect a new location

            elif (item.command == 'MOTION_STOP'):
                print()
                # Stop moving the insect

            else:
                print('ERR: Communication.py - Invalid message')

        # Limit
        time.sleep(0.1)
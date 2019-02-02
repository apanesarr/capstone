def runCommunication(event):
    running = True

    queue = event.queue
    while running:
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
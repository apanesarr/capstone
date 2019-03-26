import time
import Parameters

from Insect import Insect
from Location import Location

import asyncio
import websockets
import json

measurements    = []                    # List of recieved measurement data
recieved        = []                    # List of recieved messages
insects         = []

loc_service = Location(measurements) # TODO

print("Initialization complete")

print("Starting surveying")

async def run(websocket, path):
    async for message in websocket:
        try:
            message = json.loads(message)

            messageType = message['MessageType']
            recipientId = message['RecipientId']

            if messageType == 'I':
                ins = Insect(recipientId)
                insects.append(ins)

                next = loc_service.nextState(ins)

                await websocket.send(json.dumps({
                    'MessageType': 'I',
                    'RecipientId': recipientId,
                    'Data': {}
                }))

                print('Insect added with ID: ')
                print(recipientId)

            elif messageType == 'T':
                # TODO - save measurement
                pass
            
            elif messageType == 'R':
                next = loc_service.nextState(loc_service.getInsect(insects, recipientId))

                await websocket.send(json.dumps({
                    'MessageType': 'M',
                    'RecipientId': recipientId,
                    'Data': next
                }))

        except Exception as e:
            print('Exception in run()')
            print(e)

""" 
    # Main loop
    while (not location.surveyComplete()) and time.time() < Parameters.SURVEY_TIME:
        handleComEvent(queue_reader)

        # Set new targets for insects that aren't doing anything
        for insect in insects:
            if not insect.hasTarget:
                queue_writer.put({
                    "recipient"     : "COM",
                    "command"       : "SetState",
                    "state"         : location.nextState(insect)
                })

        # if the "q" key is pressed, stop the loop
        if key == ord("q"):
            break

        time.sleep(0.2)
 """

start = websockets.serve(run, Parameters.SOCKET_HOST, Parameters.SOCKET_PORT)

asyncio.get_event_loop().run_until_complete(start)
asyncio.get_event_loop().run_forever()
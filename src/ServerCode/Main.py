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

loc_service = Location(measurements)

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

                measurements.append({
                    
                }) 
                pass
            
            elif messageType == 'R':
                ins = loc_service.getInsect(insects, recipientId)

                next = loc_service.nextState(ins)

                ins.hasTarget = True
                ins.currentLocation = target
                ins.target = next

                await websocket.send(json.dumps({
                    'MessageType': 'M',
                    'RecipientId': recipientId,
                    'Data': next
                }))
            
            elif messageType == 'SIM':
                await websocket.send(json.dumps({
                    'MessageType': 'SIM',
                    'Data': measurements
                }))

        except Exception as e:
            print('Exception in run()')
            print(e)

start = websockets.serve(run, Parameters.SOCKET_HOST, Parameters.SOCKET_PORT, ping_timeout=999, close_timeout = 999)

asyncio.get_event_loop().run_until_complete(start)
asyncio.get_event_loop().run_forever()
import time
import Parameters

import sys, os

from Insect import Insect
from Location import Location

import asyncio
import websockets
import json

import random

measurements    = []                    # List of recieved measurement data
recieved        = []                    # List of recieved messages
insects         = []

loc_service = Location(measurements)

fakeMeasurements = [{'Location': (0, 0), 'Temperature': 22, 'Humidity': 45}, {'Location': (0, 0), 'Temperature': 22, 'Humidity': 45}, {'Location': (0, 0), 'Temperature': 22, 'Humidity': 45}]

VERBOSE = True

print("Initialization complete")

print("Starting surveying")

def randomize(value):
    return value + random.randint(-Parameters.RANDOM_RANGE, Parameters.RANDOM_RANGE)

async def run(websocket, path):
    async for message in websocket:
        try:
            if VERBOSE:
                print("Recieved: %s" % message)

            message = json.loads(message)

            messageType = message['MessageType']
            recipientId = message['RecipientId']

            if messageType == 'I':
                insects.append(Insect(recipientId))

                output = json.dumps({
                    'MessageType': 'I',
                    'RecipientId': recipientId,
                    'Data': {}
                })

                await websocket.send(output)

                print('Insect added with ID: ')
                print(recipientId)

                if VERBOSE:
                    print("Sent: %s" % output)

            elif messageType == 'T':
                ins = loc_service.getInsect(insects, recipientId)

                ins.currentLocation = (message['Data']['X'], message['Data']['Y'])
                ins.angle = message['Data']['Angle']

                measurements.append({
                    'Location': ins.currentLocation,
                    'Temperature': randomize(message['Data']['Temperature']),
                    'Humidity': randomize(message['Data']['Humidity'])
                })

                print("Measurements:")
                print(measurements)

                data = message['Data']
                X    = data['X']
                Y    = data['Y']

                loc_service.measurementMade(X, Y)

                if VERBOSE:
                    print('Measurement taken at: ')
                    print(ins.currentLocation)

                next = loc_service.nextState(ins)

                output = json.dumps({
                    'MessageType': 'M',
                    'RecipientId': recipientId,
                    'Data': next
                })

                await websocket.send(output)

                if VERBOSE:
                    print("Sent: %s" % output)
            
            elif messageType == 'R':
                ins = loc_service.getInsect(insects, recipientId)

                ins.currentLocation = (message['Data']['X'], message['Data']['Y'])
                ins.angle = message['Data']['Angle']

                if ins.arrived():
                    ins.hasTarget = False

                    output = json.dumps({
                        'MessageType': 'T',
                        'RecipientId': recipientId,
                        'Data': {}
                    })

                    await websocket.send(output)

                    if VERBOSE:
                        print("Sent: ")
                        print(output)

                else:
                    next = loc_service.nextState(ins)

                    output = json.dumps({
                        'MessageType': 'M',
                        'RecipientId': recipientId,
                        'Data': next
                    })

                    await websocket.send(output)

                    if VERBOSE:
                        print("Sent: %s" % output)
            
            elif messageType == 'SIM':
                print("Recieveing SIM message")

                await websocket.send(json.dumps({
                    'MessageType': 'SIM',
                    'Data': measurements # fakeMeasurements
                }))

        except Exception as e:
            exc_type, exc_obj, exc_tb = sys.exc_info()
            fname = os.path.split(exc_tb.tb_frame.f_code.co_filename)[1]
            print(exc_type, fname, exc_tb.tb_lineno)
            print(e)

start = websockets.serve(run, Parameters.SOCKET_HOST, Parameters.SOCKET_PORT, ping_interval=10, ping_timeout=1000)

asyncio.get_event_loop().run_until_complete(start)
asyncio.get_event_loop().run_forever()
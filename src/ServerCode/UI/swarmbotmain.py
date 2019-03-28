# -*- coding: utf-8 -*-
"""
Created on Tue Mar 19 12:08:25 2019

@author: nishanth
"""
from flask import Flask
from flask import render_template
from flask_socketio import SocketIO
from json_cleaner import clean_json
import websockets
import asyncio
import time

app = Flask(__name__)
#app.config['SECRET_KEY'] = 'secret!'
socketio = SocketIO(app)

@app.route('/')
def hello_world():
    return render_template('index.html')

@app.route('/simulation')
def start_simulation():
    #handle_json(data)
    return render_template('simulation.html')
    #async with websockets.connect(host_ip) as websocket:
        #data = await websocket.recv()
        #time.sleep(5000)
    #asyncio.get_event_loop().run_forever()
    

@socketio.on('json')
def handle_json(json):
    print('received json: ' + str(json))
    x = clean_json(json)
    print(x)

#start_server = websockets.serve(start_simulation, 'localhost', 5000)
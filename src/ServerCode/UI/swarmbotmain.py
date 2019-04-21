# -*- coding: utf-8 -*-
"""
Created on Tue Mar 19 12:08:25 2019

@author: nishanth
"""
from flask import Flask
from flask import render_template
from websocket import create_connection
import json


app = Flask(__name__)

websocket_conn = "ws://192.168.43.91:8095"
data_string = ""

@app.route('/')
def hello_world():
    return render_template('index.html')

@app.route('/simulation')
def start_simulation():
    communicate()
    return render_template('simulation.html')
    
def communicate():
    sendata = json.dumps({
            "MessageType": "SIM",
            "RecipientId": "-1",
            "Data": ""})
    ws = create_connection(websocket_conn)
    print("Sending Communication Request")
    ws.send(sendata)
    print("Sent")
    result =  ws.recv()
    print("Received '%s'" % result)
    ws.close()
    
def get_csv_data(json_data):
    parsed_data = json.loads(json_data)
    data = parsed_data["data"]
    print(data)
    data_string.append(data["Location"])
    
    

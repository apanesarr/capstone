# -*- coding: utf-8 -*-
"""
Created on Tue Mar 19 12:08:25 2019

@author: nishanth
"""
from flask import Flask
from flask import render_template
from websocket import create_connection
import json
import csv
from heatmap import draw_heatmap


app = Flask(__name__)

websocket_conn = "ws://192.168.43.91:8095"

@app.route('/')
def hello_world():
    return render_template('index.html')

@app.route('/simulation')
def start_simulation():
    #x = communicate()
    #get_csv_data(x)
    #draw_heatmap()
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
    return result
    
def get_csv_data(json_data):
    data_string = ""
    parsed_data = json.loads(json_data)
    #for i in range(20):
       # data_string.append(str(parsed_data["Data"]['Location'][i][0]))
       # data_string.append(",")
       # data_string.append(str(parsed_data["Data"]['Location'][i][1]))
       # data_string.append(",")
       # data_string.append(str(parsed_data["Data"]["Temperature"][i]))
       # data_string.append("\n")
    data_string.append(str(parsed_data["Data"]['Location'][0][0]))
    data_string.append(",")
    data_string.append(str(parsed_data["Data"]['Location'][0][1]))
    data_string.append(",")
    data_string.append(str(parsed_data["Data"]["Temperature"]))
    data_string.append("\n")
    with open('/data/example_data.csv', 'wb') as csvFile:
        writer = csv.writer(csvFile)
        writer.writerow(data_string)
    csvFile.close()
    
    

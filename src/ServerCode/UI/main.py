from flask import Flask, render_template, send_from_directory
from flask_socketio import SocketIO, emit
from fileReader import fileReader
import time, threading
import os

data = fileReader("data/example_data.csv")


# print(data.getData())

myDict = {}
static_file_dir = os.path.join(os.path.dirname(os.path.realpath(__file__)), 'static')

print(static_file_dir)

def updateData ():
	myDict = data.getData()
	print(myDict)
	threading.Timer(2,updateData).start()

app = Flask(__name__,static_url_path='')
socketio = SocketIO(app)

@app.route('/dir')
def index():
	return send_from_directory(static_file_dir,'index.html')

@app.route('/simulation')
def start_simulation():
    return render_template('simulation.html')

@socketio.on('json')
def handle_json(json):
	emit('response', data.getData())

@socketio.on('connect', namespace='/test')
def test_connect():
    emit('my response', {'data': 'Connected'})

if __name__ == '__main__':
    socketio.run(app)
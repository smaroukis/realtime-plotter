from flask import Flask, render_template
from flask_socketio import SocketIO
import time

app = Flask(__name__)
socketio = SocketIO(app)

# Edit to be the data source
# TODO - someway to grab most recent data shource
fnameData = '../app_logger/logs/9-2-12-2/log000'

SLEEP_TIME = 1

@app.route('/')
def index():
    return render_template('index.html') 

file_pos = 0

@socketio.on('connect')
def on_connect():

  while True:

    with open(fnameData) as f:
      
      # Seek to current position  
      f.seek(file_pos)  

      while True:
        line = f.readline()
        if not line:
          break
        socketio.emit('data', line)

      # Store new position  
      file_pos = f.tell()

    #   TODO - replace with threading for non blocking
    time.sleep(SLEEP_TIME)


if __name__ == '__main__':
   socketio.run(app, debug=True)
   # Default port is 5000
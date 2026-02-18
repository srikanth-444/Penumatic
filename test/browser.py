from flask import Flask, render_template
from flask_socketio import SocketIO
import threading
import time
import random  # Remove this when using real telemetry

app = Flask(__name__)
socketio = SocketIO(app, cors_allowed_origins="*")  # allows browser to connect
socketio.emit('voltage_update', {'voltage': voltage})        
time.sleep(0.1)  # update 10 Hz



# -----------------------------
# Flask route for webpage
# -----------------------------
@app.route('/')
def index():
    return render_template('index.html')

# -----------------------------
# Run Flask + SocketIO
# -----------------------------
if __name__ == '__main__':
    socketio.run(app, host='0.0.0.0', port=5000)

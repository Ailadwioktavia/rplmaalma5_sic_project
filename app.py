from dotenv import load_dotenv
load_dotenv()

from flask import Flask, jsonify, render_template
from flask_cors import CORS
from flask_socketio import SocketIO
from model import model
import paho.mqtt.client as paho, os, json, pandas as pd

data = {
    "sampah_per_jam": 0,
    "ketinggian_sampah_logam": 0,
    "ketinggian_sampah_non_logam": 0,
    "kapasitas_penuh_logam": 0,
    "kapasitas_penuh_non_logam": 0,
    "waktu_penuh_logam": 0,
    "waktu_penuh_non_logam": 0
}

app = Flask(__name__)
CORS(app)
socket = SocketIO(app)

MQTT_BROKER = os.getenv("MQTT_BROKER")

mqtt_client = paho.Client(callback_api_version=paho.CallbackAPIVersion.VERSION2)

@mqtt_client.connect_callback()
def on_connect(client, userdata, flags, rc, prop):
    print(f"{rc} to connect mqtt broker "+MQTT_BROKER+"...")
    client.subscribe("sensor")

@mqtt_client.message_callback()
def on_message(client, userdata, msg):
    global data
    payload = msg.payload.decode()
    topic = msg.topic
    if topic != "sensor":
        return
    try:
        message_data = json.loads(payload)
        data.update(message_data)
        df_logam = pd.DataFrame({
            "sampah_per_jam": [data.get("sampah_per_jam", 0)],
            "ketinggian_sampah": [data.get("ketinggian_sampah_logam", 0)],
            "kapasitas_sampah": [data.get("kapasitas_penuh_logam", 0)]
        })
        df_non_logam = pd.DataFrame({
            "sampah_per_jam": [data.get("sampah_per_jam", 0)],
            "ketinggian_sampah": [data.get("ketinggian_sampah_non_logam", 0)],
            "kapasitas_sampah": [data.get("kapasitas_penuh_non_logam", 0)]
        })
        data["waktu_penuh_logam"] = model.predict(df_logam)[0]
        data["waktu_penuh_non_logam"] = model.predict(df_non_logam)[0]

        socket.emit('update', data)
    except json.JSONDecodeError as e:
        print(e)
        print("Received non-JSON message")

mqtt_client.connect(MQTT_BROKER, 1883, 60)
mqtt_client.loop_start()

@app.route('/', methods=['GET'])
def index():
    return render_template("index.html")

@app.route('/sensor', methods=['GET'])
def get_sensor_data():
    return jsonify(data)

if __name__ == "__main__":
    socket.run(app, host="0.0.0.0", debug=True)

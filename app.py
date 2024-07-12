from flask import Flask, jsonify

app = Flask(__name__)

metal_entered = 0

@app.route('/metal', methods=['POST'])
def enter_metal():
    global metal_entered
    metal_entered += 1
    return jsonify({ 'metal_entered': metal_entered })

@app.route('/metal', methods=['GET'])
def get_entered_metal():
    global metal_entered
    return jsonify({ 'metal_entered': metal_entered })

if __name__ == "__main__":
    app.run(host="0.0.0.0", debug=True)
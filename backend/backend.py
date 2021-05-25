import json
from datetime import datetime

from flask import Flask, request

app = Flask(__name__)

data_history = []


@app.route('/', methods=['GET'])
def root():
    return str(data_history)


@app.route('/data', methods=['POST'])
def post_data():
    global data_history
    data_history.append(request.json)
    print(request)
    print(request.json)
    return 'Success', 200


@app.route('/data', methods=['PUT'])
def save_data():
    name = request.data.decode('utf-8')
    if name.isspace():
        name = 'name'

    dt_string = datetime.now().strftime("%d_%m_%Y_%H_%M_%S")
    with open(f'data/data-{dt_string}-{name}.txt', 'w', encoding='utf-8') as f:
        json.dump(data_history, f)
    print(f'Recording saved to: data-{dt_string}_{name}.txt')
    return 'Success', 200


@app.route('/data', methods=['GET'])
def delete_data():
    global data_history
    data_history = []
    print('Recording deleted.')
    return 'Success', 200


if __name__ == '__main__':
    app.run(host='0.0.0.0', port=8080, debug=True)

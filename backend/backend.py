import json
from datetime import datetime

from flask import Flask, request
from flask_cors import CORS, cross_origin

app = Flask(__name__)
cors = CORS(app, resources={r"*": {"origins": "*"}})
app.config['CORS_HEADERS'] = 'Content-Type'

data_history = []


@app.route('/', methods=['GET'])
@cross_origin()
def root():
    return str(data_history)


@app.route('/data', methods=['POST'])
@cross_origin()
def post_data():
    print('Post data called...')
    global data_history
    data_history.append(request.json)
    print(request)
    print(request.json)
    return 'Success', 200


@app.route('/data', methods=['PUT'])
@cross_origin()
def save_data():
    print('Save data called...')
    name = request.json['name']
    if len(name) < 1:
        name = 'new'

    dt_string = datetime.now().strftime("%d_%m_%Y_%H_%M_%S")
    with open(f'data/data-{dt_string}-{name}.txt', 'w', encoding='utf-8') as f:
        json.dump(data_history, f)
    print(f'Recording saved to: data-{dt_string}_{name}.txt')
    return 'Success', 200


@app.route('/data', methods=['DELETE'])
@cross_origin()
def delete_data():
    print('Delete data called...')
    global data_history
    data_history = []
    print('Recording deleted.')
    return 'Success', 200


if __name__ == '__main__':
    app.run(host='0.0.0.0', port=8080, debug=True)

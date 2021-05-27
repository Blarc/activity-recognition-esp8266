import json
from datetime import datetime

from flask import Flask, request
from flask_cors import CORS, cross_origin
from joblib import load

app = Flask(__name__)
cors = CORS(app, resources={r"*": {"origins": "*"}})
app.config['CORS_HEADERS'] = 'Content-Type'

model = load('model.joblib')

types = {
    0: 'walking',
    1: 'jumping',
    2: 'still'
}

recordings = []
prediction_result = 'happy'


@app.route('/', methods=['GET'])
@cross_origin()
def root():
    return str(recordings)


@app.route('/data', methods=['POST'])
@cross_origin()
def post_data():
    print('Post data called...')
    global recordings
    recordings.append(request.json)
    print(request)
    print(request.json)
    print(len(request.json['data']))
    return 'Success', 200


@app.route('/data', methods=['PUT'])
@cross_origin()
def save_data():
    print('Save data called...')
    name = request.json['name']
    if len(name) < 1:
        name = 'new'

    dt_string = datetime.now().strftime("%d_%m_%Y_%H_%M_%S")
    with open(f'data/data-{dt_string}-{name}.csv', 'w', encoding='utf-8') as f:
        print('x1,y1,z1,x2,y2,z2,x3,y3,z3,x4,y4,z4,x5,y5,z5,type', file=f)
        for record in recordings:
            print(*record['data'], sep=',', file=f, end=',')
            print(record['type'], file=f)

    print(f'Recording saved to: data-{dt_string}_{name}.csv')
    return 'Success', 200


@app.route('/data', methods=['DELETE'])
@cross_origin()
def delete_data():
    print('Delete data called...')
    global recordings
    recordings = []
    print('Recording deleted.')
    return 'Success', 200


@app.route('/prediction', methods=['POST'])
@cross_origin()
def prediction():
    print('Prediction called...')

    global model
    global prediction_result

    data = request.json['data']
    print(data)

    prediction_result = types[model.predict([data])[0]]

    return prediction_result, 200


@app.route('/prediction', methods=['GET'])
@cross_origin()
def get_prediction():
    global prediction_result

    return prediction_result, 200


if __name__ == '__main__':
    app.run(host='0.0.0.0', port=8080, debug=True)

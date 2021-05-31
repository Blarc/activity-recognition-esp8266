import math
from datetime import datetime
import statistics as stat

from scipy.signal import find_peaks
from scipy.stats import pearsonr

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

columns = 'x1,y1,z1,x2,y2,z2,x3,y3,z3,x4,y4,z4,x5,y5,z5,type'

columns_new = 'mean_z,' \
              'APF_x, APF_y, APF_z,' \
              'VarAPF,' \
              'STD_x, STD_y, STD_z,' \
              'RMS_x, RMS_y, RMS_z,' \
              'Corr_z_y,' \
              'MinMax_x, MinMax_y, MinMax_z,' \
              'type'


def rms(data):
    return stat.mean([x * x for x in data]) ** 1 / 2


def minmax(data):
    return max(data) - min(data)


def correlation(x, y):
    corr = pearsonr(x, y)[0]
    if math.isnan(corr):
        return 0.0

    return corr


def evaluate_data(data):
    x = data[::3]
    y = data[1::3]
    z = data[2::3]

    peaks = [len(find_peaks(x)[0]), len(find_peaks(y)[0]), len(find_peaks(z)[0])]

    return [
        stat.mean(z), \
        peaks[0], peaks[1], peaks[2], \
        stat.variance(peaks), \
        stat.stdev(x), stat.stdev(y), stat.stdev(z), \
        rms(x), rms(y), rms(z), \
        correlation(z, y), \
        minmax(x), minmax(y), minmax(z)
    ]


@app.route('/', methods=['GET'])
@cross_origin()
def root():
    return str(recordings)


@app.route('/data', methods=['POST'])
@cross_origin()
def post_data():
    print('Post data called...')

    data = request.json['data']
    activity = request.json['type']

    global recordings
    tmp = evaluate_data(data)
    tmp.append(activity)
    recordings.append(tmp)

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
        print(columns_new, file=f)
        for record in recordings:
            print(*record, sep=',', file=f)

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

    data = request.json['data']

    global model
    global prediction_result

    tmp = evaluate_data(data)
    prediction_result = types[model.predict([tmp])[0]]

    print(tmp)
    print(prediction_result)
    return prediction_result, 200


@app.route('/prediction', methods=['GET'])
@cross_origin()
def get_prediction():
    global prediction_result

    return prediction_result, 200


if __name__ == '__main__':
    app.run(host='0.0.0.0', port=8080, debug=True)

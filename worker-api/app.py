import pandas as pd
import matplotlib
from flask import Flask, jsonify, render_template
import matplotlib.pyplot as plt
from datetime import datetime
import bacadata

app = Flask(__name__)

matplotlib.use('Agg')  # Use the Agg backend

@app.route('/')
def home():
   return render_template('index.html')

@app.route('/api/data')
def getData():
    # Existing MongoDB query code
    dataArray = bacadata.get_data()

    # Return data as JSON response
    return jsonify(dataArray)

@app.route('/chart')
def chart():
    df = pd.DataFrame(bacadata.get_data())

    # Convert the timestamp column to datetime
    df['timestamp'] = pd.to_datetime(df['timestamp'], unit='s')

    # Create the Matplotlib chart
    plt.figure(figsize=(8, 4))  # Set the figure size
    plt.plot(df['timestamp'], df['current'], label='Arus', linestyle='-')
    plt.xlabel('Waktu')
    plt.ylabel('Arus (A)')
    plt.title('Arus Listrik Motor 01')
    plt.legend()

    # Save the chart as an image file (e.g., PNG)
    chart_filename = 'static/chart.png'
    plt.savefig(chart_filename)

    return render_template('motor1.html', chart_filename=chart_filename)

if __name__ == '__main__':
    app.run(host='0.0.0.0', debug=True)

from flask import Flask, render_template, jsonify, request
import sqlite3
import sys
import datetime
import pytz

db_path = '/home/ec2-user/App/Sensor_Database/patic2.db'


app = Flask(__name__)

@app.route('/')
def home():
    return render_template('index.php')

@app.route('/sensor_send_data', methods=['POST'])
def sensor_send():
    values = request.data
    print(values)
    a=str(request.values.get('LuzSolar'))
    luz= a.split(";")[0]
    humedadTierra= a.split(";")[1].split("=")[1]
    temperatura= a.split(";")[2].split("=")[1]
    humedadAmbiental= a.split(";")[3].split("=")[1]
    fecha = a.split(";")[4].split("=")[1]
    na = 'na'
    entry = (fecha, humedadTierra, luz, temperatura, humedadAmbiental, na)
    con = sqlite3.connect(db_path)
    cur = con.cursor()
    cur.execute("INSERT INTO datosSensor (fecha, humedadTierra, luzSolar, temperatura, humedadAmbiente, presion) VALUES(?,?,?,?,?,?)", entry)
    con.commit()
    con.close()
    return "ok",201

@app.route('/mostrar', methods=['POST'])
def mostrar():
    sensorTime = []
    sensorTierra = []
    sensorLuz = []
    sensorTemperatura = []
    sensorHumedad = []
    con = sqlite3.connect(db_path)
    curs = con.cursor()
    for fila in curs.execute("SELECT * FROM datosSensor"):
        sensorTime.append(fila[1])
        sensorTierra.append(fila[2])
        sensorLuz.append(fila[3])
        sensorTemperatura.append(fila[4])
        sensorHumedad.append(fila[5])
    con.close()

    return jsonify({'sensorTime' : sensorTime[-1], 'sensorTierra' : sensorTierra[-1], 'sensorLuz' : sensorLuz[-1], 'sensorTemperatura' : sensorTemperatura[-1], 'sensorHumedad' : sensorHumedad[-1]})
@app.route('/sensor_get_data', methods=['GET'])
def esp_get():
    tz = pytz.timezone('America/Bogota')
    now_bogota = datetime.datetime.now(tz)
    fecha = str(now_bogota.day) + '/' + str(now_bogota.month) + '/' + str(now_bogota.year)
    if(now_bogota.hour<=15 and now_bogota.hour>=6):
        tiempo = 0
        print('dia')
        return '0'
    else:
        con = sqlite3.connect(db_path)
        curs = con.cursor()
        numero = 0
        for fila in curs.execute("SELECT luzSolar FROM datosSensor WHERE fecha LIKE ?", (fecha+"%",)):
             numero = numero + int(''.join(fila))
        con.close()
        numero = 2000
        if(numero>5000):
             print(numero)
             print('suficiente')
             return '0'
        else:
             print('insuficiente')
             return '1'

if __name__ == '__main__':
        app.run(debug=True,host='0.0.0.0',port=80)



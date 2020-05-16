#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>
#include "Adafruit_VEML6070.h"
#include <SoftwareSerial.h>
#include <ESP8266WiFi.h>
#include <time.h>

// Sensor de Luz UV
Adafruit_VEML6070 uv = Adafruit_VEML6070();

// Sensor BME280
Adafruit_BME280 bme; // I2C

// Conexión con Arduino UNO
#define D7 13 //(RX)
#define D8 15 //(TX)
SoftwareSerial arduinoUNO(D7, D8);

// Constantes de Conexión WiFi
WiFiClient client;
const char* ssid = "WorkTime";
const char* password = "Alukardo";
const String server = "52.201.252.2";
const uint16_t port = 80;

// Para extraer la fecha y hora
int timezone = -5 * 3600;
int dst = 0;

// Datos de los sensores
String fecha;
int luzUV;
double humedadTierra;
double temperatura;
double humedadAmbiente;

void setup() {
  Serial.begin(9600);

  // Inicializar Sensor BME280
  Serial.println("Inicializando Sensor BME280");
  if (!bme.begin(0x76)) {
    Serial.println("Error: BME280 sensor, Check Wiring > ");
    while (1);
  }

  // Inicializar Sensor de Luz
  Serial.println("Inicializando Sensor de Luz");
  uv.begin(VEML6070_1_T);
  
  // Conexión con el ArduinoUNO
  Serial.println("Conectando con Arduino UNO");
  arduinoUNO.begin(9600);

  // Conexión a Internet
  Serial.println();
  Serial.print("Conectando a: ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("Conexión establecida.");

  // Conexión al servidor NTP para obtener la hora
  configTime(timezone, dst, "time.nist.gov", "time.windows.com", "pool.ntp.org");
  Serial.println("\nEsperando hora de Internet");

  while (!time(nullptr)) {
    Serial.print("*");
    delay(1000);
  }
  Serial.println("\nTiempo de respuesta....OK");
  enviarArduinoUNO(0);
}

void loop() {
  fecha = obtenerFecha();
  luzUV = uv.readUV();
  humedadTierra = obtenerHumedadTierra();
  temperatura = bme.readTemperature();
  humedadAmbiente = bme.readHumidity();
  enviarDatos();
  obtenerDatos();
  delay(5000);
}

// HTTP Request POST
void enviarDatos()
{
  String PostData = "";
  Serial.println("Datos para enviar: ");
  PostData = String("LuzSolar=" + String(luzUV) + "; HumedadDeLaTierra=" + String(humedadTierra) + 
  "; Temperatura=" + String(temperatura) +   "; HumedadAmbiental=" + String(humedadAmbiente) + "; Fecha=" + String(fecha));
  Serial.println(PostData);
  if (client.connect(server, port))
  {
    Serial.println("Conexión POST exitosa.");
    client.print("POST /sensor_send_data HTTP/1.1\n");
    client.print("Host: 52.201.252.2 \n");
    client.println("User-Agent: Arduino/1.0");
    client.println("Connection: close");
    client.println("Content-Type: application/x-www-form-urlencoded;");
    client.print("Content-Length: ");
    client.println(PostData.length());
    client.println();
    client.println(PostData);
  } else {
    Serial.println("Error de conexión");
  }
  client.stop();
}

// HTTP Request GET
void obtenerDatos()
{
  int GetData;
  if (client.connect(server, port))
  {
    Serial.println("Conexión GET exitosa.");
    client.print("GET /sensor_get_data HTTP/1.1\n");
    client.print(String("Host: " + server + "\n"));
    client.println("Connection: close");
    client.println("");
  } else {
    Serial.println("Error de conexión");
  }

  unsigned long timeout = millis();
  while (client.available() == 0) {
    if (millis() - timeout > 5000) {
      Serial.println(">>> Client Timeout!");
      client.stop();
      delay(30000);
      return;
    }
  }

  while (client.available()) {
    String dato = client.readStringUntil('\r');
    GetData = dato.toInt();
  }
  client.stop();
  Serial.println(GetData);
  enviarArduinoUNO(GetData);
}

String obtenerFecha() {  
  time_t now = time(nullptr);
  struct tm* p_tm = localtime(&now);

  int dia = p_tm->tm_mday;
  int mes = p_tm->tm_mon + 1;
  int anio = p_tm->tm_year + 1900;
  int hora = p_tm->tm_hour;
  int minuto = p_tm->tm_min;
  int segundo = p_tm->tm_sec;
  
  return String(String(dia) + "/" + String(mes) + "/" + String(anio) + " " + 
  String(hora) + ":" + String(minuto) + ":" + String(segundo));
}

double obtenerHumedadTierra(){
  double HT = map(analogRead(A0), 1023, 0, 0, 100);
  if(HT > 60){
    return 100;
  } else {
    return (HT*10)/6;
  }
}

double enviarArduinoUNO(int opcion){
  if(opcion == 0){
      arduinoUNO.write(1);
  } else if(opcion == 1){
      arduinoUNO.write(0);
  }
}

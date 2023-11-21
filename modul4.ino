#include <Wire.h>
#include <MPU6050_tockn.h>
#include <HTTPClient.h>
#include <WiFi.h>

MPU6050 mpu6050(Wire);

const int LDRPin = 34;
const char* ssid = "sicantikzara";
const char* password = "zarabieber";
const char* serverName = "https://db.nyanhosting.id/input.php";
const char* apiKey = "a2VhY2FudGlrYmFuZ2V0bXVhaGho";

void connectToWiFi() {
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi....");
  }
  Serial.println("Connected to WiFi");
  Serial.println("Local IP: " + WiFi.localIP().toString());
}

void analyzeLightSensor(int lightSensorValue) {
  Serial.print("Light Sensor Value: ");
  Serial.println(lightSensorValue);

  if (lightSensorValue >= 0 && lightSensorValue <= 50) {
    Serial.println("Sangat Terang");
  } else if (lightSensorValue > 50 && lightSensorValue <= 500) {
    Serial.println("Cahaya Terang");
  } else if (lightSensorValue > 500 && lightSensorValue <= 2000) {
    Serial.println("Keadaan Redup");
  } else {
    Serial.println("Keadaan Gelap");
  }
}

void analyzeMPUSensor() {
  
  Serial.print("Angle X: ");
  Serial.print(mpu6050.getAngleX());
  Serial.print("\tAngle Y: ");
  Serial.println(mpu6050.getAngleY());

  if (mpu6050.getAngleX() > 10) {
    Serial.println("Sensor Miring Kiri");
  } else if (mpu6050.getAngleX() < -10) {
    Serial.println("Sensor Miring Kanan");
  } else if (mpu6050.getAngleY() > 10) {
    Serial.println("Sensor Miring Depan");
  } else if (mpu6050.getAngleY() < -10) {
    Serial.println("Sensor Miring Belakang");
  }
}

void sendDataToServer(int lightSensorValue) {
  char url[256];
  sprintf(url, "%s?apikey=%s&angkax=%f&angkay=%f&kemiringanx=%f&kemiringany=%f&sensor=%d",
          serverName, apiKey, mpu6050.getAngleX(), mpu6050.getAngleY(), mpu6050.getAngleX(), mpu6050.getAngleY(), lightSensorValue);

  HTTPClient http;
  http.begin(url);
  int httpResponseCode = http.GET();
  
  if (httpResponseCode == 200) {
    Serial.println("Data sent successfully!");
  } else {
    Serial.print("Error code: ");
    Serial.println(httpResponseCode);
  }

  http.end();
}

void setup() {
  Serial.begin(9600);
  pinMode(LDRPin, INPUT);

  connectToWiFi();

  Wire.begin();
  mpu6050.begin();
  mpu6050.calcGyroOffsets();
}

void loop() {
  int lightValue = analogRead(LDRPin);
  int lightSensorValue = 4095 - lightValue;

  analyzeLightSensor(lightSensorValue);
  analyzeMPUSensor();
  sendDataToServer(lightSensorValue);

  delay(10000);
}

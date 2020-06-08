#include <ESP8266WiFi.h>

#include "DHT.h"
#include <SFE_BMP180.h>
#include <Wire.h>

const char* ssid = "BK_University_GUEST";           
const char* password = "";   
const char* server = "329b72368cf2.ngrok.io";
#define DHTPIN 0     
#define DHTTYPE DHT11


DHT dht(DHTPIN, DHTTYPE);
SFE_BMP180 bmp180;
int rainSensor = A0;
WiFiClient clientGet;
void setup() {
  pinMode(rainSensor, INPUT);
  Serial.begin(9600);
  Serial.println("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("DHTxx test!");
  bool success = bmp180.begin();

  if (success) {
    Serial.println("BMP180 init success");
  }

  dht.begin();
  Serial.println("Da khoi dong YL 83 xong");

}

void loop() {
  String postStr ="/predict/insert?";
  delay(30000);
  char status;
  double  T, P;
  int temp;
  int value = analogRead(rainSensor);
  float h = dht.readHumidity();
  float t = dht.readTemperature();
  
  status = bmp180.startTemperature();

    if (status != 0) {
      delay(status);
      status = bmp180.getTemperature(T);

      if (status != 0) {
        status = bmp180.startPressure(3);

        if (status != 0) {
          delay(status);
          status = bmp180.getPressure(P, T);

          if (status != 0) {
            Serial.print("Pressure: ");
            Serial.print(P);
            Serial.println(" hPa");
            
          }
        }
      }
    }
   
    // Yl-83
    if (value >= 500) { // Cảm biến đang không mưa
      Serial.println("Dang khong mua");
      temp=0;
    } else {
      Serial.println("Dang mua");
//      postStr += "&rain=";
      temp=1;
    }
    
    if (isnan(t) || isnan(h)) {
      Serial.println("Failed to read from DHT");
    } else {
      Serial.print("Humidity: ");
      Serial.print(h);
      Serial.print(" %\t");
      Serial.print("Temperature: ");
      Serial.print(t);
      Serial.println(" *C");
      
      postStr += "temperature=";
      postStr += String(t);
      postStr += "&humidity=";
      postStr += String(h);
      postStr += "&pressure=";
      postStr += P; 
      postStr += "&rain=";
      postStr += String(temp);
      Serial.println(postStr);
    }
  if (!clientGet.connect(server, 80)) {
        Serial.print("Connection failed: ");
        Serial.print(server+postStr);
    }else{
      Serial.print("ok");
      clientGet.println("GET " + postStr + " HTTP/1.1");
      clientGet.print("Host: 329b72368cf2.ngrok.io\n");
      clientGet.print("Connection: close\n");
      clientGet.print("Content-Length: ");
      clientGet.print(postStr.length());
      clientGet.print("\n\n");
      clientGet.print(postStr);
    }
  }

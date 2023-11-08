#define BLYNK_PRINT Serial
/* Fill in information from Blynk Device Info here */
#define BLYNK_TEMPLATE_ID "TMPL6rkayIYJV"
#define BLYNK_TEMPLATE_NAME "Air Quality Monitoring"
#define BLYNK_AUTH_TOKEN "-gsA9OPO1MxreftiMTCCwqpIDvTrdFwv"

#include <NTPClient.h>
#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#include "DHT.h"
#include <BlynkSimpleEsp8266.h>
#include "MQ135.h"

#define DHTPIN 4 // SDA/D2
#define DHTTYPE DHT11   // DHT 11
#define PIN_MQ135 A0 // MQ135

BlynkTimer timer;
WiFiUDP ntpUDP;
DHT dht(DHTPIN, DHTTYPE);
MQ135 mq135_sensor = MQ135(PIN_MQ135);
float humid, temp, hic; // Values from DHT11
float correctedRZero, resistance, correctedPPM; // Values from MQ135 
const char *ssid     = "FixThatBug.vice_versa"; // Wifi name
const char *password = "nofreewifi";// Wifi password
// const char *ssid     = "UIT Public";
// const char *password = "";
NTPClient timeClient(ntpUDP, "vn.pool.ntp.org", 25200, 60000); // ntpUDP, "VN", UTC7(in second), time update interval
char weekDay [7][12] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"}; // Weekday format
String realTime; // Time format

void Initial() {
  //Initial necessary components
  WiFi.begin(ssid, password); // Connect to wifi

  while ( WiFi.status() != WL_CONNECTED ) { // Waiting for wifi connection
    delay ( 500 );
    Serial.print ( "." );
  }
  Serial.println("");

  //Start components
  timeClient.begin(); // NTPTimeClient start
  dht.begin();// DHT11 start
  Blynk.begin(BLYNK_AUTH_TOKEN, ssid, password); // Connect Blynk server

  timer.setInterval(1000L, sendData); // Delay time for sending information to Blynk server
}

void getData() {
  // Get data from components
  timeClient.update(); // Update time
  Blynk.run();// Run Blynk

  String day = String(weekDay[timeClient.getDay()]);
  realTime = day + ' ' + timeClient.getFormattedTime() + '|'; // Formated datetime
  humid = dht.readHumidity(); // Read humid
  temp = dht.readTemperature(); // Read temperature as Celsius (the default)

  if (isnan(humid) || isnan(temp)) { // Check if any reads failed and exit early (to try again).
    Serial.println(F("Failed to read from DHT sensor!"));
    return;
  }

  hic = dht.computeHeatIndex(temp, humid, false); // Compute heat index in Celsius (isFahreheit = false)
  // float rzero = mq135_sensor.getRZero();
  correctedRZero = mq135_sensor.getCorrectedRZero(temp, humid); // Calculate RZero value
  resistance = mq135_sensor.getResistance(); // Resistance
  // float ppm = mq135_sensor.getPPM();
  correctedPPM = mq135_sensor.getCorrectedPPM(temp, humid); // Calcualte PPM value
}

void printData() {
  Serial.print(realTime);
  Serial.print(F("Humidity: "));
  Serial.print(humid);
  Serial.print(F("%  Temperature: "));
  Serial.print(temp);
  Serial.print(F("°C "));
  Serial.print(F("Heat index: "));
  Serial.print(hic);
  Serial.print(F("°C "));
  Serial.print("Corrected RZero: ");
  Serial.print(correctedRZero);
  Serial.print(" Resistance: ");
  Serial.print(resistance);
  Serial.print(" Corrected PPM: ");
  Serial.print(correctedPPM);
  Serial.println("ppm");
  Serial.println("");
}

void sendData() {
  //Send data to Blynk
  // Serial.println("Sent to Blynk!");
  Blynk.virtualWrite(V0, temp); // Virtual pin 0, tempurature
  Blynk.virtualWrite(V1, humid); // Virtual pin 1, humid
  Blynk.virtualWrite(V2, realTime); // Virual pin 2, datetime
  Blynk.virtualWrite(V3, correctedPPM); // Virtual pin 3, ppm
}

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600); // 9600 baud
  Initial();
}

void loop() {
  // put your main code here, to run repeatedly:
  getData();
  sendData();
  printData();
  delay(1000);
}

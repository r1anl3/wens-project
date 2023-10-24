#include <NTPClient.h>
#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#include "DHT.h"

#define DHTPIN 4 // SDA/D2
#define DHTTYPE DHT11   // DHT 11

const char *ssid     = "FixThatBug.vice_versa";
const char *password = "nofreewifi";

WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "vn.pool.ntp.org", 25200, 60000); // ntpUDP, "VN", UTC7(in second), time update interval
char weekDay [7][12] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};
String realTime, censor1;

DHT dht(DHTPIN, DHTTYPE);
char buf1[20];

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600); // 9600 baud

  WiFi.begin(ssid, password);

  while ( WiFi.status() != WL_CONNECTED ) {
    delay ( 500 );
    Serial.print ( "." );
  }

  timeClient.begin();
  dht.begin();
}

void loop() {
  // put your main code here, to run repeatedly:
  timeClient.update();

  String day = String(weekDay[timeClient.getDay()]);
  realTime = day + ' ' + timeClient.getFormattedTime() + '|';

  float h = dht.readHumidity(); // Read humit
  // Read temperature as Celsius (the default)
  float t = dht.readTemperature();
  // Read temperature as Fahrenheit (isFahrenheit = true)
  float f = dht.readTemperature(true);

  // Check if any reads failed and exit early (to try again).
  if (isnan(h) || isnan(t) || isnan(f)) {
    Serial.println(F("Failed to read from DHT sensor!"));
    return;
  }

  // Compute heat index in Fahrenheit (the default)
  float hif = dht.computeHeatIndex(f, h);
  // Compute heat index in Celsius (isFahreheit = false)
  float hic = dht.computeHeatIndex(t, h, false);

  Serial.print(realTime);
  Serial.print(F("Humidity: "));
  Serial.print(h);
  Serial.print(F("%  Temperature: "));
  Serial.print(t);
  Serial.print(F("°C "));
  // Serial.print(f);
  Serial.print(F("Heat index: "));
  Serial.print(hic);
  Serial.println(F("°C "));
  // Serial.print(hif);
  // Serial.println(F("°F"));
  delay(1000);
}

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

#define DHTPIN 4 // SDA/D2
#define DHTTYPE DHT11   // DHT 11

BlynkTimer timer;

const char *ssid     = "FixThatBug.vice_versa";
const char *password = "nofreewifi";
// const char *ssid     = "UIT Public";
// const char *password = "";

WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "vn.pool.ntp.org", 25200, 60000); // ntpUDP, "VN", UTC7(in second), time update interval
char weekDay [7][12] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};
String realTime, censor1;

DHT dht(DHTPIN, DHTTYPE);
float humid, temp, hic;


void Initial() {
  //Initial necessary components

  WiFi.begin(ssid, password);

  while ( WiFi.status() != WL_CONNECTED ) {
    delay ( 500 );
    Serial.print ( "." );
  }
  Serial.println("");

  //Start components
  timeClient.begin();
  dht.begin();
  Blynk.begin(BLYNK_AUTH_TOKEN, ssid, password);

  timer.setInterval(1000L, sendData);
}

void getData() {
  // Get data from components
  timeClient.update(); // Update time
  Blynk.run();

  String day = String(weekDay[timeClient.getDay()]);
  realTime = day + ' ' + timeClient.getFormattedTime() + '|'; // Formated datetime

  // Read humid
  humid = dht.readHumidity(); 
  // Read temperature as Celsius (the default)
  temp = dht.readTemperature();

  // Check if any reads failed and exit early (to try again).
  if (isnan(humid) || isnan(temp)) {
    Serial.println(F("Failed to read from DHT sensor!"));
    return;
  }

  // Compute heat index in Celsius (isFahreheit = false)
  hic = dht.computeHeatIndex(temp, humid, false);
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
  Serial.println("");
}

void sendData() {
  //Send data to Blynk
  // Serial.println("Sent to Blynk!");
  Blynk.virtualWrite(V0, temp);
  Blynk.virtualWrite(V1, humid);
  Blynk.virtualWrite(V2, realTime);
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

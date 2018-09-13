#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <DHT_U.h>
#include <SoftwareSerial.h>
#define RX 10
#define TX 11

#define DHTPIN            7         // Pin which is connected to the DHT sensor.
#define DHTTYPE           DHT11    
String AP = "xxxxxxxxx";      
String PASS = "xxxxxxx"; 
String HOST = "yoururl";
String uri = "/endpoints/lpg/lpgconsumptiondata/";
String PORT = "80";
String data;
int temp;
int hum;
int countTrueCommand;
int countTimeCommand; 
boolean found = false; 
int valSensor = 1;
int smokeA0 = A5;
// Your threshold value
int sensorThres = 2;

DHT_Unified dht(DHTPIN, DHTTYPE);
SoftwareSerial esp8266(RX,TX); 
uint32_t delayMS;

void setup() {
  pinMode(smokeA0, INPUT);
  Serial.begin(9600); 
  esp8266.begin(9600);
  dht.begin();
  sendCommand("AT",5,"OK");
  sendCommand("AT+CWMODE=1",5,"OK");
  sendCommand("AT+CWJAP=\""+ AP +"\",\""+ PASS +"\"",50,"OK");
  Serial.println("DHTxx Unified Sensor Example");
  
  // Print temperature sensor details.
  sensor_t sensor;
  dht.temperature().getSensor(&sensor);
  Serial.println("------------------------------------");
  Serial.println("Temperature");
  Serial.print  ("Sensor:       "); Serial.println(sensor.name);
  Serial.print  ("Driver Ver:   "); Serial.println(sensor.version);
  Serial.print  ("Unique ID:    "); Serial.println(sensor.sensor_id);
  Serial.print  ("Max Value:    "); Serial.print(sensor.max_value); Serial.println(" *C");
  Serial.print  ("Min Value:    "); Serial.print(sensor.min_value); Serial.println(" *C");
  Serial.print  ("Resolution:   "); Serial.print(sensor.resolution); Serial.println(" *C");  
  Serial.println("------------------------------------");
  // Print humidity sensor details.
  dht.humidity().getSensor(&sensor);
  Serial.println("------------------------------------");
  Serial.println("Humidity");
  Serial.print  ("Sensor:       "); Serial.println(sensor.name);
  Serial.print  ("Driver Ver:   "); Serial.println(sensor.version);
  Serial.print  ("Unique ID:    "); Serial.println(sensor.sensor_id);
  Serial.print  ("Max Value:    "); Serial.print(sensor.max_value); Serial.println("%");
  Serial.print  ("Min Value:    "); Serial.print(sensor.min_value); Serial.println("%");
  Serial.print  ("Resolution:   "); Serial.print(sensor.resolution); Serial.println("%");  
  Serial.println("------------------------------------");
  // Set delay between sensor readings based on sensor details.
  delayMS = sensor.min_delay / 1000;
}

void loop() {
  // Delay between measurements.
  delay(delayMS);
  // Get temperature event and print its value.
  sensors_event_t event;  
  dht.temperature().getEvent(&event);
  if (isnan(event.temperature)) {
    Serial.println("Error reading temperature!");
  }
  else {
    Serial.print("Temperature: ");
    temp = event.temperature;
    Serial.print(temp);
    Serial.println(" *C");
    
  }
  // Get humidity event and print its value.
  dht.humidity().getEvent(&event);
  if (isnan(event.relative_humidity)) {
    Serial.println("Error reading humidity!");
  }
  else {
    Serial.print("Humidity: ");
    hum = event.relative_humidity;
    Serial.print(hum);
    Serial.println("%");
   
  }

  sendData(String(temp), String(hum));
}

void sendData(String temp, String hum){

    String input ="{\"temp\":\""+temp+"\",\"hum\":\""+hum+"\"}";
   //root.printTo(Serial);
   String postRequest ="POST " + uri + " HTTP/1.0\r\n" +
   "Host: " + HOST + "\r\n" +
   "Accept: *" + "/" + "*\r\n" +
   "Content-Length: " + input.length() + "\r\n" +
   "Content-Type: application/json\r\n" +
   "\r\n" + input;
   
   sendCommand("AT+CIPMUX=1",5,"OK");
   sendCommand("AT+CIPSTART=0,\"TCP\",\""+ HOST2 +"\","+ PORT,15,"OK");
   sendCommand("AT+CIPSEND=0," +String(postRequest.length()+4),4,">");
   esp8266.println(postRequest);
   delay(1500);
   countTrueCommand++;
   sendCommand("AT+CIPCLOSE=0",5,"OK");
}

void sendCommand(String command, int maxTime, char readReplay[]) {
  Serial.print(countTrueCommand);
  Serial.print(". at command => ");
  Serial.print(command);
  Serial.print(" ");
  while(countTimeCommand < (maxTime*1))
  {
    esp8266.println(command);//at+cipsend
    if(esp8266.find(readReplay))//ok
    {
      found = true;
      break;
    }
  
    countTimeCommand++;
  }
  
  if(found == true)
  {
    Serial.println("OK");
    countTrueCommand++;
    countTimeCommand = 0;
  }
  
  if(found == false)
  {
    Serial.println("Fail");
    countTrueCommand = 0;
    countTimeCommand = 0;
  }
  
  found = false;
 }

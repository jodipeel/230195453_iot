//include all relevant external files or directories etc
#include "WiFiS3.h";
#include "secrets.h";

#include <ThingSpeak.h>

WiFiClient client;

//initialise wifi name and password from the secrets file
char SSID[] = SECRET_SSID;
char pass[] = SECRET_PASS;
int status = WL_IDLE_STATUS;

//initialise the microphone values needed and necessary pin
int analogVal = 0;
int analogPin = A1;
int baseVal;

//initialise pir values and pin
int PIRPin = 2;
int state = LOW;
int val = 1;


void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);  //use the serial to check what is happening during testing stages
  ConnectWifi();   //call the wificonnect function to connect the arduino to wifi
  ThingSpeak.begin(client);     //start thingspeak communications


  //initialise relevant pins
  pinMode(analogPin, INPUT);
  baseVal = analogRead(analogPin);

  pinMode(PIRPin, INPUT);

}

void loop() {
  // put your main code here, to run repeatedly:

  //delay(10000);
  //PrintNetwork();

  //code for getting analogue values from microphone
  analogVal = abs(analogRead(analogPin) - baseVal);
  analogVal = abs(analogVal);
  Serial.print("Analog Value: ");
  Serial.println(analogVal);


  //if statements for setting correct pir values, 2 for motion, 1 for no motion
  if (digitalRead(PIRPin) == HIGH) {
    if (state == LOW) {
      state = HIGH;
      val = 2;
    }
    Serial.println("Motion deteced!");
    delay(200);
  }
  else{
    if (state == HIGH) {
      state = LOW;
      val = 1;
    }
    Serial.println("No motion detected.");
    delay(200);
  }


  SendData2TS(analogVal, 1, val, 2);  //send data to thingspeak, both channels in one
  delay(15000);   //15 second delay since thingspeak has a limit

}

void ConnectWifi() {
  // for connecting the arduino to wifi

  //checking arduino has wifi capabilities
  if (WiFi.status() == WL_NO_MODULE) {
    Serial.println("Communications with module failed.");
    while(true);
  }

  //checking firmware is up to date
  String firmVer = WiFi.firmwareVersion();
  if (firmVer < WIFI_FIRMWARE_LATEST_VERSION) {
    Serial.println("Please update your firmware.");
  }

  //connection attempt
  while (status != WL_CONNECTED) {
    Serial.print("Attempting connection with ");
    Serial.println(SSID);
    //connect
    status = WiFi.begin(SSID, pass);

    delay(10000);
  }

  Serial.println("WiFi Connected.");
  PrintNetwork();

}

void PrintNetwork() {
  //to get information about wifi connection for testing and checking
  Serial.print("WiFi Status: ");
  Serial.println(WiFi.status());

  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());

  IPAddress ip = WiFi.localIP();
  Serial.print("IP Address: ");
  Serial.println(ip);

}

void SendData2TS(int data1, int field1, int data2, int field2) {
  //function for initialising data to send to thingspeak channel, and then sending it
  unsigned long chNum = SECRET_CH_ID;
  const char * writeAPIKey = SECRET_WRITE_APIKEY;
  //int x = ThingSpeak.writeField(chNum, field, data, writeAPIKey);  --> for updating one field, but this system needs two updates
  ThingSpeak.setField(field1, data1);
  ThingSpeak.setField(field2, data2);

  int x = ThingSpeak.writeFields(chNum, writeAPIKey);

  //checking ts was updates using success codes
  if (x == 200) {
    Serial.println("Channel update success.");
  } else {
    Serial.println("Something went wrong. HTTP error code " + String(x));
  }
}

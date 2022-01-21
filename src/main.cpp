#include <Arduino.h>
#include <WiFi.h>
#include <WiFiUdp.h>
#include <esp_wifi.h>

#define LED_PIN 33

// default relay coms channel is 7 (from 0 - 14)
#ifndef CHANNEL
    #define CHANNEL 7
#endif

// default relay node id should start with "RELAY_NODE" to get recognised by network
#ifndef NODE_ID
	#define NODE_ID "RELAY_NODE_1"
#endif

// default password for the relay network
#ifndef PASS
	#define PASS "DEFAULT_PASS"
#endif

// default encryption will be turned off
#ifndef ENCRYPTION
	#define ENCRYPTION 0
#endif

// my access point settings 
const char* ssid = "BESKOVNIK_BAJTA";
const char* password = "JojKakSmoZejni!";
// esp access point settings
const char* rSsid = "eps32-1";
const char* rPassword = "123456789!";
// udp wifi
WiFiUDP udp;
// esp access point
int aps;

void setup() {
	// set the pin 33 to be output
	pinMode(LED_PIN, OUTPUT);
	// write digital high to output
	digitalWrite(LED_PIN, HIGH);

	// start the serial connection
	Serial.begin(115200);
	Serial.println("Serial connected");

	WiFi.mode(WIFI_AP_STA);
	WiFi.begin(ssid, password);

	// connect to our wifi
	while(WiFi.status() != WL_CONNECTED)
	{
		delay(500);
		Serial.print(".");
	}

	// get the ip we got
	Serial.println("Connected to wifi");
	Serial.print("Local IP: ");
	Serial.println(WiFi.localIP());


	// setup internal access point in esp
	// set the wifi mode to AP (access point)
	WiFi.mode(WIFI_AP);
	// config for LR mode (long range)
	int aps = esp_wifi_set_protocol(WIFI_IF_AP, WIFI_PROTOCOL_LR);
	if(aps != ESP_OK)
		Serial.println("ESP access point was not properly setup!");
	// setup the access point with ssid and password
	WiFi.softAP(rSsid, rPassword);
	Serial.print("Router IP: ");
	Serial.println(WiFi.softAPIP());
	delay(1000);
	digitalWrite(LED_PIN, LOW);
	udp.begin(8888);
}

void loop() {
	// send a brodcast message on port 8888 with IP below
  	udp.beginPacket({ 192, 168, 4, 255 }, 8888);
	// the packet
	udp.write('b');

	// flash the LED
	digitalWrite(LED_PIN, !digitalRead(LED_PIN));

	if(!udp.endPacket())
	{
		Serial.println("Packet was not sent!");
		delay(100);
		ESP.restart();
	}else{
		Serial.println("Packet sent!");
	};

	delay(1000);
}
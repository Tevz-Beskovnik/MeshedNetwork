#include <Arduino.h>
#include <WiFi.h>
#include <WiFiUdp.h>
#include <esp_wifi.h>
#include "../lib/NODE/NODE.h"

// led pin on message recieve
#define LED_PIN_MESSAGE_RCV 18

// led pin on message send
#define LED_PIN_MESSAGE_SND 22

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

// max connections the node can have default is 20 can be lowered if necessary
#ifndef MAX_CONNECTIONS
	#define MAX_CONNECTIONS 20
#endif

void sendFunc(const uint8_t *mac_addr, esp_now_send_status_t status)
{
	if(status = esp_now_send_status_t::ESP_NOW_SEND_SUCCESS)
		Serial.println("send successful");  
	else
		Serial.println("send not successful"); 
	digitalWrite(LED_PIN_MESSAGE_SND, !digitalRead(LED_PIN_MESSAGE_SND));
}

void recieveFunc(const uint8_t *mac_addr, const uint8_t *data, int data_len)
{
	Serial.println("recieved a message");
	digitalWrite(LED_PIN_MESSAGE_RCV, !digitalRead(LED_PIN_MESSAGE_RCV));
}

void setup() 
{
	pinMode(LED_PIN_MESSAGE_RCV, OUTPUT);
	pinMode(LED_PIN_MESSAGE_SND, OUTPUT);

	digitalWrite(LED_PIN_MESSAGE_RCV, LOW);
	digitalWrite(LED_PIN_MESSAGE_SND, LOW);


}

void loop() 
{
	
}
/*
    @ Name: ESP-NOW mashed network
    @ Author: Tevž Beškovnik
    @ Date: 17. 1. 2022
    @ Description: Meshed network of ESP32 devices over the connectionless ESP-NOW network
    @ Notes:
        - This uses wifi routers to auto discover other devices and pair with them while this is 
          handy and good this is not the best way to discover other devices, there could be 3rd. party devices
          protending to be part of the network
        - The data that can be recieved in one message as far as i can understand can have a length from anywhere
          from 0 to 250 bytes, so we should have a very optimised way to send messages or some sort of packet method

*/

#include <Arduino.h>
#include <esp_wifi.h>
#include <esp_now.h>
#include <esp_err.h>
#include <string>
#include <WiFi.h>

class NODE{
    public:
        // this initialises all the things for esp-now, access point stuff, etc...
        NODE(uint8_t* primaryKey, const uint8_t channel, const char* SSID, const char* PASS, bool encrypt);

        // this also initis just doesnt take the primary key.
        NODE(const uint8_t channel, const char* SSID, const char* PASS, bool encrypt);

        ~NODE();

        // get the current primary key for esp-now
        uint8_t get_pmk();

        // set new primary key
        void set_pmk(uint8_t* pmk);

        // get current channel
        uint8_t get_channel();

        // set new channel
        void set_channel(const uint8_t channel);

        // register a callback function for sending
        void register_send_cb(esp_now_send_cb_t sendFunc);

        // register a callback function for revieving messages
        void register_recieve_cb(esp_now_recv_cb_t recieveFunc);

        // this should be called in the loop, basicly just looks for new nodes and adds them to the network if they are found
        void search_new_nodes();

    private:
        uint8_t channel; // message channel for ESP-NOW
        uint8_t* pmk; // primary master key
        String macAddress; // this devices mac address
        bool encrypt;
        const char* SSID; // ssid
        const char* PASS; // password
        esp_now_peer_info_t* currentNode;
        uint8_t nodesConnected;
        String connectedNodes[20];
        esp_now_peer_info_t conectedNodeInfo[20];

        // pair another node with current node
        esp_err_t pair_node(const esp_now_peer_info_t* peer);

        // check if the node specified in args is already paired
        bool check_if_node_paired(const esp_now_peer_info_t* peer);

        // clear the current node
        void clearCurrentNode();

        // turn on the router
        bool enable_discoverability();

        // searches for devices and pairs with them
        bool searchForDevices();
};
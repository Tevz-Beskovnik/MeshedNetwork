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

struct node_info_s
{
    String peerName;
    esp_now_peer_info_t peerInfo;
};

// type of function pointer to update connections when new connection is made
typedef void (*node_update_connections_cb_t)(node_info_s* nodeInfo);

class NODE{
    public:
        // this initialises all the things for esp-now, access point stuff, etc...
        NODE(uint8_t* primaryKey, const uint8_t channel, const char* SSID, const char* PASS, bool encrypt, uint8_t maxConnections);

        // this also initis just doesnt take the primary key.
        NODE(const uint8_t channel, const char* SSID, const char* PASS, bool encrypt, uint8_t maxConnections);

        ~NODE();

        // get the current primary key for esp-now
        uint8_t get_pmk();

        // get current channel
        uint8_t get_channel();

        //get the data from the connections
        void update_connections_cb(node_update_connections_cb_t updateFunc);

        // register a callback function for sending
        void register_send_cb(esp_now_send_cb_t sendFunc);

        // register a callback function for revieving messages
        void register_recieve_cb(esp_now_recv_cb_t recieveFunc);

        // this should be called in the loop, basicly just looks for new nodes and adds them to the network if they are found
        void dynamic_pair();

        // send data
        bool sendData(const uint8_t* peer_addr ,const uint8_t* data, uint8_t dataLen);

    private:
        uint8_t channel; // message channel for ESP-NOW
        uint8_t* pmk; // primary master key
        String macAddress; // this devices mac address
        bool encrypt; // if transmition is encrypted
        const char* SSID; // ssid
        const char* PASS; // password
        esp_now_peer_info_t* currentNode; // current node trying to connect to
        const uint8_t maxConnections;
        node_info_s connectedNodes[20]; // connected node info
        node_update_connections_cb_t connectionUpdate;

        // pair another node with current node
        esp_err_t pair_node(const esp_now_peer_info_t* peer);

        // check if the node specified in args is already paired
        bool check_if_node_paired(const esp_now_peer_info_t* peer);

        // clear the current node
        void clear_current_node();

        // turn on the router
        bool enable_discoverability();

        // searches for devices and pairs with them
        bool search_for_devices();
};
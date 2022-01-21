#include "NODE.h"

NODE::NODE(uint8_t* pmk, const uint8_t chnannel, const char* SSID, const char* PASS, bool encrypt = false)
    :pmk(pmk), channel(channel), PASS(PASS), SSID(SSID), nodesConnected(0), encrypt(encrypt)
{
    WiFi.mode(WIFI_MODE_APSTA);

    if(!enable_discoverability())
        Serial.println("Access point was not able to setup");

    WiFi.disconnect();

    if(esp_now_init() == ESP_OK)
    {
        Serial.println("EPS-NOW initialised");
    }
    else
    {
        Serial.println("ESP-NOW initialisation failed... Restarting...");

        ESP.restart();
    }

    if(esp_now_set_pmk(pmk) != ESP_OK)
    {
        Serial.println("Setting the PMK was unsuccesful.");
    }
}

NODE::NODE(const uint8_t channel, const char* SSID, const char* PASS, bool encrypt = false)
    :channel(channel), PASS(PASS), SSID(SSID), nodesConnected(0), encrypt(encrypt)
{
    WiFi.mode(WIFI_MODE_APSTA);

    if(!enable_discoverability())
        Serial.println("Access point was not able to setup");

    WiFi.disconnect();

    if(esp_now_init() == ESP_OK)
    {
        Serial.println("EPS-NOW initialised");
    }
    else
    {
        Serial.println("ESP-NOW initialisation failed... Restarting...");

        ESP.restart();
    }
}

NODE::~NODE()
{
    ;
}

void NODE::register_send_cb(esp_now_send_cb_t sendFunc)
{
    esp_now_register_send_cb(sendFunc);
}

void NODE::register_recieve_cb(esp_now_recv_cb_t recieveFunc)
{
    esp_now_register_recv_cb(recieveFunc);
}

uint8_t NODE::get_pmk()
{
    return *pmk;
}

void NODE::set_pmk(uint8_t* newPmk)
{
    pmk = newPmk;
    if(esp_now_set_pmk(pmk) != ESP_OK)
        Serial.println("Setting the PMK was unssucessful");
}

uint8_t NODE::get_channel()
{
    return channel;
}

void NODE::set_channel(const uint8_t newChannel)
{
    channel = newChannel;
}

bool NODE::searchForDevices()
{
    uint8_t networksFound = WiFi.scanNetworks();
    uint8_t relaysAdded = 0;
    clearCurrentNode();

    if(nodesConnected > 19) return false;

    if(networksFound = 0)
    {
        Serial.println("No networks found in scan.");
    }
    else
    {
        Serial.print("Found ");
        Serial.print(networksFound);
        Serial.println(" networks");

        for(uint8_t i = 0; i < networksFound; i++)
        {
            Serial.println("Pairing...");

            String peerSSID = WiFi.SSID(i);
            if(peerSSID.substring(0, 10) != "RELAY_NODE") continue;
            connectedNodes[nodesConnected] = peerSSID;
            uint8_t* peerMAC = WiFi.BSSID(i);
            uint8_t mac[6] = { peerMAC[5], peerMAC[4], peerMAC[3], peerMAC[2], peerMAC[1], peerMAC[0] };
            for(uint8_t j = 0; j < 6; j++)
            {
                currentNode->peer_addr[j] = mac[j];
            }
            currentNode->channel = channel;
            currentNode->encrypt = encrypt;

            if(check_if_node_paired(currentNode)) continue;

            if(pair_node(currentNode) != ESP_OK)
            {
                Serial.print("Pairing relay node ");
                Serial.print(peerSSID);
                Serial.println(" failed to complete.");
            }else{
                Serial.print("Relay node ");
                Serial.print(peerSSID);
                Serial.println(" paired!");
                nodesConnected++;
                relaysAdded++;
            }

            clearCurrentNode();
        }
    }

    return (relaysAdded > 0);
}

esp_err_t NODE::pair_node(const esp_now_peer_info_t* peer)
{
    return esp_now_add_peer((const esp_now_peer_info_t*)peer);
}

bool NODE::check_if_node_paired(const esp_now_peer_info_t* info)
{
    if(esp_now_get_peer(info->peer_addr, (esp_now_peer_info_t*)info) != ESP_OK)
        return false;
    else
        return true;
}

void NODE::clearCurrentNode()
{
    memset((void*)currentNode, 0, sizeof(esp_now_peer_info_t));
}

bool NODE::enable_discoverability()
{
    bool apSetup = WiFi.softAP(SSID, PASS);
    if(!apSetup)
        return false;
    else   
        return true;
}
#include "NODE.h"

NODE::NODE(uint8_t* pmk, const uint8_t chnannel, const char* SSID, const char* PASS, bool encrypt = false, uint8_t maxConnections)
    :pmk(pmk), channel(channel), PASS(PASS), SSID(SSID), encrypt(encrypt), maxConnections(maxConnections)
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
        Serial.println("ESP-NOW initialisation failed... Restarting ESP...");

        ESP.restart();
    }

    if(esp_now_set_pmk(pmk) != ESP_OK)
    {
        Serial.println("Setting the PMK was unsuccesful.");
    }
}

NODE::NODE(const uint8_t channel, const char* SSID, const char* PASS, bool encrypt = false, uint8_t maxConnections)
    :channel(channel), PASS(PASS), SSID(SSID), encrypt(encrypt), maxConnections(maxConnections)
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

void NODE::update_connections_cb(node_update_connections_cb_t updateFunc)
{
    connectionUpdate = updateFunc;
}

uint8_t NODE::get_pmk()
{
    return *pmk;
}

uint8_t NODE::get_channel()
{
    return channel;
}

bool NODE::sendData(const uint8_t* peer_addr, const uint8_t* data, uint8_t dataLen)
{
    if(!(dataLen < ESP_NOW_MAX_DATA_LEN) || !esp_now_is_peer_exist(peer_addr)) return;

    uint8_t* cutData = (uint8_t*)malloc(dataLen*sizeof(uint8_t));

    memcpy(cutData, data, dataLen * sizeof(uint8_t));

    esp_err_t result = esp_now_send(peer_addr, cutData, dataLen);

    free(cutData);

    if(result == ESP_OK)
        return true;
    else if(result == ESP_ERR_ESPNOW_NOT_FOUND ||result == ESP_ERR_ESPNOW_IF)
    {
        esp_now_del_peer(peer_addr);
        return false;
    }
    else return false;
}

void NODE::dynamic_pair()
{
    if(search_for_devices())
        Serial.println("Paired with a device / devices.");
}

bool NODE::search_for_devices()
{
    uint8_t networksFound = WiFi.scanNetworks();
    uint8_t relaysAdded = 0;
    clear_current_node();

    esp_now_peer_num_t* numOfConnections;

    esp_now_get_peer_num(numOfConnections);

    if(numOfConnections->total_num > maxConnections) return false;

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
                connectedNodes[numOfConnections->total_num-1].peerName = peerSSID;
                connectedNodes[numOfConnections->total_num-1].peerInfo = *currentNode;
                Serial.print("Relay node ");
                Serial.print(peerSSID);
                Serial.println(" paired!");
                relaysAdded++;
            }

            clear_current_node();
        }
    }

    WiFi.scanDelete();

    return (relaysAdded > 0);
}

esp_err_t NODE::pair_node(const esp_now_peer_info_t* peer)
{
    return esp_now_add_peer((const esp_now_peer_info_t*)peer);
}

bool NODE::check_if_node_paired(const esp_now_peer_info_t* info)
{
    return esp_now_is_peer_exist(info->peer_addr);
}

void NODE::clear_current_node()
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
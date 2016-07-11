/*  NETPIE ESP8266 basic sample                            */
/*  More information visit : https://netpie.io             */

/**
 * Plugin Name:     Netpie Json File
 * Description:     Netpie Config from Json File
 * Author:          lamloei
 * Author URI:      http://www.lamloei.com
 * Modified Date:   2016071101
 * Version:         V01
 * TODO List:       2
 */

/**** TODO JSON #1/2 ****/
#include <ArduinoJson.h>
#include <ESP8266WiFi.h>
#include <MicroGear.h>
#include "FS.h"

char* config_file = "/config.json";

StaticJsonBuffer<400> jsonBuffer;
char json[300];

char ssid[20];
char password[20];
char APPID[40];
char KEY[60];
char SECRET[60];
char ALIAS[40];
/***********************/

WiFiClient client;
AuthClient *authclient;

int timer = 0;
MicroGear microgear(client);

/* If a new message arrives, do this */
void onMsghandler(char *topic, uint8_t* msg, unsigned int msglen) {
    Serial.print("Incoming message --> ");
    msg[msglen] = '\0';
    Serial.println((char *)msg);
}

void onFoundgear(char *attribute, uint8_t* msg, unsigned int msglen) {
    Serial.print("Found new member --> ");
    for (int i=0; i<msglen; i++)
        Serial.print((char)msg[i]);
    Serial.println();  
}

void onLostgear(char *attribute, uint8_t* msg, unsigned int msglen) {
    Serial.print("Lost member --> ");
    for (int i=0; i<msglen; i++)
        Serial.print((char)msg[i]);
    Serial.println();
}

/* When a microgear is connected, do this */
void onConnected(char *attribute, uint8_t* msg, unsigned int msglen) {
    Serial.println("Connected to NETPIE...");
    /* Set the alias of this microgear ALIAS */
    microgear.setName(ALIAS);
}

void setup() {
/**** TODO JSON #2/2 ****/
    SPIFFS.begin();
    File f = SPIFFS.open(config_file,"r");
    f.read((uint8_t*)json,f.size());
    f.close();

    JsonObject& root = jsonBuffer.parseObject(json);

    sprintf(ssid,root["ssid"]);
    sprintf(password,root["password"]);
    sprintf(APPID,root["APPID"]);
    sprintf(KEY,root["KEY"]);
    sprintf(SECRET,root["SECRET"]);
    sprintf(ALIAS,root["ALIAS"]); 
/***********************/

    /* Add Event listeners */

    /* Call onMsghandler() when new message arraives */
    microgear.on(MESSAGE,onMsghandler);

    /* Call onFoundgear() when new gear appear */
    microgear.on(PRESENT,onFoundgear);

    /* Call onLostgear() when some gear goes offline */
    microgear.on(ABSENT,onLostgear);

    /* Call onConnected() when NETPIE connection is established */
    microgear.on(CONNECTED,onConnected);

    Serial.begin(115200);
    Serial.println("Starting...");

    /* Initial WIFI, this is just a basic method to configure WIFI on ESP8266.                       */
    /* You may want to use other method that is more complicated, but provide better user experience */
    if (WiFi.begin(ssid, password)) {
        while (WiFi.status() != WL_CONNECTED) {
            delay(500);
            Serial.print(".");
        }
    }

    Serial.println("WiFi connected");  
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());

    /* Initial with KEY, SECRET and also set the ALIAS here */
    microgear.init(KEY,SECRET,ALIAS);

    /* connect to NETPIE to a specific APPID */
    microgear.connect(APPID);
}

void loop() {
    /* To check if the microgear is still connected */
    if (microgear.connected()) {
        Serial.println("connected");

        /* Call this method regularly otherwise the connection may be lost */
        microgear.loop();

        if (timer >= 1000) {
            Serial.println("Publish...");

            /* Chat with the microgear named ALIAS which is myself */
            microgear.chat(ALIAS,"Hello");
            timer = 0;
        } 
        else timer += 100;
    }
    else {
        Serial.println("connection lost, reconnect...");
        if (timer >= 5000) {
            microgear.connect(APPID);
            timer = 0;
        }
        else timer += 100;
    }
    delay(100);
}

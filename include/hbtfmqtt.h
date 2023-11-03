#ifndef HBTFMQTT_H
#define HBTFMQTT_H

#include <ESPAsyncWebServer.h>
#include <AsyncTCP.h>
#include "files.h"
#include "wlan.h"
#include "websrv.h"
#include "mqtt.h"

#include <SPI.h>
#include <AskSinPP.h>
#include <Device.h>

#include <Register.h>
#include <MultiChannelDevice.h>


// #define USE_HW_SERIAL
#define USE_RTC

#define LED_PIN                     32
#define CONFIG_BUTTON_PIN           33
#define CC1101_GDO0_PIN             2
#define CC1101_CS_PIN               5

#define PEERS_PER_CHANNEL           6
#define MSG_INTERVAL                180
#define MAX_SENSORS                 3
#define EXTRAMILLIS 730

#define DEVICEID                    String(ESP.getEfuseMac(),HEX)
#define DEVICETYPE                  "hbtfmqtt"
#define CONF_FILE                   "/config.cfg"

// WLAN
#define WLAN_INTERVAL               10000L  // (10 Sek)
#define AP_TIMEOUT                  180000L // (3 Min)
#define WLAN_FAILS                  5

// MQTT
#define TOPIC_CMD                   "cmnd"
#define TOPIC_WILL                  "will"
#define MQTT_INTERVAL               10000L

// SIZES
#define CONF_SIZE                   640

#ifdef HBTFMQTT_DEBUG
    #define DEBUG_PRINT(x) Serial.print(x)
    #define DEBUG_PRINTHEX(x) Serial.print(x, HEX)
    #define DEBUG_PRINTLN(x) Serial.println(x)
    #define DEBUG_PRINTF(x) Serial.printf(x)
#else
    #define DEBUG_PRINT(x)
    #define DEBUG_PRINTLN(x)
    #define DEBUG_PRINTF(x)
#endif

#endif
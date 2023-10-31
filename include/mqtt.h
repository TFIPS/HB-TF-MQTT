#ifndef MQTT_H
#define MQTT_H

#include <WiFi.h>
#include <PubSubClient.h>

#include "hbtfmqtt.h"

class MQTT
{
    protected:
        WiFiClient      _espClient;
        PubSubClient    _mqttClient;

        void _callback(char* _topic, byte* _message, unsigned int _length);

    private:
        static MQTT *instance;
        MQTT(){};

    public:
        String mTemp1 	= "0";
        String mHum1	= "0";
        String mTemp2	= "0";
        String mHum2	= "0";
        String mTemp3	= "0";
        String mHum3	= "0";
        
        static MQTT *getInstance() 
        {
            if (!instance)
            instance = new MQTT;
            return instance;
        }
        
        bool ready      = false;

        struct mqttData
        {
            String topic;
            String cmnd;
            String message;
        };
        mqttData mdata;
       
        void setup();
        void loop();
        void initMQTT();
        void send(String _topic, String _message);
        void disconnect();
};
#endif
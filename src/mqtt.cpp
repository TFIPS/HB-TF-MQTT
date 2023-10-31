#define HBTFMQTT_DEBUG
#include "mqtt.h"

MQTT *MQTT::instance = 0;

void MQTT::setup()
{
    _mqttClient.setClient(_espClient);
    _mqttClient.setCallback(std::bind(&MQTT::_callback, this
        , std::placeholders::_1
        , std::placeholders::_2
        , std::placeholders::_3)
    );
}

void MQTT::loop()
{
    initMQTT();
    _mqttClient.loop();
}

void MQTT::initMQTT()
{
    if(!_mqttClient.connected() || !ready)
    {
        ready = false;
        FILES* files = FILES::getInstance();
        DynamicJsonDocument config 	= files->getConfig();
        config.shrinkToFit();
        /*
            DEBUG_PRINTLN("MQTT Host: "+config["mqttHost"].as<String>());
            DEBUG_PRINTLN("MQTT Username: "+config["mqttUsername"].as<String>());
            DEBUG_PRINTLN("MQTT Password: "+config["mqttPassword"].as<String>());
            DEBUG_PRINTLN("Topic: "+String(TOPIC)+"/"+String(TOPIC_CMD));
        */

        if(!config["mqttHost"].isNull() && !config["mqttPort"].isNull() && !config["mqttTopic"].isNull())
        {
            _mqttClient.setServer(config["mqttHost"].as<const char *>(), config["mqttPort"].as<int>());
            const char* deviceName = (String(DEVICETYPE)+"-"+DEVICEID).c_str();
            if (_mqttClient.connect(deviceName, config["mqttUsername"].as<const char *>(), config["mqttPassword"].as<const char *>()))
            {
                const char* topic = String(config["mqttTopic"].as<String>()+ "/" TOPIC_CMD "/#").c_str();
                _mqttClient.subscribe(topic);
                ready = true;
            } 
            else 
            {
                ready = false;
                delay(MQTT_INTERVAL);
            }
        }
        config.clear();
    }
}

void MQTT::send(String _topic, String _message)
{
    if(ready)
    {
        FILES* files = FILES::getInstance();
        DynamicJsonDocument config 	= files->getConfig();
        _mqttClient.publish((config["mqttTopic"].as<String>()+"/"+_topic).c_str(), _message.c_str());
        delay(500);
    }
}

void MQTT::_callback(char* _topic, byte* _message, unsigned int _length) 
{
    FILES* files = FILES::getInstance();
    DynamicJsonDocument config 	= files->getConfig();
	config.shrinkToFit();
    mdata               = {};
    String topic        = String(_topic);
    String thisTopic    = config["mqttTopic"].as<String>()+"/" TOPIC_CMD "/";
    if(topic.startsWith(thisTopic))
    {
        String messageFinal;
        for (int i=0;i<_length;i++) 
        {
            messageFinal += (char)_message[i];
        }
        mdata = {thisTopic, topic.substring(thisTopic.length()), messageFinal};
        messageFinal.clear();
    }
    DEBUG_PRINTLN(mdata.cmnd);
    DEBUG_PRINTLN(mdata.message);
    if(mdata.cmnd == "temp1")
	{
		mTemp1 = mdata.message;
		DEBUG_PRINTLN("Temp1 wurde auf: "+mTemp1+" gestellt");
	}
	else if(mdata.cmnd == "hum1")
	{
		mHum1 = mdata.message;
		DEBUG_PRINTLN("Hum1 wurde auf: "+mHum1+" gestellt");
	}

	else if(mdata.cmnd == "temp2")
	{
		mTemp2 = mdata.message;
		DEBUG_PRINTLN("Temp2 wurde auf: "+mTemp2+" gestellt");
	}
	else if(mdata.cmnd == "hum2")
	{
		mHum2 = mdata.message;
		DEBUG_PRINTLN("Hum2 wurde auf: "+mHum2+" gestellt");
    }

    else if(mdata.cmnd == "temp3")
    {
        mTemp3 = mdata.message;
        DEBUG_PRINTLN("Temp3 wurde auf: "+mTemp3+" gestellt");
    }
    else if(mdata.cmnd == "hum3")
    {
        mHum3 = mdata.message;
        DEBUG_PRINTLN("Hum3 wurde auf: "+mHum3+" gestellt");
    }
    mdata = {};
}

void MQTT::disconnect()
{
    if(ready)
    {
        ready = false;
        _mqttClient.disconnect();
    }
}